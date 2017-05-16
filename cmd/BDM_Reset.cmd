// After reset the commands written below will be executed
// Disable watchdog
WB 0x1802 0x03
// Set up oscillator
WB 0x0048 0x38
WB 0x0049 0x70
//Findproc main
