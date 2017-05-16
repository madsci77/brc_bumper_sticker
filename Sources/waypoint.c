/* APRS Waypoint Mapping

   The following table maps APRS symbol codes to Garmin waypoint symbols.
   The first entry is the APRS symbol character (with high bit set for
   alternate symbol table), and the second entry is the Garmin identifier.
   Note that not all symbols are used on all Garmin devices, and may be
   substituted with a generic dot symbol. The final entry in the table
   determines the default if no other mapping is specified. */

#include <string.h>
#include "main.h"
#include "waypoint.h"
#include "hardware.h"
#include "ctype.h"
#include "utility.h"
#include "garmin.h"
#include "status.h"
#include "nmea.h"

#define MAX_WPT_SIZE 117

const symbol_mapping symbols[] = {						// APRS, Garmin, and Magellan
	{STD_TABLE | '#', 'B','D', 2, 16391, sym_digi},		// Digi = Tall Tower / Box
	{ALT_TABLE | '#', 'B','D', 2, 16391, sym_digi},		// Numbered Digi = Tall Tower / Box
	{STD_TABLE | '!', 'B', 'B', 24, 8249, 0},			// Police = Badge / Dot
	{ALT_TABLE | '!', 'O', 'B', 33, 8353, 0},			// Emergency = Red Triangle / Bull's-Eye
	{STD_TABLE | '$', 'B', 'E', 24, 155, 0},			// Phone = Phone / Dot
	{ALT_TABLE | '$', 'O', 'E', 7, 6, 0},				// Bank/ATM = Dollar Sign / Dollar Sign
	{STD_TABLE |  39, 'B', 'H', 5, 16384, 0},			// Small Aircraft = Airport / Airport
	{ALT_TABLE |  39, 'O', 'H', 33, 179, sym_crash},	// Crash Site = Circle with X / Bull's-Eye
	{STD_TABLE | '+', 'B', 'L', 12, 156, 0},			// Red Cross = First Aid Symbol / First Aid
	{ALT_TABLE | '+', 'O', 'L', 25, 8236, 0},			// Church = Church / Museum
	{STD_TABLE | ',', 'B', 'M', 38, 175, 0},			// Boy Scouts = Trailhead / Tourist
	{ALT_TABLE | ',', 'O', 'M', 38, 175, 0},			// Girl Scouts = Trailhead / Tourist
	{STD_TABLE | '-', 'B', 'N', 3, 10, 0},				// House (VHF) = House / House
	{ALT_TABLE | '-', 'O', 'N', 3, 10, 0},				// House (HF) = House / House
	{STD_TABLE | '.', 'B', 'O', 1, 8289, sym_x},		// Red X = Red Pin / Crossed Box
	{STD_TABLE | '/', 'B', 'P', 24, 18, 0},				// Dot = Waypoint Dot / Dot
	{ALT_TABLE | '9', 'A', '9', 15, 8, 0},				// Gas Station = Fuel / Fuel
	{STD_TABLE | ';', 'M', 'S', 10, 151, 0},			// Campground = Campground / Camps
	{ALT_TABLE | ';', 'N', 'S', 29, 160, 0},			// Picnic Area = Picnic Area / Knife and Fork
	{STD_TABLE | '<', 'M', 'T', 24, 18, sym_motorcycle},// Motorcycle = Dot / Dot
	{STD_TABLE | '>', 'M', 'V', 6, 170, 0},				// Car = Car / Amusement Park
	{ALT_TABLE | '>', 'N', 'V', 6, 170, 0},				// Numbered Car = Car
	{ALT_TABLE | '?', 'N', 'W', 38, 157, 0},			// Info Kiosk = Information
	{STD_TABLE | 'A', 'P', 'A', 12, 156, 0},			// Aid Station = First Aid Symbol
	{ALT_TABLE | 'C', 'A', 'C', 13, 186, 0},			// Coast Guard = Coast Guard
	{STD_TABLE | 'E', 'P', 'E', 33, 8286, sym_eyeball},	// Eyeball = Red Flag, Target
	{STD_TABLE | 'H', 'P', 'H', 18, 173, 0},			// Hotel = Lodging
	{STD_TABLE | 'I', 'P', 'I', 1, 	8292, sym_tcpip},	// TCP/IP = Red Block, Crossed Box
	{STD_TABLE | 'K', 'P', 'K', 24, 8216, 0},			// School = School
	{ALT_TABLE | 'L', 'A', 'L', 21, 12, 0},				// Lighthouse = Light
	{ALT_TABLE | 'N', 'A', 'N', 14, 17, 0},				// Navigation Buoy = White Buoy
	{STD_TABLE | 'O', 'P', 'O', 4, 18, sym_balloon},	// Balloon = Dot / Balloon
	{STD_TABLE | 'P', 'P', 'P', 24, 8249, 0},			// Police = Badge
	{ALT_TABLE | 'P', 'A', 'P', 24, 158, 0},			// Parking = Parking
	{STD_TABLE | 'R', 'P', 'R', 32, 8215, 0},			// RV = RV Park
	{ALT_TABLE | 'R', 'A', 'R', 29, 11, 0},				// Restaurant = Knife and Fork
	{ALT_TABLE | 'V', 'A', 'V', 13, 16387, 0},			// VORTAC  = VOR
	{STD_TABLE | 'X', 'P', 'X', 24, 16388, sym_helicopter},	// HELO = Heliport
	{ALT_TABLE | 'X', 'A', 'X', 24, 8212, 0},			// Pharmacy = Pharmacy
	{STD_TABLE | 'Y', 'P', 'Y', 9, 0, sym_sailboat},	// Yacht = Anchor
	{STD_TABLE | '[', 'H', 'S', 20, 175, sym_jogger},	// Jogger = Trail Head
	{STD_TABLE | ']', 'H', 'U', 24, 8214, 0},			// Post Office = Post Office
	{STD_TABLE | '^', 'H', 'V', 5, 16384, 0},			// Large Aircraft = Airport
	{ALT_TABLE | '^', 'D', 'V', 5, 16384, 0},			// Numbered Aircraft = Airport
	{STD_TABLE | 'a', 'L', 'A', 12, 156, sym_ambulance},// Ambulance = First Aid Symbol
	{STD_TABLE | 'b', 'L', 'B', 24, 8293, 0},			// Bike = Bike Trail
	{STD_TABLE | 'g', 'L', 'G', 24, 16393, 0},			// Glider = Glider
	{STD_TABLE | 'h', 'L', 'H', 12, 156, 0},			// Hospital = First Aid Symbol
	{ALT_TABLE | 'h', 'S', 'H', 24, 172, 0},			// Ham Store = Shopping Cart
	{STD_TABLE | 'j', 'L', 'J', 39, 170, sym_jeep},		// Jeep = Car
	{STD_TABLE | 'k', 'L', 'K', 39, 176, sym_truck},	// Truck = Truck Stop
	{STD_TABLE | 'o', 'L', 'O', 22, 8299, sym_eoc},		// EOC = Blue Diamond
	{STD_TABLE | 'p', 'L', 'P', 35, 8275, 0},			// Rover = Dog (Rino units only)
	{STD_TABLE | 'r', 'L', 'R', 2, 16391, 0},			// Antenna = Tall Tower
	{ALT_TABLE | 'r', 'S', 'R', 28, 152, 0},			// Restrooms = Restrooms / 'R' (resort)
	{STD_TABLE | 't', 'L', 'T', 39, 176, 0},			// Truck Stop = Truck Stop
	{STD_TABLE | 'u', 'L', 'U', 39, 176, sym_semi},		// Truck = Truck Stop
	{ALT_TABLE | 'u', 'S', 'U', 39, 176, sym_semi},		// Numbered Truck = Truck Stop
	{STD_TABLE | 'v', 'L', 'V', 6, 170, sym_van},		// Van = Car
	{ALT_TABLE | 'v', 'S', 'V', 6, 170, sym_van},		// Numbered Van = Car
	{STD_TABLE | 'x', 'L', 'X', 1, 8238, sym_xastir},	// Xastir = Crossing, Box
	{STD_TABLE | 'y', 'L', 'Y', 3, 10, 0},				// House with Yagi = House
	{STD_TABLE | '_', 'H', 'W', 24, 8296, sym_weather},	// Weather Station = Blue Circle
	{ALT_TABLE | '_', 'D', 'W', 24, 8296, sym_weather},	// Numbered Weather Station = Blue Circle
	{0, 0, 0, 24, 18, 0}									// End of table - Default = Waypoint Dot
};

const symbol_mapping default_symbol = {0, 0, 0, 24, 18, 0};

unsigned char garmin_checksum;
extern unsigned char scratch[];

unsigned int crc_calc(unsigned char c, unsigned int f)
{
	unsigned char bit;
	for (bit=8; bit; bit--)
	{
		if ((f ^ c) & 1) f = (f >> 1) ^ 0x8408;
		else f = f >> 1;
		c = c >> 1;
	}
	return f;
}

void send_waypoint(unsigned char *id, unsigned char *lat, unsigned char *lon, unsigned char ns, unsigned char ew, float altitude, const symbol_mapping *symbol, unsigned char *comment)
{
	unsigned char decoded_alt[7], wpt_name[10];
	unsigned char *p, sym;

	strcpy(wpt_name, id);
	/*
	// NMEA $GPWPL
	nmea_checksum = 0;
	nmea_outs("$GPWPL,");
	nmea_checksum ^= '$';
	nmea_outn(lat, 7);
	nmea_outc(',');
	nmea_outc(ns);
	nmea_outc(',');
	nmea_outn(lon, 8);
	nmea_outc(',');
	nmea_outc(ew);
	nmea_outc(',');
	nmea_outs(wpt_name);
	nmea_send_checksum();
	*/
	// Kenwood $PKWDWPL format: $PKWDWPL,hhmmss,V,lat,N,lon,W,speed,course,DDMMYY,altitude,name,table/symbol*hh
	nmea_checksum = 0;
	nmea_outs("$PKWDWPL,");
	nmea_checksum ^= '$';
	nmea_outn(&fixdata.aprs_ddhhmmss[2], 6);
	nmea_outs(",V,");
	nmea_outn(lat, 7);
	nmea_outc(',');
	nmea_outc(ns);
	nmea_outc(',');
	nmea_outn(lon, 8);
	nmea_outc(',');
	nmea_outc(ew);
	nmea_outs(",0,0,000000,"); // Speed, course, date
	alt_feet((long)(altitude * 100), decoded_alt);
	decoded_alt[6] = 0;
	for (p = decoded_alt; *p == '0'; p++);
	nmea_outs(p);
	nmea_outc(',');
	nmea_outs(wpt_name);
	nmea_outc(',');
	// symbol
	if (symbol->aprs_symbol & ALT_TABLE) nmea_outc('\\');
	else nmea_outc('/');
	nmea_outc(symbol->aprs_symbol & ~ALT_TABLE);	
	nmea_send_checksum();

	// Magellan $PMGNWPL format: $PMGNWPL,lat,N,lon,W,altitude,[F/M],name,comment,symbol,wpt_type*hh
	nmea_checksum = 0;
	nmea_outs("$PMGNWPL,");
	nmea_checksum ^= '$';
	nmea_outn(lat, 7);
	nmea_outc(',');
	nmea_outc(ns);
	nmea_outc(',');
	nmea_outn(lon, 8);
	nmea_outc(',');
	nmea_outc(ew);
	nmea_outc(',');
	alt_feet((long)(altitude * 100), decoded_alt);
	decoded_alt[6] = 0;
	for (p = decoded_alt; *p == '0'; p++);
	nmea_outs(p);
	nmea_outs(",F,");
	nmea_outs(wpt_name);
	nmea_outc(',');
	for (p = comment; *p; p++) if (!isprint(*p) || *p == ',') *p = ' ';
	nmea_outs(comment);
	nmea_outc(',');
	sym = symbol->magellan_symbol;
	if (sym > 26)
	{
		sym -= 26;
		nmea_outc('a');
	}
	nmea_outc(96 + sym);
	nmea_send_checksum();
}


const char wpt_static[] = {
	0x01, 0x00, 0x0f, 0x70, // dtyp, class, color, attr
	0x15, 0x20, // symbol
	0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // sublass
	0x38, 0x8e, 0xe3, 0x18, 0xab, 0xaa, 0xaa, 0xaa}; // position


// Send a waypoint to the GPS receiver in Garmin binary format
void garmin_send_waypoint(char *id, unsigned long lat, unsigned long lon, float altitude,
                          unsigned int symbol, char *comment)
{

	char *ext = &scratch[52], *pos = &scratch[24], *p, *q;
	unsigned char len;
	union {
		unsigned char c[4];
		float f;
	} fc;

	// Zero out and copy down waypoint data
	memset(scratch, 0, MAX_WPT_SIZE);
	memcpy(scratch, wpt_static, sizeof(wpt_static));
	
	// We can only send waypoints if we've identified the format in use
	if (wpt_protocol == WPT_UNKNOWN || wpt_protocol == WPT_UNSUPPORTED) return;
	
	// Replace non-printable characters in comment with spaces
	for (p = comment; *p; p++)
	{
		if (!isprint(*p)) *p = ' ';
	}		

	if (wpt_protocol == WPT_D104 || wpt_protocol == WPT_D103)
	{
		// Handle character restrictions for comment
		for (p = comment; *p; p++)
		{
			if (islower(*p)) *p = toupper(*p);
		}
		// Only first 6 bytes of ID used
		len = strlen(id);
		if (len > 6)
		{
			p = id + len - 1;
			q = &scratch[5];
			while (q >= scratch)
			{
				if (isalnum(*p))
				{
					*q = *p;
					q--;
				}
				p--;
			}
		}
		else
		{				
			scratch[0] = scratch[1] = scratch[2] = scratch[3] = scratch[4] = scratch[5] = ' ';
			scratch[6] = 0;
			strcpy(scratch, id);
		}
		// Fix characters in waypoint name
		// TODO: See if this breaks D104
		for (p = scratch; *p; p++)
		{
			if (islower(*p)) *p = toupper(*p);
			if (!isalnum(*p)) *p = '-';
		}
		*p = ' ';
		pos = &scratch[6];
		scratch[14] = scratch[15] = scratch[16] = scratch[17] = 0;
		if (strlen(comment) > 40) comment[40] = 0;
		strcpy(&scratch[18], comment);
		if (wpt_protocol == WPT_D103)
		{
			// D103-specific, including symbol substitutions
			switch (symbol)
			{
			case 10:
				symbol = 1;	break;
			case 8:
				symbol = 2;	break;
			case 170:
			case 176:
				symbol = 3; break;
			case 0:
				symbol = 5;	break;
			case 14:
				symbol = 9; break;
			case 16391:
				symbol = 10;break;
			case 151:
			case 160:
				symbol = 11;break;
			case 8353:
			case 179:
			case 8289:
				symbol = 12;break;
			case 8275:
				symbol = 13;break;
			case 156:
				symbol = 14;break;
			case 175:
				symbol = 15;break;
			default:
				symbol = 0;
			}
			scratch[58] = symbol & 0xff;
			scratch[59] = 0; // symbol + name
			len = 60;		
		}
		else
		{
			// D104-specific
			scratch[58] = scratch[59] = scratch[60] = scratch[61] = 0;		
			scratch[62] = symbol & 0xff;
			scratch[63] = symbol >> 8;
			// dspl: 0,1 = symbol, 3 = symbol + name, 5 = symbol + comment
			scratch[64] = 3;		
			len = 65;
		}
	}
	else
	{
		
		// Default is D109 waypoint

//		if (gflags & GF_FMI_ON)
		{
			// Nuvi has some symbol changes
			switch (symbol)
			{
			case 170:
				symbol = 8207; break;	// Car repair -> car
			case 8353:
				symbol = 8286; break;	// Red triangle -> red flag
			case 8296:
				symbol = 8290; break;	// Blue circle -> blue box
			}
		}
		
		// Substitute unprintable characters
		for (p = comment; *p; p++)
		{
			switch (*p)
			{
				case '@': *p = '-';	break;
			}
		}		
		
		
		if (wpt_protocol == WPT_D607)
		{
			// D607 is specific to FMI and has new structure
			unsigned int hash;
			unsigned char c;
			len = strlen(id);
			hash = 0xffff;
			scratch[0] = 0x30;													// 0x0130 = Create Waypoint Packet ID
			scratch[1] = 0x01;
			// Form 16-bit hash for (hopefully) unique ID, byte order doesn't matter
			*(unsigned int *)&scratch[2] = 0xffff;
			for (c = 0; c < len; c++) *(unsigned int *)&scratch[2] = crc_calc(id[c], *(unsigned int *)&scratch[2]);
			scratch[4] = symbol & 0xff;
			scratch[5] = symbol >> 8;
			scratch[14] = (1 << (gflags & 0x03)) | 16;
			scratch[15] = 0;
			strcpy(&scratch[16], id);
			strncpy(&scratch[45], comment, 50);
			scratch[95] = 0;
			pos = &scratch[6];
			len = 98;
		}
		else
		{	
			// D108 overrides
			if (wpt_protocol == WPT_D108)
			{
				ext = &scratch[48]; // No ETE
				scratch[0] = 0;
				scratch[1] = 255;
				scratch[2] = 0;
			}
			
			// D110 overrides
			if (wpt_protocol == WPT_D110)
			{
				scratch[3] = 0x80;	// Attr: Mark as D110
				ext = &scratch[62];	// Adds temp(4), time(4), wpt_cat(2)
				scratch[52] = scratch[53] = scratch[54] = scratch[55] = 0; // Temp
				scratch[56] = scratch[57] = scratch[58] = scratch[59] = 0xff; // Time
				scratch[60] = scratch[61] = 0x00; // Wpt_cat (user)
			}
			
			// Store altitude
			fc.f = altitude;
			scratch[32] = fc.c[3]; scratch[33] = fc.c[2];
			scratch[34] = fc.c[1]; scratch[35] = fc.c[0];
				
			scratch[4] = symbol & 0xff;
			scratch[5] = symbol >> 8;
			strcpy(ext, id);
			ext += strlen(id) + 1;
			// Calculate space remaining
			len = (scratch + MAX_WPT_SIZE) - ext - 5;
	#ifdef DEBUG
/*
			if (fixdata.time_valid)
			{
				len -= 7;
				strncpy(ext, fixdata.aprs_ddhhmmss, 6);
				ext[6] = ' ';
				ext += 7;
			}
			*/
	#endif
			if (strlen(comment) >= len) comment[len] = 0;
			strcpy(ext, comment);
			ext += strlen(comment) + 1;
			*ext++ = 0; // facility
			*ext++ = 0; // city
			*ext++ = 0; // addr
			*ext++ = 0; // crossroad
			len = ext-scratch;
		}
	}
	
	// Position
	*pos = lat & 0xff;
	*(pos+1) = lat >> 8;
	*(pos+2) = lat >> 16;
	*(pos+3) = lat >> 24;
	*(pos+4) = lon & 0xff;	
	*(pos+5) = lon >> 8;
	*(pos+6) = lon >> 16;
	*(pos+7) = lon >> 24;
		
	garmin_send_packet((wpt_protocol == WPT_D607 ? Pid_FMI : Pid_Wpt_Data), scratch, len);
	
}
