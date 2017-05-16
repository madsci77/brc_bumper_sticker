// Status flags
#define F_LED_BLINK 0x01
#define F_HEADERLN 0x02
#define F_HOLD 0x10
#define F_TIME_SYNCED 0x80
#define F_PROCESSING_NMEA 0x1000
extern volatile unsigned near int flags;

