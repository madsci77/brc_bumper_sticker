#include "hardware.h"


// Hardware-specific code for SR1F simplex repeater

void mcu_setup(void)
{
	// Watchdog enabled, long period, STOP enabled
	SOPT = 0xe0;	
	SPMSC1 = 0x1d;				// Enable bandgap buffer (for battery measurement)

//	SOPT = 0x20;								// STOP enabled, COP disabled
//	SPMSC1 = 0x00;								// LVD disabled	

	// Set all unused pins to output/low

	PTADD = PTBDD = PTCDD = PTDDD = PTEDD = PTFDD = PTGDD = 0xff;
	PTAD = PTBD = PTCD = PTDD = PTED = PTFD = PTGD = 0;
	PTBD = 0;
	PTBDD = 0xff;	
	PTEDD = 0x04;
	
	// Clock generator setup

	ICGC1 = 0xf8;				// High power, high range, FLL engaged
	ICGC2 = 0x20;				// N = 8, 4.9152 MHz * 8 / 2 = 19.6608 MHz
	while ((ICGS1 & 0xc0) != 0xc0) reset_watchdog;
		
	SRTISC = 0x37; // External crystal / 32768

	// SCI setup - baud rate clock 1228800 hz

	SCI1BDH = 1;
	SCI1BDL = 0;			   // 4800 baud (1228800/256 = 4800)
	SCI1C1 = 0;
	SCI1C2 = 0x2c;				// TX on, RX on

	SCI2BDH = 0;
	SCI2BDL = 128;				// 9600 baud (1228800/128 = 9600)
	SCI2C1 = 0;
	SCI2C2 = 0x2c;				// TX on, RX on
	SCI2C3 = 0x10;          // TX invert	

}

void sci1_write(unsigned char d)
{
	while (!(SCI1S1 & 0x80)) reset_watchdog;
	SCI1D = d;
}

void sci2_write(unsigned char d)
{
	while (!(SCI2S1 & 0x80)) reset_watchdog;
	SCI2D = d;
}

unsigned char readadc8(unsigned char channel)
{
	// Channel 26 = temperature sensor, 27 = 1.2v nominal bandgap
 	ADC1SC1 = 0x1f & channel;
 	while ((ADC1SC1 & 0x80) == 0) reset_watchdog;
 	return ADC1RL;
}