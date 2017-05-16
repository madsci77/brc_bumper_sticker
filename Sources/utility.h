// Utility routine header file

#define NOP5 {__asm nop; __asm nop; __asm nop; __asm nop; __asm nop;}
#define NOP1 {__asm nop;}
#define NOP2 {__asm nop; __asm nop;}

// bit 7 = led on/off, bit 6 = continuous
// set values
#define BLINK_NONE 0
#define BLINK_ON 0xc0
#define BLINK_N 0x80
#define BLINK_ONCE 0x81
#define	COLOR_RED 0x20
#define COLOR_GREEN 0x10
// masks
#define BLINK_ENABLED 0x4f
#define BLINK_STAT 0x80
#define BLINK_COUNT 0x0f

#define Wait {__asm WAIT;}
#define Stop {__asm STOP;}


void semicircledecode(signed long sslat, signed long sslon, unsigned char *lat, unsigned char *lon,
                  unsigned char *ns, unsigned char *ew);

long semicircles(char *degstr, char hemi);

// Converts semicircles to Base 91 units (must be properly biased first)
// Non-reentrant use of ltemp, but we only call this function in one place
void base91encode(char *s);
void base91decode(char *s, signed long *l);
void alt_feet(long alt, char *s);
const unsigned far int base91speed[];

// Send a string to the serial port
void sci_print(const char *c);
void sci_printn(const char *c, unsigned char n);
unsigned int hexint(char *val);

void sci_writehex(unsigned char c);
void sci_writehex_b(unsigned char c);

// Convert two ascii digits to char
char ddtoc(char *val);

// Convert three ascii digits to int
unsigned int tdtoi(char *val);
unsigned char hexbyte(char *val);

// Decimal conversion
void calc_digit(char *d, unsigned int *dv, unsigned int ds);

// Convert integer to string
char *itoa (int v, char *s);
void itodec(unsigned int i, char *s);
void itodec5(unsigned int i, char *s);

unsigned char strcasecmp(const char *s1, const char *s2);
unsigned char strncasecmp(const char *s1, const char *s2, unsigned char n);

extern const long valtable[];

extern unsigned near char led_on_period;	// How many ticks the LED stays on
extern unsigned near char led_off_period;	// How many ticks the LED stays off
extern unsigned near char led_counter;		// Counter for LED, updated every tick
extern unsigned near char led_mode; 		// LED mode and count - bit 7 = current state, bit 6 = continuous

extern volatile unsigned near int wait_ticks;

void wait(unsigned int ticks);
void wait25(void);
void wait10(void);

extern near long ltemp;

// Blink LED.  On, off, blinking, or n blinks.  Takes 'on' and 'off' times in 4-ticks.

#define led_blink(lb_mode, lb_on, lb_off) { led_mode = lb_mode; led_on_period = lb_on; led_off_period = lb_off; }

