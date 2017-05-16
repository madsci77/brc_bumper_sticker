#define STD_TABLE 0x00
#define ALT_TABLE 0x80

// Enumerate custom waypoint symbols
enum
{
	sym_digi = 7680,
	sym_motorcycle = 7681,
	sym_helicopter = 7682,
	sym_sailboat = 7683,
	sym_jogger = 7684,
	sym_ambulance = 7685,
	sym_jeep = 7686,
	sym_truck = 7687,
	sym_eoc = 7688,
	sym_van = 7689,
	sym_crash = 7690,
	sym_weather = 7691,
	sym_x = 7692,
	sym_semi = 7693,
	sym_balloon = 7694,
	sym_eyeball = 7695,
	sym_tcpip = 7696,
	sym_xastir = 7697
};

typedef struct
{
	unsigned char aprs_symbol;
	unsigned char gps_symbol1;
	unsigned char gps_symbol2;
	unsigned char magellan_symbol;
	unsigned int garmin_symbol;
	unsigned int custom_symbol;
} symbol_mapping;

void send_waypoint(unsigned char *id, unsigned char *lat, unsigned char *lon, unsigned char ns, unsigned char ew, float altitude, const symbol_mapping *symbol, unsigned char *comment);

void garmin_send_waypoint(unsigned char *id, unsigned long lat, unsigned long lon, float altitude,
                          unsigned int symbol, unsigned char *comment);


extern const symbol_mapping symbols[];

extern const symbol_mapping default_symbol;
