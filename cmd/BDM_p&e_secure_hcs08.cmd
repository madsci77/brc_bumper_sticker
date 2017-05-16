
// HCS08 unsecuring command file: 
// These commands program the security byte to 0x00 (secured state, no backdoor key mechanism).
// If you want to enable the backdoor key mechanism, please search below "backdoor".

Memory < attributes ADDRESS 0 // do not display addresses below

FLASH MEMUNMAP // do not interact with regular flash programming monitor

wb 0x1802 0x03  // disable COP clearing SIMOPT register
wb 0x1825 0x30  // clear FPVIOL and FACCERR in FSTAT register
wb 0x1824 0xff  // remove all flash protections clearing FPROT register
wb 0x1820 30    // set clock divider for 5.2 MHz bus speed with FECDIV register 
                // Please see below to find how to evaluate this constant value.
      
//reprogram Security byte to secure state
wb 0x1825 0x30 // clear FPVIOL and FACCERR in FSTAT register
wb 0xffbf 0x00 // write NVFEOPT register in flash array to SECURED state 
               // If you want to allow the backdoor key mechanism, 
               // replace "wb 0xffbf 0x00" by "wb 0xffbf 0x80".
wb 0x1826 0x20 // write BYTE PROGRAM command in FCMD register   
wb 0x1825 0x80 // set FCBEF in FSTAT register to execute the command  
wait 10

reset

FLASH MEMMAP // restore regular flash programming monitor

// Evaluate the clock divider to set in FECDIV register:

// An average programming clock of 175 kHz is chosen.

// If the bus frequency is less than 10 MHz, the value to store
// in FECDIV is equal to " bus frequency (kHz) / 175 ".

// If the bus frequency is higher than 10 MHz, the value to store
// in FECDIV is equal to " bus frequency (kHz) / 1400  + 0x40 (to set PRDIV8 flag)".

// Datasheet proposed values:
//
// bus frequency     FECDIV value (decimal)
// 
//  20 MHz            76
//  10 MHz            49
//   8 MHz            39
//   4 MHz            19
//   2 MHz             9
//   1 MHz             4
// 200 kHz             0
// 150 kHz             0   


