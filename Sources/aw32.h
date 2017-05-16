#pragma ONCE
#define M9S08AW32

#define Wait {__asm WAIT;}
#define Stop {__asm STOP;}
#define NOP3 {__asm BRN 0;}

#define reset_watchdog {__asm sta 0x1800;}

  #define SRTISC      (*(volatile unsigned char*)(0x1808))  /* RTI status and control */

/*  IO DEFINITIONS FOR MC9S08AW32 */

/*  PORTS section */

  #define PTAD      (*(volatile unsigned char*)(0x00))  /* port A */
  #define PTADD     (*(volatile unsigned char*)(0x01))  /* data direction port A */

  #define PTBD      (*(volatile unsigned char*)(0x02))  /* port B */
  #define PTBDD     (*(volatile unsigned char*)(0x03))  /* data direction port B */

  #define PTCD      (*(volatile unsigned char*)(0x04))  /* port C */
  #define PTCDD     (*(volatile unsigned char*)(0x05))  /* data direction port C */

  #define PTDD      (*(volatile unsigned char*)(0x06))  /* port D */
  #define PTDDD     (*(volatile unsigned char*)(0x07))  /* data direction port D */    

  #define PTED      (*(volatile unsigned char*)(0x08))  /* port D */
  #define PTEDD     (*(volatile unsigned char*)(0x09))  /* data direction port D */    

  #define PTFD      (*(volatile unsigned char*)(0x0a))  /* port D */
  #define PTFDD     (*(volatile unsigned char*)(0x0b))  /* data direction port D */    

  #define PTGD      (*(volatile unsigned char*)(0x0c))  /* port D */
  #define PTGDD     (*(volatile unsigned char*)(0x0d))  /* data direction port D */    

  #define PTADS     (*(volatile unsigned char*)(0x1842))  /* Drive strength, port A */
  #define PTBDS     (*(volatile unsigned char*)(0x1846))  /* Drive strength, port B */
  #define PTCDS     (*(volatile unsigned char*)(0x184A))  /* Drive strength, port C */
  #define PTDDS     (*(volatile unsigned char*)(0x184E))  /* Drive strength, port D */
  #define PTEDS     (*(volatile unsigned char*)(0x1852))  /* Drive strength, port E */
  #define PTFDS     (*(volatile unsigned char*)(0x1856))  /* Drive strength, port F */
  #define PTGDS     (*(volatile unsigned char*)(0x185A))  /* Drive strength, port G */

  #define PTAPE     (*(volatile unsigned char*)(0x1840))  /* Pull-up enable, port A */
  #define PTBPE     (*(volatile unsigned char*)(0x1844))  /* Pull-up enable, port B */
  #define PTCPE     (*(volatile unsigned char*)(0x1848))  /* Pull-up enable, port C */
  #define PTDPE     (*(volatile unsigned char*)(0x184C))  /* Pull-up enable, port D */
  #define PTEPE     (*(volatile unsigned char*)(0x1850))  /* Pull-up enable, port E */
  #define PTFPE     (*(volatile unsigned char*)(0x1854))  /* Pull-up enable, port F */
  #define PTGPE     (*(volatile unsigned char*)(0x1858))  /* Pull-up enable, port G */



// ADC

  #define ADC1SC1    (*(volatile unsigned char*)(0x10))
  #define ADC1SC2    (*(volatile unsigned char*)(0x11))
  #define ADC1RH     (*(volatile unsigned char*)(0x12))
  #define ADC1RL     (*(volatile unsigned char*)(0x13))
  #define ADC1CVH    (*(volatile unsigned char*)(0x14))
  #define ADC1CVL    (*(volatile unsigned char*)(0x15))
  #define ADC1CFG    (*(volatile unsigned char*)(0x16))
  #define APCTL1     (*(volatile unsigned char*)(0x17))
  #define APCTL2     (*(volatile unsigned char*)(0x18))
  #define APCTL3     (*(volatile unsigned char*)(0x19))
    
// Timers

  #define TPM1SC    (*(volatile unsigned char*)(0x20))
  #define TPM1CNTH  (*(volatile unsigned char*)(0x21))
  #define TPM1CNTL  (*(volatile unsigned char*)(0x22))
  #define TPM1MODH  (*(volatile unsigned char*)(0x23))
  #define TPM1MODL  (*(volatile unsigned char*)(0x24))
  #define TPM1C0SC  (*(volatile unsigned char*)(0x25))
  #define TPM1C0VH  (*(volatile unsigned char*)(0x26))
  #define TPM1C0VL  (*(volatile unsigned char*)(0x27))
  #define TPM1C1SC  (*(volatile unsigned char*)(0x28))
  #define TPM1C1VH  (*(volatile unsigned char*)(0x29))
  #define TPM1C1VL  (*(volatile unsigned char*)(0x2a))
  #define TPM1C2SC  (*(volatile unsigned char*)(0x2b))
  #define TPM1C2VH  (*(volatile unsigned char*)(0x2c))
  #define TPM1C2VL  (*(volatile unsigned char*)(0x2d))
  #define TPM1C3SC  (*(volatile unsigned char*)(0x2e))
  #define TPM1C3VH  (*(volatile unsigned char*)(0x2f))
  #define TPM1C3VL  (*(volatile unsigned char*)(0x30))
  #define TPM1C4SC  (*(volatile unsigned char*)(0x31))
  #define TPM1C4VH  (*(volatile unsigned char*)(0x32))
  #define TPM1C4VL  (*(volatile unsigned char*)(0x33))
  #define TPM1C5SC  (*(volatile unsigned char*)(0x34))
  #define TPM1C5VH  (*(volatile unsigned char*)(0x35))
  #define TPM1C5VL  (*(volatile unsigned char*)(0x36))
  
  #define TPM2SC    (*(volatile unsigned char*)(0x60))
  #define TPM2CNTH  (*(volatile unsigned char*)(0x61))
  #define TPM2CNTL  (*(volatile unsigned char*)(0x62))
  #define TPM2MODH  (*(volatile unsigned char*)(0x63))
  #define TPM2MODL  (*(volatile unsigned char*)(0x64))
  #define TPM2C0SC  (*(volatile unsigned char*)(0x65))
  #define TPM2C0VH  (*(volatile unsigned char*)(0x66))
  #define TPM2C0VL  (*(volatile unsigned char*)(0x67))
  #define TPM2C1SC  (*(volatile unsigned char*)(0x68))
  #define TPM2C1VH  (*(volatile unsigned char*)(0x69))
  #define TPM2C1VL  (*(volatile unsigned char*)(0x6a))

// Clock generator

  #define ICGC1     (*(volatile unsigned char*)(0x48))
  #define ICGC2     (*(volatile unsigned char*)(0x49))
  #define ICGS1     (*(volatile unsigned char*)(0x4a))
  #define ICGS2     (*(volatile unsigned char*)(0x4b))
  #define ICGFLTU   (*(volatile unsigned char*)(0x4c))
  #define ICGFLTL   (*(volatile unsigned char*)(0x4d))
  #define ICGTRM    (*(volatile unsigned char*)(0x4e))
  
// SCI

  #define SCI1BDH   (*(volatile unsigned char*)(0x38))
  #define SCI1BDL   (*(volatile unsigned char*)(0x39))
  #define SCI1C1    (*(volatile unsigned char*)(0x3a))
  #define SCI1C2   (*(volatile unsigned char*)(0x3b))
  #define SCI1C3   (*(volatile unsigned char*)(0x3e))
  #define SCI1S1   (*(volatile unsigned char*)(0x3c))
  #define SCI1S2   (*(volatile unsigned char*)(0x3d))
  #define SCI1D    (*(volatile unsigned char*)(0x3f))
  #define SCI2BDH   (*(volatile unsigned char*)(0x40))
  #define SCI2BDL   (*(volatile unsigned char*)(0x41))
  #define SCI2C1    (*(volatile unsigned char*)(0x42))
  #define SCI2C2   (*(volatile unsigned char*)(0x43))
  #define SCI2C3   (*(volatile unsigned char*)(0x46))
  #define SCI2S1   (*(volatile unsigned char*)(0x44))
  #define SCI2S2   (*(volatile unsigned char*)(0x45))
  #define SCI2D    (*(volatile unsigned char*)(0x47))

// SPI

  #define SPI1C1    (*(volatile unsigned char*)(0x50))
  #define SPI1C2    (*(volatile unsigned char*)(0x51))
  #define SPI1BR    (*(volatile unsigned char*)(0x52))
  #define SPI1S     (*(volatile unsigned char*)(0x53))
  #define SPI1D     (*(volatile unsigned char*)(0x55))

  #define IRQSC     (*(volatile unsigned char*)(0x1c))

  #define SOPT      (*(volatile unsigned char*)(0x1802))

  #define SPMSC1    (*(volatile unsigned char*)(0x1809))
