// Tracker2 events

#define EVENT_ANY 0xffff
#define EVENT_GARMIN 4
#define EVENT_POSITION 0x4000
#define EVENT_SECOND 0x8000

#define RaiseEvent(x) (events |= (x))
#define ClearEvent(x) (events &= ~(x))
#define CheckEvent(x) (events & (x))

extern unsigned near volatile int events;