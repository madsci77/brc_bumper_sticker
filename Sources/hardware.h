/*

Hardware-specific code for SR1F simplex repeater

Hardware Assignments:

IRQ = Squelch detect
PTB0 = BDM header pin 5
PTB1 = BDM header pin 3
PTB2 = Red LED
PTB3 = Green LED
PTF0/T1CH2 = Audio out
PTF1 = DIP switch 3 (needs pull-up)
PTE4/SS1 = Flash 1 CS
PTE5/MISO = SPI In
PTE6/MOSI = SPI Out
PTE7/SPSCK = SPI Clock
PTG0 = Flash 2 CS
PTD7/ADC15 = Audio in
PTD3 = PTT out
PTC4 = COR in (needs pull-up)

Radio connector (RJ45):

Looking at front panel, from left to right (pins on bottom):
1 - Power  2 - N/C  3 - Audio in  4 - PTT  5 - GND  6 - Audio out  7 - GND  8 - COR in

External storage is two Winbond W25P16 16 mbit serial flash memories
64k sector erase in 600 msec, write speed ~71 kbytes/sec (256 byte page in 3.5 msec)
SPI modes 0 and 3 supported

*/

#include "hidef.h"
#include "aw32.h"

#define RED_ON (PTBD |= 4)
#define RED_OFF (PTBD &= 0xfb)
#define RED_TOGGLE (PTBD ^= 4)
#define GREEN_ON (PTBD |= 8)
#define GREEN_OFF (PTBD &= 0xf7)
#define GREEN_TOGGLE (PTBD ^= 8)

#define DIP_SWITCH ((PTFD & 2) == 0)

#define PTT_ON (PTDD |= 8)
#define PTT_OFF (PTDD &= 0xf7)

#define SWITCH_DOWN ((PTFD & 2) == 0)

#define FLASH1_ENABLE {PTED &= ~0x10;}
#define FLASH1_DISABLE {PTED |= 0x10;}
#define FLASH2_ENABLE {PTGD &= ~0x01;}
#define FLASH2_DISABLE {PTGD |= 0x01;}

#define TICKS_PER_SEC 150
#define PWM_RATE 76800
#define PWM_MULT 1.171875

void mcu_setup(void);
unsigned char spi_byte(unsigned char in);
void do_tone(unsigned int freq, unsigned int time);
void power_down(void);
void power_up(void);
void sci1_writehex(unsigned char c);
void sci1_write(unsigned char d);
void sci2_write(unsigned char d);
void audio_out(unsigned char v);
unsigned char readadc8(unsigned char channel);
