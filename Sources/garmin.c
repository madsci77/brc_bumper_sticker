/* Garmin protocol routines

   Notes on Garmin binary interface protocol:
   
   Physical layer is 9600-N-8-1.   
   Packets have the format: <DLE> <PID> <Length> <Payload> <Checksum> <DLE> <ETX>
   All fields are single bytes, except for the variable-length payload.
   Commands should use a minimum of 2 bytes in payload for compatibility.
   DLE characters in the packet are doubled.

ETA A603 protocol notes

0x0200 - Server to client ETA request
0x0201 - Client to server ETA data
0x0202 - Server to client ETA receipt

Receipt includes only the unique ID

*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "garmin.h"
#include "hardware.h"
#include "utility.h"
#include "math.h"
#include "main.h"
#include "events.h"
#include "status.h"
#include "geo.h"

#define GARMIN_BUFFER_SIZE 100
#define PVT_TIMEOUT 2
#define FMI_TIMEOUT 4
#define PRODUCT_RQST_RETRY 3

#define STATE_DLE_WAIT 0
#define STATE_PID 1
#define STATE_LENGTH 2
#define STATE_PAYLOAD 3
#define STATE_CHECKSUM 4
#define STATE_END_DLE 5
#define STATE_ETX 6
#define STATE_PROCESS 7

time_t utc;
struct tm *ptm;
long latitude, longitude;
extern unsigned short eta_distance;

// Wrapper to send byte to appropriate port(s)
void garmin_outc(unsigned char c)
{
	sci2_write(c);
}

void process_garmin(unsigned char pid, char *payload, unsigned char length);

volatile unsigned char pending_ack, pvt_timer, fmi_timer, gflags;
unsigned char state = STATE_DLE_WAIT;
unsigned char wpt_protocol = WPT_UNKNOWN;
unsigned long fmi_msg_id = 0xffffff;

static unsigned char pid, length, calc_checksum;
static unsigned char payload[GARMIN_BUFFER_SIZE];
extern unsigned char scratch[];

// Initialize Garmin protocol interface
void garmin_init(void)
{
	pending_ack = 0;
	gflags &= ~GF_FMI_ON;
	fmi_timer = FMI_TIMEOUT;
	pvt_timer = PVT_TIMEOUT;
	state = STATE_DLE_WAIT;
	garmin_send_packet(Pid_FMI, "\x00\x00", 2); // Enable fleet management interface
}

// Handle Garmin protocol events
void garmin_handler(void)
{
	unsigned char acked_pid[2];

	if (pending_ack)
	{
		acked_pid[0] = pending_ack;
		acked_pid[1] = 0;
		garmin_send_packet(Pid_Ack_Byte, acked_pid, 2);
		pending_ack = 0;
	}

	if (state == STATE_PROCESS)
	{
		// Process the payload
		process_garmin(pid, payload, length);
		state = STATE_DLE_WAIT;
	}

	// Alternate between FMI start and queue check messages
	if (state == STATE_DLE_WAIT && fmi_timer == 0)
	{
		garmin_send_packet(Pid_FMI, "\x00\x00", 2); // Enable fleet management interface
		fmi_timer = FMI_TIMEOUT;
		gflags ^= GF_FMI_MSG;
	}
	// Send periodic ETA request
	if (state == STATE_DLE_WAIT && !pvt_timer)
	{
		pvt_timer = PVT_TIMEOUT;
		garmin_send_packet(Pid_FMI, "\x00\x02", 2); // Request ETA
	}
}

// Once per second handler
void garmin_timer(void)
{
	if (pvt_timer) pvt_timer--;
	else RaiseEvent(EVENT_GARMIN);
	if (fmi_timer) fmi_timer--;
	else RaiseEvent(EVENT_GARMIN);
}

// Process a single received byte - called from SCI ISR
unsigned char garmin_rx_byte(unsigned char s)
{
	static unsigned char esc, remaining, *p;

	// Keep bytes queued if we're busy processing
	if (state == STATE_PROCESS)
	{
		return 1;
	}
	
	// Handle DLE stuffing (DLEs in payload get doubled on transmission)
	if (state != STATE_DLE_WAIT && state != STATE_END_DLE)
	{
		if (s == 0x10 && !esc)
		{
			esc = 1;
			return 0;
		}
	}
	if (esc)
	{
		// Check for framing error
		if (s != 0x10)
		{
			state = STATE_PID;
			calc_checksum = 0;
		}
	}
	esc = 0;
	
	// Packet checksum - start at zero and subtract each byte after DLE
	calc_checksum -= s;

	// Link level protocol state machine	
	switch (state)
	{
	case STATE_DLE_WAIT:
		// Waiting for start of packet
		if (s == 0x10) state = STATE_PID;
		calc_checksum = 0;
		esc = 0;
		break;
	case STATE_PID:
		// Receive Packet ID - if it's a DLE, stay here
		pid = s;
		if (s != 0x10) state = STATE_LENGTH;
		break;
	case STATE_LENGTH:
		// Receive paload length
		length = remaining = s;
		p = payload;
		// TODO: limit rx size to buffer size
		if (!length) state = STATE_CHECKSUM;
		else state = STATE_PAYLOAD;
		if (length > GARMIN_BUFFER_SIZE) length = GARMIN_BUFFER_SIZE;
		break;
	case STATE_PAYLOAD:
		// Receiving payload - add to buffer, decrement remaining counter
		if (p < (payload + GARMIN_BUFFER_SIZE))
		{
			*p = s;
			p++;
		}
		*p = 0; // Null-terminate
		remaining--;
		if (!remaining) state = STATE_CHECKSUM;
		break;
	case STATE_CHECKSUM:
		// Verify that the checksum matches what we got
		if (calc_checksum) pid = 0;
		
		state = STATE_END_DLE;
		break;
	case STATE_END_DLE:
		// Next byte should be ending DLE
		if (s == 0x10) state = STATE_ETX;
		break;
	case STATE_ETX:
		// Last byte is ETX
		if (s == 0x03)
		{
			state = STATE_DLE_WAIT;
			// Exit if we didn't get a valid checksum
			if (!pid) break;
			// Ignore ACK and NAK bytes
			if (pid != Pid_Ack_Byte && pid != Pid_Nak_Byte)
			{
				if (pid != Pid_Pvt_Data)
				{
					// Queue ACK for transmission in main loop (excluding PVT)
					pending_ack = pid;
				}
				// Main loop calls message processor
				RaiseEvent(EVENT_GARMIN);
				state = STATE_PROCESS;
			}
		}
		break;
	case STATE_PROCESS:
		// Waiting for main loop to process incoming data
		break;
	default:
		// We shouldn't be here!  Back to start
		state = STATE_DLE_WAIT;
		break;
	}
	return 0;
}

// Process received packet
void process_garmin(unsigned char pid, char *payload, unsigned char length)
{
	if (pid == Pid_FMI)
	{
		fmi_timer = FMI_TIMEOUT;
		gflags |= GF_FMI_ON;
		if (payload[0] == 0x01 && payload[1] == 0x02)
		{
			unsigned long m, tmi;	// 1609.34 meters per mile
			m = payload[10];
			m += (unsigned long)payload[11] << 8;
			m += (unsigned long)payload[12] << 16;
			m += (unsigned long)payload[13] << 24;
			tmi = (m << 8) / 41199;
			eta_distance = (unsigned short)tmi;
//			sprintf(scratch, "%ld meters, %ld tenth miles\r\n", m, tmi);
			// ACK an ETA packet
			payload[0] = 0x02;	// Just change the ID and send the first part back out
			garmin_send_packet(Pid_FMI, &payload[2], 4);
//			sci_print(scratch);
		}
	}		
}

// Send a Garmin protocol packet with the specified PID and payload.
// Garmin says even empty packets should have a 2-byte payload.
void garmin_send_packet(unsigned char pid, unsigned char *payload, unsigned char length)
{
	unsigned char checksum;
	garmin_outc(0x10); // DLE
	garmin_outc(pid);
	garmin_outc(length);
	checksum = 0-(signed char)pid-length;
	for (; length; length--, payload++)
	{
		garmin_outc(*payload);
		// Double DLEs in payload
		if (*payload == 0x10) garmin_outc(0x10);
		checksum -= *payload;
	}
	garmin_outc(checksum);
	garmin_outc(0x10); // DLE
	garmin_outc(0x03); // ETX
}

