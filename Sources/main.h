char ddtoc(char *val);
unsigned int tdtoi(char *val);
extern volatile unsigned near int second, gps_fix_age, lastdigi;
extern unsigned near char ctemp[];

#define GPS_EPE_FLAG			31
#define GPS_INVALID_FLAG	255
#define PACKET_DATA_SIZE	128

typedef struct
{
	unsigned UseFMI			: 1;
	unsigned CustomSymbols	: 1;
	unsigned OldNMEA			: 1;
	unsigned FMIPassthrough	: 1;
} SystemConfiguration;


typedef struct {
	char position[21];
	char aprs_msl_height[8];
	char aprs_msl_tenths;
	char aprs_yymm[4];
	char aprs_ddhhmmss[8];
	char aprs_dao[2]; 
	unsigned int course;
	float altitude;
	float speed;
	unsigned char hdop;
	unsigned gotfirstfix	: 1;
	unsigned fixtype		: 2;
	unsigned satsinview		: 5;
	unsigned velocity_valid : 1;
	unsigned altitude_valid : 1;
	unsigned position_valid	: 1;
	unsigned time_valid		: 1;
	unsigned date_valid		: 1;
	unsigned quality_valid	: 1;
	unsigned neg_altitude	: 1;
	unsigned fix_expired	: 1;
	unsigned int climb;				// Vertical speed, feet/minute
	unsigned neg_climb		: 1;	// Rate of climb is negative
	unsigned long maxaltitude;		// Max altitude attained	
} GPS_FIX;

char ParseGPS(char gpsdata);
long semicircles(char *degstr, char hemi);
extern near GPS_FIX fixdata;
void nmea_init(void);
extern const SystemConfiguration config;
extern unsigned char packet_data[];
extern unsigned char packet_data_len;