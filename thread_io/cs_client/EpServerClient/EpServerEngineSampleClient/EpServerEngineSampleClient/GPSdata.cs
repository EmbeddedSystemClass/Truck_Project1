using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EpServerEngineSampleClient
{
	class mfloat
	{
		public int value;
		public int scale;
	}
	class mdate
	{
		public int day;
		public int month;
		public int year;
	}
	class mtime
	{
		public int hours;
		public int minutes;
		public int seconds;
		public int microseconds;
	}
	class minmea_sentence_rmc
	{
		public mtime time;
		public bool valid;
		public mfloat latitude;
		public mfloat longitude;
		public mfloat speed;
		public mfloat course;
		public mdate date;
		public mfloat variation;
	}
	class minmea_sentence_gga
	{
		public mtime time;
		public mfloat latitude;
		public mfloat longitude;
		public int fix_quality;
		public int satellites_tracked;
		public mfloat hdop;
		public mfloat altitude;
		char altitude_units;
		public mfloat height;
		char height_units;
		public mfloat dgps_age;
	}
	class minmea_sentence_gll
	{
		public mfloat latitude;
		public mfloat longitude;
		public mtime time;
		public char status;
		public char mode;
	}
	class minmea_sentence_gst
	{
		public mtime time;
		public mfloat rms_deviation;
		public mfloat semi_major_deviation;
		public mfloat semi_minor_deviation;
		public mfloat semi_major_orientation;
		public mfloat latitude_error_deviation;
		public mfloat longitude_error_deviation;
		public mfloat altitude_error_deviation;
	}

	class minmea_sentence_gsa
	{
		public char mode;
		public int fix_type;
		//int sats[12];
		public mfloat pdop;
		public mfloat hdop;
		public mfloat vdop;
	}

	class minmea_sat_info
	{
		public int nr;
		public int elevation;
		public int azimuth;
		public int snr;
	}

	class minmea_sentence_gsv
	{
		public int total_msgs;
		public int msg_nr;
		public int total_sats;
//		public msat_info sats[4];
	}

	class minmea_sentence_vtg
	{
		public mfloat true_track_degrees;
		public mfloat magnetic_track_degrees;
		public mfloat speed_knots;
		public mfloat speed_kph;
		//enum minmea_faa_mode faa_mode;
	}

	class minmea_sentence_zda
	{
		public mtime time;
		public mdate date;
		public int hour_offset;
		public int minute_offset;
	};

	class GPSdata
	{
		public minmea_sentence_rmc rmc;
		public minmea_sentence_gga gga;
		public minmea_sentence_gll gll;
		public minmea_sentence_gst gst;
		public minmea_sentence_gsa gsa;
		public minmea_sentence_gsv gsv;
		public minmea_sentence_vtg vtg;
		public minmea_sentence_zda zda;
	}
}
