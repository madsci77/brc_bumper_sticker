// Utility routines
// void sleep(unsigned int ticks);

#pragma DATA_SEG SHORT _DATA_ZEROPAGE
#pragma CONST_SEG CONST_FLASH

#include "utility.h"
#include "hardware.h"
#include "status.h"
#include "ctype.h"

unsigned near char led_on_period;	// How many ticks the LED stays on
unsigned near char led_off_period;	// How many ticks the LED stays off
unsigned near char led_counter;		// Counter for LED, updated every tick
unsigned near char led_mode; 		// LED mode and count - bit 7 = current state, bit 6 = continuous

volatile unsigned near int wait_ticks;		// Counts down every tick if not zero already
long near ltemp;

// Constants for converting lat/lon to semicircles
const long valtable[]  = {1193046471L, 119304647L, 11930465L, 1988411L,
   	                     198841L, 19884L, 1988L, 199L, 20L, 2L};

// Takes degrees and fractional minutes and returns 2^31/180 degrees
// West and South values are negative
long semicircles(char *degstr, char hemi)
{
	char dgt = 0, *p;
	ltemp = 0;
	if (degstr[4] == '.') dgt++;		// Skip hundreds place if we know we're doing latitude
	p = degstr;
	for (;dgt<10; p++)
	{
		if (*p == '.') continue;
		if (!isdigit(*p)) break;
		ltemp += (*p & 0x0f) * valtable[dgt];
		dgt++;
	}
	if (hemi) return -ltemp;
	return ltemp;
}

void semicircledecode(signed long sslat, signed long sslon, unsigned char *lat, unsigned char *lon,
                  unsigned char *ns, unsigned char *ew)
{
	*ns = 'N';
	*ew = 'E';

	if (sslat < 0)
	{
		sslat = 0 - sslat;
		*ns = 'S';
	}
	
	if (sslon < 0)
	{
		sslon = 0 - sslon;
		*ew = 'W';
	}
	
	// Convert to decimal
	for (lat[0] = '0'; sslat >= valtable[1]; sslat -= valtable[1]) lat[0]++;
	for (lat[1] = '0'; sslat >= valtable[2]; sslat -= valtable[2]) lat[1]++;
	for (lat[2] = '0'; sslat >= valtable[3]; sslat -= valtable[3]) lat[2]++;
	for (lat[3] = '0'; sslat >= valtable[4]; sslat -= valtable[4]) lat[3]++;
	lat[4] = '.';
	for (lat[5] = '0'; sslat >= valtable[5]; sslat -= valtable[5]) lat[5]++;
	for (lat[6] = '0'; sslat >= valtable[6]; sslat -= valtable[6]) lat[6]++;
	for (lat[7] = '0'; sslat >= valtable[7]; sslat -= valtable[7]) lat[7]++;
	for (lat[8] = '0'; sslat >= valtable[8]; sslat -= valtable[8]) lat[8]++;
	if (lat[8] > '9') lat[8] = '9';
	
	// Convert to decimal
	for (lon[0] = '0'; sslon >= valtable[0]; sslon -= valtable[0]) lon[0]++;
	for (lon[1] = '0'; sslon >= valtable[1]; sslon -= valtable[1]) lon[1]++;
	for (lon[2] = '0'; sslon >= valtable[2]; sslon -= valtable[2]) lon[2]++;
	for (lon[3] = '0'; sslon >= valtable[3]; sslon -= valtable[3]) lon[3]++;
	for (lon[4] = '0'; sslon >= valtable[4]; sslon -= valtable[4]) lon[4]++;
	lon[5] = '.';
	for (lon[6] = '0'; sslon >= valtable[5]; sslon -= valtable[5]) lon[6]++;
	for (lon[7] = '0'; sslon >= valtable[6]; sslon -= valtable[6]) lon[7]++;
	for (lon[8] = '0'; sslon >= valtable[7]; sslon -= valtable[7]) lon[8]++;
	for (lon[9] = '0'; sslon >= valtable[8]; sslon -= valtable[8]) lon[9]++;
	if (lon[9] > '9') lon[9] = '9';
}

// Converts altitude in 1/100 meters to fixed 6-character ascii value in feet.
void alt_feet(long alt, char *s)
{
	// 3.28084 feet per meter, or 78/256ths (but alt is in centimeters)
	static const unsigned long places[6] = {780287975L, 78028798L, 7802880L, 780288L, 78029L, 7803L};
	char c;

	alt <<= 8;
	for (c=0; c<6; c++)
	{
		s[c] = '0';
		while (alt >= places[c])
		{
			alt -= places[c];
			s[c]++;
		}
	}
}

// Send a string the the serial port
void sci_print(const char *c)
{
	while (*c)
	{
		sci1_write(*c);
		c++;
	}
}

// Send a string the the serial port
void sci_printn(const char *c, unsigned char n)
{
	while (n-- && *c)
	{
		sci1_write(*c);
		c++;
	}
}


// Wait specified number of clock ticks
void wait(unsigned int ticks)
{
	wait_ticks = ticks;
	while (wait_ticks)
	{
		Wait;
		reset_watchdog;
	}		
}

// Convert two ascii digits to char
char ddtoc(char *val)
{
	return ((val[0]&0x0f) * 10) + (val[1]&0x0f);
}

// Convert three ascii digits to int
unsigned int tdtoi(char *val)
{
	return ((int)(val[0]-'0') * 100) + ((val[1]-'0') * 10) + val[2]-'0';
}

// Convert hex digit to value
unsigned char hexval(unsigned char c)
{
	if (c >= '0' && c <= '9') return c & 0x0f;
	return (c & 0x0f) + 9;
}

// Convert 2-digit hex to char
unsigned char hexbyte(char *val)
{
	unsigned char h;
	h = hexval(val[0]) << 4;
	h |= hexval(val[1]);
	return h;
}

// Convert 4-digit hex to int
unsigned int hexint(char *val)
{
	unsigned int h;
	h = hexval(val[0]) << 12;
	h |= hexval(val[1]) << 8;
	h |= hexval(val[2]) << 4;
	h |= hexval(val[3]);
	return h;
}

// Case-insensitive compare
unsigned char strcasecmp(const char *s1, const char *s2)
{
	while (*s1 && *s2)
	{
		if (toupper(*s1) != toupper(*s2)) return 1;
		s1++;
		s2++;
	}
	if (*s1 != *s2) return 1;
	return 0;
}

unsigned char strncasecmp(const char *s1, const char *s2, unsigned char n)
{
	while (*s1 && *s2 && n--)
	{
		if (toupper(*s1) != toupper(*s2)) return 1;
		s1++;
		s2++;
	}
	if (!n) return 0;
	if (toupper(*s1) != toupper(*s2)) return 1;
	return 0;
}


const char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void sci_writehex(unsigned char c) {
	sci1_write(hex[(c & 0xf0) >> 4]);
	sci1_write(hex[c & 0x0f]);
}

void sci_writehex_b(unsigned char c) {
	sci2_write(hex[(c & 0xf0) >> 4]);
	sci2_write(hex[c & 0x0f]);
}

void calc_digit(char *d, unsigned int *dv, unsigned int ds)
{
	*d = '0';
	while (*dv >= ds)
	{
		*d = (*d)+1;
		*dv -= ds;
	}
}


#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME
#pragma NO_RETURN

void wait25(void)
{
	// Wait 25 clock cycles.  Assume 9 cycles for jump and return.
	asm
	{
		BRN	0
		BRN 0
		BRN 0
		BRN 0
		BRN 0
		NOP
		RTS
	}
}

#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME
#pragma NO_RETURN

void wait10(void)
{
	// Wait 10 clock cycles.  Assume 9 cycles for jump and return.
	asm
	{
		NOP
		RTS
	}
}

char *itoa (int v, char *s)
{
	int i,neg = 0;
	char *p = s;
	char *q = s;

	if (v == 0) {				// if value is 0, return 0 and null-terminate
		*p++ = '0';
		*p = 0;
		return (s);
		}
	if (v < 0) {				// if value is negative, set neg flag and negate
		neg = 1;
		v = -v;
		}
	while (v > 0) {
		i = v % 10;				// get value mod radix
		*p++ = '0' + i;			// write ascii value of digit
		v /= 10;				// divide value by radix to get next place
		}
	if (neg) *p++ = '-';		// if it's negative, append a minus sign
	*p-- = 0;					// null-terminate string
	q = s;						// reverse string
	while (p > q) {
		i = *q;
		*q++ = *p;
		*p-- = i;
		}
	return (s);
}


void itodec5(unsigned int i, char *s)
{
	unsigned int v;
	v = i;
	s[0] = s[1] = '0';
	while (v >= 10000)
	{
		s[0]++;
		v -= 10000;
	}
	while (v >= 1000)
	{
		s[1]++;
		v -= 1000;
	}
	itodec(v, s+2);
}


void itodec(unsigned int i, char *s)
{
	unsigned int v;
	v = i;
	s[0] = s[1] = s[2] = '0';
	while (v >= 100)
	{
		s[0]++;
		v -= 100;
	}
	while (v >= 10)
	{
		s[1]++;
		v -= 10;
	}
	while (v)
	{
		s[2]++;
		v--;
	}	
}