/*
	 GPS Protocol Translator Firmware
    Copyright (C)2008 Argent Data Systems - All Rights Reserved
    
    Port 1 - NMEA
    Port 2 - Garmin
    
    Digit 1 - Port A
    Digit 2 - Port B
    Digit 3 - Port D
    
    0 - A
    1 - B
    2 - C
    3 - D
    4 - E
    5 - F
    6 - G
    7 - DP
    
    
*/

#define GPS_TIMEOUT 30

#pragma DATA_SEG SHORT _DATA_ZEROPAGE
#pragma CONST_SEG CONST_FLASH

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "status.h"
#include "utility.h"
#include "hardware.h"
#include "garmin.h"
#include "events.h"

unsigned near volatile int events,	// Pending system-wide events
                           clock_ticks,
                           flags,			// Flags for keeping track of various state
            					second;			// Second-of-hour for timeslotting

// General-purpose scratch pad variables
unsigned near char ctemp[10];					// Multi-use temp shared with GPS code

volatile unsigned char queue_a_len, queue_b_len, qa, qb, qd;

#pragma DATA_SEG SHORT DEFAULT

#define SERIAL_QUEUE_LEN 254

unsigned char queue_a[SERIAL_QUEUE_LEN];
unsigned char queue_b[SERIAL_QUEUE_LEN];

unsigned char scratch[255];

void once_per_second(void);

unsigned volatile char digit[3], phase;
unsigned short eta_distance;

const char lcd_lookup[] = {
	0b00111111,	// 0
	0b00000110,	// 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111  // 9
};


// Main program entry point

void main(void)
{
	mcu_setup();
	digit[0] = digit[1] = digit[2] = 0xff;
	EnableInterrupts;
   wait(TICKS_PER_SEC/2);
   
	garmin_init();	

	// Main program loop.  Only exit is to bootloader on reset command.
	for (;;)
	{

		reset_watchdog;

		// Process pending data in port A serial queue
		if (queue_a_len)
		{
			queue_a_len = 0;
		}

		// Process pending data in port B serial queue
		if (queue_b_len)
		{
			qd = queue_b[qb];
			// Abort processing if byte wasn't handled
			if (!garmin_rx_byte(qd)) qb++;
			DisableInterrupts;
			if (qb == queue_b_len) qb = queue_b_len = 0;
			EnableInterrupts;
		}

		// Handle Garmin protocol events
		if (CheckEvent(EVENT_GARMIN))
		{
			ClearEvent(EVENT_GARMIN);
			garmin_handler();
		}

		// Process once-per-second events
		if (CheckEvent(EVENT_SECOND))
		{
			ClearEvent(EVENT_SECOND);
			once_per_second();
		}
	}
}



#pragma mark -


// --- Interrupt Handlers ---

// SCI receive handler
interrupt void RXhandler(void)
{
	unsigned char scidata;
	if (!(SCI1S1 & 0x20)) return;						// Make sure this is a rx full event
	scidata = SCI1D;
	if (queue_a_len == SERIAL_QUEUE_LEN) return;	// Must have room in the serial queue
	queue_a[queue_a_len++] = scidata;				// Get received character
}

// Port B SCI receive handler
interrupt void RXhandler2(void)
{
	unsigned char scidata;
	if (!(SCI2S1 & 0x20)) return;					// Make sure this is a rx full event
	scidata = SCI2D;
	if (queue_b_len == SERIAL_QUEUE_LEN) return;	// Must have room in the serial queue
	queue_b[queue_b_len++] = scidata;				// Get received character
}


// Null handler, to catch any stray interrupts

interrupt void NullHandler(void)
{
}


// IRQ handler - triggered by falling edge on IRQ pin

interrupt void IRQHandler(void)
{
}

// Timebase interrupt handler - called (ticks_per_sec) times per second

interrupt void tick(void)
{
	// Handle 150 hz tick counter

	SRTISC |= 0x40; // Acknowledge RTI

	clock_ticks++;
	if (wait_ticks) wait_ticks--;
	if (clock_ticks == TICKS_PER_SEC)
	{
		RaiseEvent(EVENT_SECOND);
		second++;
		clock_ticks = 0;
	}		

	// LCD driver

	phase++;
	if (phase & 2)			// 37.5 hz frame rate
	{
		PTED = 4;
		PTAD = ~digit[0];
		PTBD = ~digit[1];
		PTDD = ~digit[2];
	}
	else
	{
		PTED = 0;
		PTAD = digit[0];
		PTBD = digit[1];
		PTDD = digit[2];
	}
} 

// Do once-per-second stuff here
void once_per_second(void)
{	
//	count++;
//	if (count > 999) count = 0;

	unsigned short disp;
	disp = eta_distance;
	if (disp > 9999) disp = 0;	// Default is 0 (goes off guidance on arrival)

	DisableInterrupts;
	if (eta_distance <= 999)
	{
		// Tenths of miles
		digit[2] = lcd_lookup[disp % 10];
		digit[1] = lcd_lookup[(disp / 10) % 10] | 0x80;	// Add decimal point
		digit[0] = lcd_lookup[disp / 100];
	}
	else
	{
		// Whole miles
		disp = disp / 10;
		digit[2] = lcd_lookup[disp % 10];
		digit[1] = lcd_lookup[(disp / 10) % 10];
		digit[0] = lcd_lookup[disp / 100];
	
	}
	// Suppress leading zeros
	if (digit[0] == lcd_lookup[0])
	{
		digit[0] = 0;
		if (digit[1] == lcd_lookup[0]) digit[1] = 0;
	}
	
	EnableInterrupts;
	if (++second >= 3600) second = 0;
	
	garmin_timer();
}

