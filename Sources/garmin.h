unsigned char garmin_rx_byte(unsigned char s);
void garmin_handler(void);// L001 - Link protocol 1 PIDs
void garmin_init(void);
void garmin_timer(void);
void garmin_send_packet(unsigned char pid, unsigned char *payload, unsigned char length);

#define WPT_UNKNOWN 0
#define WPT_D103 103
#define WPT_D104 104
#define WPT_D107 107
#define WPT_D108 108
#define WPT_D109 109
#define WPT_D110 110
#define WPT_D607 207
#define WPT_UNSUPPORTED 255

#define GF_FMI_MSG	4
#define GF_FMI_ON	8
#define GF_EXPIRE_COHORT 16

extern unsigned char wpt_protocol;
extern volatile unsigned char gflags;

enum {
	Pid_Ack_Byte = 6,
	Pid_Nak_Byte = 21,
	Pid_Command_Data = 10,
	Pid_Xfer_Cmplt = 12,
	Pid_Date_Time_Data = 14,
	Pid_Position_Data = 17,
	Pid_Prx_Wpt_Data = 19,
	Pid_Records = 27,
	Pid_Rte_Hdr = 29,
	Pid_Rte_Wpt_Data = 30,
	Pid_Almanac_Data = 31,
	Pid_Trk_Data = 34,
	Pid_Wpt_Data = 35,
	Pid_Pvt_Data = 51,
	Pid_Rte_Link_Data = 98,
	Pid_Trk_Hdr = 99,
	Pid_FlightBook_Record = 134,	/* packet with FlightBook data */
	Pid_Lap = 149,					/* part of Forerunner data */
	Pid_Wpt_Cat = 152,
	Pid_FMI = 161,					/* Fleet management interface */
	Pid_Protocol_Array = 253,		/* may not be implemented in all devices */
	Pid_Product_Rqst = 254,
	Pid_Product_Data = 255,
	Pid_Ext_Product_Data = 248		/* may not be implemented in all devices */
};

// A0101 - Device command protocol 1 commands
enum {
	Cmnd_Abort_Transfer = 0,		/* abort current transfer */
	Cmnd_Transfer_Alm = 1,			/* transfer almanac */
	Cmnd_Transfer_Posn = 2,			/* transfer position */
	Cmnd_Transfer_Prx = 3,			/* transfer proximity waypoints */
	Cmnd_Transfer_Rte = 4,			/* transfer routes */
	Cmnd_Transfer_Time = 5,			/* transfer time */
	Cmnd_Transfer_Trk = 6,			/* transfer track log */
	Cmnd_Transfer_Wpt = 7,			/* transfer waypoints */
	Cmnd_Turn_Off_Pwr = 8,			/* turn off power */
	Cmnd_Start_Pvt_Data = 49,		/* start transmitting PVT data */
	Cmnd_Stop_Pvt_Data = 50,		/* stop transmitting PVT data */
	Cmnd_FlightBook_Transfer= 92,	/* transfer flight records */
	Cmnd_Transfer_Laps = 117,		/* transfer laps */
	Cmnd_Transfer_Wpt_Cats = 121	/* transfer waypoint categories */
};
