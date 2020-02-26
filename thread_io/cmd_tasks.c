 #if 1
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <pthread.h>
#define closesocket close
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include "../mytypes.h"
#include "ioports.h"
#include "serial_io.h"
//#include "queue/illist_threads_rw.h"
#include "queue/ollist_threads_rw.h"
//#include "queue/rt_llist_threads_rw.h"
#include "tasks.h"
#include "cs_client/config_file.h"
#include "lcd_func.h"

extern int engine_running;
int enable_gps_send_data;
int enable_adc_send_data;
extern pthread_mutex_t     tcp_read_lock;
extern pthread_mutex_t     tcp_write_lock;
int config_file_ok = -1;
int myprintf1(char *str)
{
	return 0;
}
int myprintf2(char *str, int x)
{
	return 0;
}

/**********************************************************************************************************/
int myprintf3(char *str, int x, int y)
{
	return 0;
}

#define TOGGLE_OTP otp->onoff = (otp->onoff == 1?0:1)

CMD_STRUCT cmd_array[110] =
{
	{	NON_CMD,"NON_CMD\0" },
	{	ENABLE_START,"ENABLE_START\0" },
	{	STARTER_OFF,"STARTER_OFF\0" },
	{	ON_ACC,"ON_ACC\0" },
	{	OFF_ACC,"OFF_ACC\0" },
	{	ON_FUEL_PUMP,"ON_FUEL_PUMP\0" },
	{	OFF_FUEL_PUMP,"OFF_FUEL_PUMP\0" },
	{	ON_FAN,"ON_FAN\0" },
	{	OFF_FAN,"OFF_FAN\0" },
	{	ON_LIGHTS,"ON_LIGHTS\0" },
	{	OFF_LIGHTS,"OFF_LIGHTS\0" },
	{	ON_BRIGHTS,"ON_BRIGHTS\0" },
	{	OFF_BRIGHTS,"OFF_BRIGHTS\0" },
	{	BLANK,"BLANK\0" },
	{	ESTOP_SIGNAL,"ESTOP_SIGNAL\0" },
	{	ON_BRAKES,"ON_BRAKES\0" },
	{	OFF_BRAKES,"OFF_BRAKES\0" },
	{	ON_RUNNING_LIGHTS,"ON_RUNNING_LIGHTS\0" },
	{	OFF_RUNNING_LIGHTS,"OFF_RUNNING_LIGHTS\0" },
	{	SPECIAL_CMD,"SPECIAL_CMD\0" },
	{	START_SEQ,"START_SEQ\0" },
	{	SHUTDOWN,"SHUTDOWN\0" },
	{	ON_LLIGHTS,"ON_LLIGHTS\0" },
	{	OFF_LLIGHTS,"OFF_LLIGHTS\0" },
	{	ON_LBRIGHTS,"ON_LBRIGHTS\0" },
	{	OFF_LBRIGHTS,"OFF_LBRIGHTS\0" },
	{	ON_RLIGHTS,"ON_RLIGHTS\0" },
	{	OFF_RLIGHTS,"OFF_RLIGHTS\0" },
	{	ON_RBRIGHTS,"ON_RBRIGHTS\0" },
	{	OFF_RBRIGHTS,"OFF_RBRIGHTS\0" },
	{	WIPER1,"WIPER1\0" },
	{	WIPER2,"WIPER2\0" },
	{	WIPER_OFF,"WIPER_OFF\0" },
	{	SHUTDOWN_IOBOX,"SHUTDOWN_IOBOX\0" },
	{	REBOOT_IOBOX,"REBOOT_IOBOX\0" },
	{	LCD_SHIFT_RIGHT,"LCD_SHIFT_RIGHT\0" },
	{	LCD_SHIFT_LEFT,"LCD_SHIFT_LEFT\0" },
	{	SCROLL_UP,"SCROLL_UP\0" },
	{	SCROLL_DOWN,"SCROLL_DOWN\0" },
	{	ENABLE_LCD,"ENABLE_LCD\0" },
	{	SET_TIME,"SET_TIME\0" },
	{	GET_TIME,"GET_TIME\0" },
	{	TEST_LEFT_BLINKER,"TEST_LEFT_BLINKER\0" },
	{	TEST_RIGHT_BLINKER,"TEST_RIGHT_BLINKER\0" },
	{	DISCONNECT,"DISCONNECT\0" },
	{	BAD_MSG,"BAD_MSG\0" },
	{	CURRENT_TIME,"CURRENT_TIME\0" },
	{	SET_PARAMS,"SET_PARAMS\0" },
	{	EXIT_PROGRAM,"EXIT_PROGRAM\0" },
	{	ENGINE_TEMP,"ENGINE_TEMP\0" },
	{	INDOOR_TEMP,"INDOOR_TEMP\0" },
	{	SEND_RT_VALUES1,"SEND_RT_VALUES1\0" },
	{	SEND_RT_VALUES2,"SEND_RT_VALUES2\0" },
	{	SEND_RT_VALUES3,"SEND_RT_VALUES3\0" },
	{	ENGINE_RUNTIME,"ENGINE_RUNTIME\0" },
	{	SERVER_UPTIME,"SERVER_UPTIME\0" },
	{	SEND_CONFIG,"SEND_CONFIG\0" },
	{	SEND_MSG,"SEND_MSG\0" },
	{	SEND_RPM,"SEND_RPM\0" },
	{	SEND_MPH,"SEND_MPH\0" },
	{	SEND_ADCS1,"SEND_ADCS1\0" },
	{	SEND_ADCS2,"SEND_ADCS2\0" },
	{	NAV_UP,"NAV_UP\0" },
	{	NAV_DOWN,"NAV_DOWN\0" },
	{	NAV_SIDE,"NAV_SIDE\0" },
	{	NAV_CLICK,"NAV_CLICK\0" },
	{	NAV_CLOSE,"NAV_CLOSE\0" },
	{	NAV_NUM,"NAV_NUM\0" },
	{	SEND_STATUS,"SEND_STATUS\0" },
	{	SERVER_UP,"SERVER_UP\0" },
	{	SERVER_DOWN,"SERVER_DOWN\0" },
	{	UPLOAD_NEW,"UPLOAD_NEW\0" },
	{	UPLOAD_OTHER,"UPLOAD_OTHER\0" },
	{	UPLOAD_NEW_PARAM,"UPLOAD_NEW_PARAM\0" },
	{	SET_TEMP_LIMIT,"SET_TEMP_LIMIT\0" },
	{	SET_FAN_ON,"SET_FAN_ON\0" },
	{	SET_FAN_OFF,"SET_FAN_OFF\0" },
	{	HIGH_REV_LIMIT,"HIGH_REV_LIMIT\0" },
	{	LOW_REV_LIMIT,"LOW_REV_LIMIT\0" },
	{	LIGHTS_ON_DELAY,"LIGHTS_ON_DELAY\0" },
	{	SET_BLOWER_EN_TEMP,"SET_BLOWER_EN_TEMP\0" },
	{	SET_BLOWER1_TEMP,"SET_BLOWER1_TEMP\0" },
	{	SET_BLOWER2_TEMP,"SET_BLOWER2_TEMP\0" },
	{	SET_BLOWER3_TEMP,"SET_BLOWER3_TEMP\0" },
	{	SET_BATT_BOX_TEMP,"SET_BATT_BOX_TEMP\0" },
	{	TEMP_TOO_HIGH,"TEMP_TOO_HIGH\0" },
	{	GET_VERSION,"GET_VERSION\0" },
	{	UPDATE_CONFIG,"UPDATE_CONFIG\0" },
	{	SEND_CONFIG2,"SEND_CONFIG2\0" },
	{	GET_CONFIG2,"GET_CONFIG2\0" },
	{	CLIENT_CONNECTED,"CLIENT_CONNECTED\0" },
	{	SERVER_CONNECTED,"SERVER_CONNECTED\0" },
	{	SET_KEYMODE,"SET_KEYMODE\0" },
	{	SHELL_AND_RENAME,"SHELL_AND_RENAME\0" },
	{	REFRESH_LCD,"REFRESH_LCD\0" },
	{	SEND_GPS_GLL_DATA,"SEND_GPS_GLL_DATA\0" },
	{	SEND_GPS_GGA_DATA,"SEND_GPS_GGA_DATA\0" },
	{	SEND_GPS_GSA_DATA,"SEND_GPS_GSA_DATA\0" },
	{	SEND_GPS_GSV_DATA,"SEND_GPS_GSV_DATA\0" },
	{	SEND_GPS_RMC_DATA,"SEND_GPS_RMC_DATA\0" },
	{	SEND_GPS_VTG_DATA,"SEND_GPS_VTG_DATA\0" },
	{	SEND_GPS_ZDA_DATA,"SEND_GPS_ZDA_DATA\0" },
	{	SET_GPS_DATA,"SET_GPS_DATA\0" },
	{	SET_GPS_BAUDRATE,"SET_GPS_BAUDRATE\0" },
	{	ENABLE_GPS_SEND_DATA,"ENABLE_GPS_SEND_DATA\0" },
	{	ENABLE_ADC_SEND_DATA,"ENABLE_ADC_SEND_DATA\0" },
	{	ADC_GATE,"ADC_GATE\0" },
	{	SET_ADC_RATE,"SET_ADC_RATE\0" },
	{	SET_RPM_MPH_RATE,"SET_RPM_MPH_RATE\0" },
	{	SET_FPGA_RATE,"SET_FPGA_RATE\0" }
};

//extern illist_t ill;
extern ollist_t oll;

UCHAR msg_buf[UPLOAD_BUFF_SIZE];
UCHAR msg_buf2[UPLOAD_BUFF_SIZE/2];
extern PARAM_STRUCT ps;
extern char password[PASSWORD_SIZE];

int shutdown_all;
static UCHAR pre_preamble[] = {0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00};
static void format_param_msg(void);

// these are the actual values that are shown in the client's dialog's comboboxes
// as choices for rev limit's and lights_on_delays
// they are stored on disk as the selected indexes in the comboboxes
// so the have to be converted to the actual values
char hi_rev[10][10] =  {"6000",
						"5800",
						"5600",
						"5400",
						"5200",
						"5000",
						"4800",
						"4600",
						"4200",
						"4000"};

char lo_rev[9][10] =   {"4500",
						"4400",
						"4300",
						"4200",
						"4100",
						"4000",
						"3900",
						"3800",
						"3700"};

/*
char lights[13][15] =  {"1 second",
						"2 seconds",
						"3 seconds",
						"5 seconds",
						"10 seconds",
						"15 seconds",
						"30 seconds",
						"1 minute",
						"2 minutes",
						"5 minutes",
						"10 minutes",
						"30 minutes",
						"1 hour"};
*/
int lights_on_delay[13] = {1,2,3,5,10,15,30,60,120,300,600,1800,3600};
static int actual_high_rev_limit;
static int actual_low_rev_limit;
int actual_lights_on_delay;
int tcp_window_on;
//int serial_recv_on;
//int time_set;

// array of structs to list all the inputs that have outputs assigned
// to them 
#endif
/*********************************************************************/
// task to get commands from the host
UCHAR get_host_cmd_task(int test)
{
//	I_DATA tempi1;
	O_DATA tempo1;
//	RI_DATA tempr1;
//	I_DATA *itp;
	O_DATA *otp;
	O_DATA **otpp = &otp;
	int rc = 0; 
	int rc1 = 0;
	UCHAR cmd = 0x21;
	UCHAR onoff;
	char errmsg[50];
	char filename[15];
	char *fptr;
	size_t size;
	int i;
	int j;
	int k;
	size_t isize;
	size_t osize;
	UCHAR bank;
	int test1, test2;
	UCHAR test2a, checksum;
	UCHAR _port,_onoff,_type,_time_delay;
	int _input;
	int testx;
	UCHAR rec_no;
	struct dirent **namelist;
	DIR *d;
	struct dirent *dir;
	int num;
	UCHAR test_io_num = 0;
	char tempx[SERIAL_BUFF_SIZE];
	UCHAR tempy[30];
	char temp_time[5];
	char *pch;
	int fname_index;
	UCHAR uch_fname_index;
	UCHAR mask;
	time_t curtime2;
	time_t *pcurtime2 = &curtime2;
	tcp_window_on = 0;	
	int fp;
	off_t fsize;
	long cur_fsize;
	struct timeval mtv;
	struct tm t;
	struct tm *pt = &t;
	int msg_len;
	serial_recv_on = 1;
//	time_set = 0;
	shutdown_all = 0;
	char version[15] = "sched v1.31\0";
	UINT utemp;
//	UCHAR time_buffer[20];
	UCHAR write_serial_buffer[SERIAL_BUFF_SIZE];

	memset(write_serial_buffer, 0, SERIAL_BUFF_SIZE);
	// since each card only has 20 ports then the 1st 2 port access bytes
	// are 8-bit and the 3rd is only 4-bits, so we have to translate the
	// inportstatus array, representing 3 byts of each 2 (3x8x2 = 48) to
	// one of the 40 actual bits as index

	// the check_inputs & change_outputs functions
	// use the array to adjust from index to bank
	// since there are only 4 bits in banks 3 & 5

	for(i = 0;i < 20;i++)
	{
		real_banks[i].i = i;
		real_banks[i].bank = i/8;
		real_banks[i].index = i - real_banks[i].bank*8;
	}

	for(i = 20;i < 40;i++)
	{
		real_banks[i].i = i;
		real_banks[i].bank = (i+4)/8;
		real_banks[i].index = i - (real_banks[i].bank*8)+4;
	}
	memset(dat_names,0,sizeof(dat_names));

/*
	i = NUM_PORT_BITS;
	isize = sizeof(I_DATA);
	isize *= i;
*/
	i = NUM_PORT_BITS;
	osize = sizeof(O_DATA);
	osize *= i;

	running_hours = running_minutes = running_seconds = 0;
	trunning_hours = trunning_minutes = trunning_seconds = 0;

//	program_start_time = curtime();

	ollist_init(&oll);
	if(access(oFileName,F_OK) != -1)
	{
		config_file_ok = olLoadConfig(oFileName,&oll,osize,errmsg);
		//sprintf(tempx, "%d loadconfig",rc);
		//printString3(tempx);
		if(rc > 0)
		{
			myprintf1(errmsg);
		}
	}
	init_ips();

	same_msg = 0;
//	lcd_init();

// flash green and red led's to signal we are up (if LCD screen not attached)
#if 0
	for(i = 0;i < 5;i++)
	{
		red_led(1);
		usleep(10000);
		red_led(0);
		green_led(1);
		usleep(10000);
		green_led(0);
		red_led(1);
		usleep(10000);
		red_led(0);
		green_led(1);
		usleep(10000);
		green_led(0);
	}
#endif
//	myprintf1("start....\0");

	myprintf1(version);
	memset(rt_file_data,0,sizeof(rt_file_data));
	odometer = 0;
	trip = 0;
	mask = 1;

	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
//		if(1)
		{
			memset(msg_buf,0,sizeof(msg_buf));
			msg_len = get_msg();
//			printHexByte(msg_len);
			if(msg_len < 0)
			{
//				printf("bad msg\r\n");
				cmd = BAD_MSG;
				usleep(10000);
			}else
			{
				rc = recv_tcp(&msg_buf[0],msg_len,1);
				cmd = msg_buf[0];
//				printHexByte(cmd);
//				printHexByte(rc);
//				printString2("\r\n");	
				j = 0;

//				for(i = 0;i < msg_len;i++)
//					printHexByte(msg_buf[i]);

//				printString2("\r\n");	

				for(i = 2;i < msg_len+2;i+=2)
					msg_buf2[j++] = msg_buf[i];


//				for(i = 0;i < j;i++)
//				{
//					printHexByte(msg_buf2[i]);
//					write_serial2(msg_buf2[i]);
//					printHexByte(msg_buf2[i]);
//				}

//				printString2("\r\n");	
			}
			if(cmd != LCD_SHIFT_RIGHT && cmd != LCD_SHIFT_LEFT && cmd != SCROLL_DOWN && 
				cmd != SCROLL_UP && cmd > 0)
//					&& cmd != GET_TIME && cmd != SET_TIME && cmd > 0)
				myprintf2(cmd_array[cmd].cmd_str,cmd);
//#if 0
			if(cmd > 0)
			{
				sprintf(tempx, "cmd: %d %s\0",cmd,cmd_array[cmd].cmd_str);
				//printString2(tempx);
				printString3(tempx);
				//printf("%s\r\n",tempx);
//				printf("cmd: %d %s\r\n",cmd,cmd_array[cmd].cmd_str);
			}
//#endif
			if(cmd > 0)
			{
				rc = 0;
 				switch(cmd)
				{
					// ENABLE_START closes a relay for so many seconds so a button
					// on the dash can close the ckt to the starter solinoid
					case ENABLE_START:
					case STARTER_OFF:
					// ingition - power to the coil
					case ON_ACC:
					case OFF_ACC:
					case ON_FUEL_PUMP:
					case OFF_FUEL_PUMP:
					case ON_FAN:
					case OFF_FAN:
					case ON_LIGHTS:
					case OFF_LIGHTS:
					case START_SEQ:
					case SHUTDOWN:
					case SHUTDOWN_IOBOX:
					case REBOOT_IOBOX:
					case UPLOAD_NEW:
					case UPLOAD_NEW_PARAM:
					case UPLOAD_OTHER:
					case SHELL_AND_RENAME:
					case TEST_LEFT_BLINKER:
					case TEST_RIGHT_BLINKER:
					case ON_BRIGHTS:
					case OFF_BRIGHTS:
					case ON_BRAKES:
					case OFF_BRAKES:
					case ON_LLIGHTS:
					case OFF_LLIGHTS:
					case ON_LBRIGHTS:
					case OFF_LBRIGHTS:
					case ON_RLIGHTS:
					case OFF_RLIGHTS:
					case ON_RBRIGHTS:
					case OFF_RBRIGHTS:
					case ON_RUNNING_LIGHTS:
					case OFF_RUNNING_LIGHTS:
					case SPECIAL_CMD:
					case WIPER1:
					case WIPER2:
					case WIPER_OFF:
						send_serialother(cmd,tempx);
						add_msg_queue(cmd);
						break;

					case SET_PARAMS:
						send_param_msg();
						//printString2("set params");
						break;

					case SET_TIME:
						curtime2 = 0L;
						j = 0;
						memset(tempx,0,sizeof(tempx));

//						for(i = 2;i < msg_len;i+=2)
//							memcpy((void*)&tempx[j++],(char*)&msg_buf[i],1);
						for(i = 0;i < msg_len/2+2;i++)
						{
							tempx[i] = msg_buf2[i];
//							write_serial2(tempx[i]);
						}
						tempx[msg_len/2-2] = 'M';
						memset(temp_time,0,sizeof(temp_time));
						i = 0;
						pch = &tempx[0];

						while(*(pch++) != '/' && i < msg_len)
						{
							i++;
//							printf("%c",*pch);
						}
						memcpy(&temp_time[0],&tempx[0],i);
						i = atoi(temp_time);
//						printf("mon: %d\r\n",i - 1);
						pt->tm_mon = i - 1;
						i = 0;

						while(*(pch++) != '/' && i < msg_len)
						{
							i++;
//							printf("%c",*pch);
						}
						memset(temp_time,0,sizeof(temp_time));
						memcpy(temp_time,pch-i-1,i);
//						printf("%s\n",temp_time);
						i = atoi(temp_time);
						pt->tm_mday = i;
//						printf("day: %d\r\n",i);
				//		return 0;

						i = 0;
						while(*(pch++) != ' ' && i < msg_len)
						{
							i++;
//							printf("%c\r\n",*pch);
						}

						memset(temp_time,0,sizeof(temp_time));
						memcpy(temp_time,pch-3,2);
						i = atoi(temp_time);
						i += 100;
						pt->tm_year = i;
//						printf("year: %d\r\n",i-100);
				//		return 0;
						i = 0;

						while(*(pch++) != ':' && i < msg_len)
							i++;
						memset(temp_time,0,sizeof(temp_time));
						memcpy(temp_time,pch-i-1,i);
//						printf("%s \n",temp_time);
						i = atoi(temp_time);
						pt->tm_hour = i;
//						printf("hour: %d\r\n",i);
				//		return 0;

						i = 0;
						while(*(pch++) != ':' && i < msg_len)
							i++;
						memset(temp_time,0,sizeof(temp_time));
						memcpy(temp_time,pch-3,2);
//						printf("%s \n",temp_time);
						i = atoi(temp_time);
						pt->tm_min = i;
//						printf("min: %d\r\n",i);

						i = 0;
						while(*(pch++) != ' ' && i < msg_len)
							i++;
						memset(temp_time,0,sizeof(temp_time));
						memcpy(temp_time,pch-3,2);
//						printf("%s \n",temp_time);
						i = atoi(temp_time);
						pt->tm_sec = i;
//						printf("sec: %d\r\n",i);
//						printf("%c %x\n",*pch,*pch);
						if(*pch == 'P')
						{
//							printf("PM\n");
							pt->tm_hour += 12;
						}

						curtime2 = mktime(pt);
						stime(pcurtime2);
/*
						gettimeofday(&mtv, NULL);
						curtime2 = mtv.tv_sec;
						strftime(tempx,30,"%m-%d-%Y %T\0",localtime(&curtime2));
						printf("%s\n",tempx);
*/
//						time_set = 1;
//#endif
						break;

					case GET_TIME:
						gettimeofday(&mtv, NULL);
						curtime2 = mtv.tv_sec;
						strftime(tempx,30,"%m-%d-%Y %T\0",localtime(&curtime2));
						myprintf1(tempx);
						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx,GET_TIME);
						break;

					// adjust the 2x20 LCD screen on the IO box
					case LCD_SHIFT_LEFT:
//						shift_left();
						break;

					case LCD_SHIFT_RIGHT:
//						shift_right();
						break;

					case SCROLL_UP:
//						scroll_up();
						break;

					case SCROLL_DOWN:
//						scroll_down();
						break;

					// update the sched.log file with current log of events
					case BAD_MSG:
//						shutdown_all = 1;
//						myprintf1("bad msg");
						break;

					case DISCONNECT:
						if(test_sock() > 0)
						{
							close_tcp();
							//printString2("disconnected from socket\0");
//							printf("disconnecting...\r\n");
						}
						break;

					case SET_TEMP_LIMIT:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.engine_temp_limit = utemp;
//						sprintf(tempx,"engine temp limit: %.1f\0", convertF(ps.engine_temp_limit));
//						printString2(tempx);
						write_serial_buffer[0] = msg_buf[2];
						write_serial_buffer[1] = msg_buf[3];
						//send_serial(SET_TEMP_LIMIT);
						send_serialother(SET_TEMP_LIMIT, &write_serial_buffer[0]);
//						usleep(1000);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;
						
					case SET_FAN_ON:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.cooling_fan_on = utemp;
//						sprintf(tempx,"cooling fan on: %d", ps.cooling_fan_on);
//						sprintf(tempx,"cooling fan on: %.1f\0", convertF(ps.cooling_fan_on));
//						printString2(tempx);
//						write_serial_buffer[0] = msg_buf[2];
//						write_serial_buffer[1] = msg_buf[3];
//						send_serialother(SET_FAN_ON, &write_serial_buffer[0]);
//						usleep(1000);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;
						
					case SET_FAN_OFF:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.cooling_fan_off = utemp;
//						sprintf(tempx,"cooling fan off: %d",ps.cooling_fan_off);
//						sprintf(tempx,"cooling fan off: %.1f\0", convertF(ps.cooling_fan_off));
//						printString2(tempx);
//						write_serial_buffer[0] = msg_buf[2];
//						write_serial_buffer[1] = msg_buf[3];
//						send_serialother(SET_FAN_OFF, &write_serial_buffer[0]);
//						usleep(1000);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;

					case HIGH_REV_LIMIT:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.high_rev_limit = utemp;
						sprintf(tempx,"high rev limit: %d\0", ps.high_rev_limit);
						//printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						actual_high_rev_limit = atoi(hi_rev[ps.high_rev_limit]);
//						sprintf(tempx,"actual: %d",actual_high_rev_limit);
//						printString2(tempx);
						break;

					case LOW_REV_LIMIT:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.low_rev_limit = utemp;
						sprintf(tempx,"low rev limit: %d\0", ps.low_rev_limit);
						//printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						actual_low_rev_limit = atoi(lo_rev[ps.low_rev_limit]);
//						sprintf(tempx,"actual: %d",actual_low_rev_limit);
//						printString2(tempx);
						break;

					case LIGHTS_ON_DELAY:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.lights_on_delay = utemp;
//						sprintf(tempx,"lights on delay: %d\0", ps.lights_on_delay);
//						printString2(tempx);
						actual_lights_on_delay = lights_on_delay[ps.lights_on_delay];
						utemp = (UINT)actual_lights_on_delay;
						msg_buf[2] = (UCHAR)utemp;
						utemp >>= 8;
						msg_buf[3] = (UCHAR)utemp;
//						write_serial_buffer[0] = msg_buf[2];
//						write_serial_buffer[1] = msg_buf[3];
//						send_serialother(LIGHTS_ON_DELAY, &write_serial_buffer[0]);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;

					case SET_BLOWER1_TEMP:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.blower1_on = utemp;
//						sprintf(tempx,"blower 1 on: %d\0", ps.blower1_on);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;

					case SET_BLOWER2_TEMP: 
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.blower2_on = utemp;
//						sprintf(tempx,"blower 2 on: %d\0", ps.blower2_on);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;

					case SET_BLOWER3_TEMP:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.blower3_on = utemp;
//						sprintf(tempx,"blower 3 on: %d\0", ps.blower3_on);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;

					case SET_BLOWER_EN_TEMP:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.blower_enabled = utemp;
//						sprintf(tempx,"blower en on: %d\0", ps.blower_enabled);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;
					
					case SET_BATT_BOX_TEMP:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.batt_box_temp = utemp;
//						sprintf(tempx,"blower en on: %d\0", ps.batt_box_temp);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							//printString2(tempx);
						}
						break;

					case UPDATE_CONFIG:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.rpm_mph_update_rate = utemp;
						write_serial_buffer[0] = msg_buf[2];
						write_serial_buffer[1] = msg_buf[3];
						
						utemp = (UINT)msg_buf[5];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[4];
						ps.fpga_xmit_rate = utemp;
						write_serial_buffer[2] = msg_buf[4];
						write_serial_buffer[3] = msg_buf[5];
						
						utemp = (UINT)msg_buf[7];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[6];
						ps.high_rev_limit = utemp;
						write_serial_buffer[4] = msg_buf[6];
						write_serial_buffer[5] = msg_buf[7];
						actual_high_rev_limit = atoi(hi_rev[ps.high_rev_limit]);
						sprintf(tempx,"hi rev: %d",actual_high_rev_limit);
						printString3(tempx);
						
						utemp = (UINT)msg_buf[9];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[8];
						ps.low_rev_limit = utemp;
						write_serial_buffer[6] = msg_buf[8];
						write_serial_buffer[7] = msg_buf[9];
						actual_low_rev_limit = atoi(lo_rev[ps.low_rev_limit]);
						sprintf(tempx,"low rev: %d",actual_low_rev_limit);
						printString3(tempx);
						
						utemp = (UINT)msg_buf[11];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[10];
						ps.cooling_fan_on = utemp;
						// start loading serial buffer to send to STM32
						// as a SEND_CONFIG2 msg
						// only need to send temp data - low byte 1st
						write_serial_buffer[8] = msg_buf[10];
						write_serial_buffer[9] = msg_buf[11];
						//sprintf(tempx,"fan on: %d",ps.cooling_fan_on);
						sprintf(tempx,"fan on: %.1f\0", convertF(ps.cooling_fan_on));
						printString3(tempx);

						utemp = (UINT)msg_buf[13];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[12];
						ps.cooling_fan_off = utemp;
						write_serial_buffer[10] = msg_buf[12];
						write_serial_buffer[11] = msg_buf[13];
						sprintf(tempx,"fan off: %.1f\0", convertF(ps.cooling_fan_off));
						printString3(tempx);
						
						utemp = (UINT)msg_buf[15];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[14];
						ps.blower_enabled = utemp;
						write_serial_buffer[12] = msg_buf[14];
						write_serial_buffer[13] = msg_buf[15];
						sprintf(tempx,"blower en: %.1f\0", convertF(ps.blower_enabled));
						printString3(tempx);
						
						utemp = (UINT)msg_buf[17];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[16];
						ps.blower1_on = utemp;
						write_serial_buffer[14] = msg_buf[16];
						write_serial_buffer[15] = msg_buf[17];
						sprintf(tempx,"blower1: %.1f\0", convertF(ps.blower1_on));
						printString3(tempx);
						
						utemp = (UINT)msg_buf[19];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[18];
						ps.blower2_on = utemp;
						write_serial_buffer[16] = msg_buf[18];
						write_serial_buffer[17] = msg_buf[19];
						
						utemp = (UINT)msg_buf[21];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[20];
						ps.blower3_on = utemp;
						write_serial_buffer[18] = msg_buf[20];
						write_serial_buffer[19] = msg_buf[21];
						
						utemp = (UINT)msg_buf[23];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[22];
						ps.lights_on_delay = utemp;
						actual_lights_on_delay = lights_on_delay[ps.lights_on_delay];
						utemp = (UINT)actual_lights_on_delay;
						sprintf(tempx,"lights on delay: %d %d",actual_lights_on_delay,ps.lights_on_delay);
						printString3(tempx);

						utemp = (UINT)msg_buf[25];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[24];
						ps.engine_temp_limit = utemp;
						write_serial_buffer[20] = msg_buf[24];
						write_serial_buffer[21] = msg_buf[25];
						sprintf(tempx,"eng temp limit: %.1f\0", convertF(ps.engine_temp_limit));
						printString3(tempx);

						utemp = (UINT)msg_buf[27];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[26];
						ps.batt_box_temp = utemp;
						write_serial_buffer[22] = msg_buf[26];
						write_serial_buffer[23] = msg_buf[27];

						utemp = (UINT)msg_buf[29];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[28];
						ps.test_bank = utemp;

						utemp = (UINT)msg_buf[31];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[30];
						ps.password_timeout = utemp;
						write_serial_buffer[24] = msg_buf[30];
						write_serial_buffer[25] = msg_buf[31];

						utemp = (UINT)msg_buf[33];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[32];
						ps.password_retries = utemp;
						write_serial_buffer[26] = msg_buf[32];
						write_serial_buffer[27] = msg_buf[33];

						utemp = (UINT)msg_buf[35];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[34];
						ps.baudrate3 = utemp;

						//memcpy(&password[0],&msg_buf[36],4);
						j = 0;
						memset(password,0,PASSWORD_SIZE);
						for(i = 0;i < 7;i+= 2)
						{
							password[j] = write_serial_buffer[j + 20] = msg_buf[i+36];
							j++;
						}
						write_serial_buffer[j] = 0;
						password[4] = 0;
						//printf("\r\n%s\r\n",password);
						//printString2(password);
						send_serialother(SEND_CONFIG2, &write_serial_buffer[0]);
						usleep(500);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						//printString2("update config file");
						break;

					case GET_CONFIG2:
						//printString2("get config2");
						send_serialother(GET_CONFIG2,&write_serial_buffer[0]);
						break;

					case GET_VERSION:
						//printString2(version);
						send_status_msg(version);
						break;

					case REFRESH_LCD:
						init_LCD(1);
						break;

					case SERVER_UP:
						send_serialother(SERVER_UP,(UCHAR *)tempx);
						break;

					case SET_GPS_DATA:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						test1 = (int)utemp;
						utemp = (UINT)msg_buf[5];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[4];
						test2 = (int)utemp;
						
						checksum = 0;
						memset(tempx,0,sizeof(tempx));
						sprintf(tempx,"$PSRF103,%02d,00,%02d,01",test1,test2);
					
						for(i = 1;i < strlen(tempx);i++)
							checksum ^= tempx[i];

						sprintf(tempy,"*%02x",checksum);
						strcat(tempx,tempy);
						strcat(tempx,"\r\n");

						for(i = 0;i < 25;i++)
						{
							if(write_serial3(tempx[i]) < 0)
								printf("bad char comm 3\r\n");
							usleep(500);
						}
						break;

					case SET_GPS_BAUDRATE:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						test1 = (int)utemp;
						set_gps_baudrate(test1);
						break;

					case ENABLE_GPS_SEND_DATA:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						enable_gps_send_data = (int)utemp;
						break;

					case ENABLE_ADC_SEND_DATA:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						enable_adc_send_data = (int)utemp;
						//sprintf(tempx,"enable_adc_send_data: %d",enable_adc_send_data);
						//printString3(tempx);

						if(enable_adc_send_data == 0)
						{
							write_serial_buffer[0] = 0;
						}
						else
						{
							write_serial_buffer[0] = 0xFF;
						}
						send_serialother(ADC_GATE, &write_serial_buffer[0]);
						printString3(tempx);						
						break;

					case ADC_GATE:
						if(enable_adc_send_data == 0)
						{
							write_serial_buffer[0] = 0xFF;
							enable_adc_send_data = 1;
						}
						else
						{
							write_serial_buffer[0] = 0;
							enable_adc_send_data = 0;
						}
						send_serialother(ADC_GATE, &write_serial_buffer[0]);
						sprintf(tempx,"send ADC_GATE: %d",enable_adc_send_data);
						printString3(tempx);
						break;

					case SET_ADC_RATE:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						write_serial_buffer[0] = (UCHAR)utemp;
						send_serialother(SET_ADC_RATE, &write_serial_buffer[0]);
						sprintf(tempx,"adc rate: %d",utemp);
						printString3(tempx);
						break;

					case SET_RPM_MPH_RATE:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						write_serial_buffer[0] = (UCHAR)utemp;
						send_serialother(SET_RPM_MPH_RATE, &write_serial_buffer[0]);
//						sprintf(tempx,"rpm rate: %d",utemp);
//						printString3(tempx);
						break;

					case SET_FPGA_RATE:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						write_serial_buffer[0] = (UCHAR)utemp;
						send_serialother(SET_FPGA_RATE, &write_serial_buffer[0]);
						sprintf(tempx,"fpga rate: %d",utemp);
						printString3(tempx);
						break;

					case EXIT_PROGRAM:

exit_program:

						j = 0;
						if(reboot_on_exit == 1)
						{
							myprintf1("exit to shell\0");
						}
						else if(reboot_on_exit == 2)
						{
							myprintf1("rebooting...\0");
						}
						else if(reboot_on_exit == 3)
						{
							myprintf1("shutting down...\0");
						}
						else if(reboot_on_exit == 4)
						{
							myprintf1("upload new...\0");
						}

						// save the current list of events
						strcpy(tempx,"odometer.txt\0");
						fp = open((const char *)&tempx[0], O_RDWR | O_CREAT | O_TRUNC,
					//	fp = open((const char *)&tempx[0], O_RDWR | O_APPEND,
							S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
						if(fp < 0)
						{
							printf("can't create odometer.txt\n");
						}else
						{
	//							printf("writing to odometer.txt\r\n");
							sprintf(tempx,"%d\r\n\0",odometer);
							rc = write(fp,(const void *)tempx,strlen(tempx));
							close(fp);
						}

	/*
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,otpp,&oll);
	//							printf("%d %d\r\n",otp->port,otp->onoff);
							usleep(_1MS);
							change_output(otp->port,otp->onoff);
							change_output(otp->port,0);
							usleep(_1MS);
							ollist_insert_data(i,&oll,otp);
						}

						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);
	*/
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
	//							printf("%s\r\n",errmsg);
							//printString2(errmsg);
							myprintf1(errmsg);
						}

						// pulse the LED on the IO box before shutting down
#if 0
						for(i = 0;i < 20;i++)
						{
							setdioline(7,0);
							uSleep(0,TIME_DELAY/30);
							setdioline(7,1);
							uSleep(0,TIME_DELAY/30);
						}
						setdioline(7,0);
						uSleep(2,0);
						setdioline(7,1);

						// pulse the LED's on the card FWIW

						for(i = 0;i < 20;i++)
						{
							red_led(1);
							usleep(20000);
							red_led(0);
							green_led(1);
							usleep(20000);
							green_led(0);
						}
#endif
	//						send_serial(SYSTEM_DOWN);
	//						sprintf(tempx,"%s\n","SYSTEM DOWN");
	//						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SYSTEM_DOWN2);
						usleep(10000000);
						shutdown_all = 1;
						return 0;
						break;
				}								  // end of switch
			}									  // if rc > 0
		}else									  // if test_sock
		{
			uSleep(1,1000);
			if(shutdown_all == 1)
			{
//				printf("shutting down cmd host\r\n");
				return 0;
			}
		}
	}
	return test + 1;
}
/*********************************************************************/
// get preamble & msg len from client
// preamble is: {0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00,
// msg_len(lowbyte),msg_len(highbyte),0x00,0x00,0x00,0x00,0x00,0x00}
// returns message length
int get_msg(void)
{
	int len;
	UCHAR low, high;
	int ret;
	int i;

	UCHAR preamble[20];
	ret = recv_tcp(preamble,16,1);
	if(ret < 0)
	{
		//printString2("get_msg error\0");
		printHexByte(ret);
	}
/*
	printString2("\r\n");	

	for(i = 0;i < 16;i++)
		printHexByte(preamble[i]);
	printString2("\r\n");	
*/
	if(memcmp(preamble,pre_preamble,8) != 0)
		return -1;

	low = preamble[8];
	high = preamble[9];
	len = (int)(high);
	len <<= 8;
	len |= (int)low;

	return len;
}

/*********************************************************************/
void send_msg(int msg_len, UCHAR *msg, UCHAR msg_type)
{
	int len;
	int ret;
	int i;

	if(test_sock())
	{
		ret = send_tcp(&pre_preamble[0],8);
		msg_len++;
		send_tcp((UCHAR *)&msg_len,1);
		ret = 0;
		send_tcp((UCHAR *)&ret,1);

		for(i = 0;i < 6;i++)
			send_tcp((UCHAR *)&ret,1);

		send_tcp((UCHAR *)&msg_type,1);

		ret = 0;
		send_tcp((UCHAR *)&ret,1);

		for(i = 0;i < msg_len;i++)
		{
			send_tcp((UCHAR *)&msg[i],1);
			send_tcp((UCHAR *)&ret,1);
		}
	}
}
/*********************************************************************/
int recv_tcp(UCHAR *str, int strlen,int block)
{
	int ret = 0;
	char errmsg[20];
	memset(errmsg,0,20);
	if(test_sock())
	{
		pthread_mutex_lock( &tcp_read_lock);
		ret = get_sock(str,strlen,block,&errmsg[0]);
		pthread_mutex_unlock(&tcp_read_lock);
		if(ret < 0 && (strcmp(errmsg,"Success") != 0))
		{
			myprintf1(errmsg);
//			sprintf(errmsg,"errno: %d\0",ret);
//			printString2(errmsg);
		}
	}
	else
	{
		strcpy(errmsg,"sock closed");
		ret = -1;
	}
	return ret;
}

/*********************************************************************/
int send_tcp(UCHAR *str,int len)
{
	int ret = 0;
	char errmsg[60];
	memset(errmsg,0,60);
	pthread_mutex_lock( &tcp_write_lock);
	ret = put_sock(str,len,1,&errmsg[0]);
	pthread_mutex_unlock(&tcp_write_lock);
	if(ret < 0 && (strcmp(errmsg,"Success") != 0))
	{
		if(same_msg == 0)
			myprintf1(errmsg);
		same_msg = 1;
	}
	else same_msg = 0;
	return ret;
}

/*********************************************************************/
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg)
{
	int rc = 0;
	char extra_msg[10];
	if(test_sock())
	{
		if(block)
// block
			rc = send(global_socket,buf,buflen,MSG_WAITALL);
		else
// don't block
			rc = send(global_socket,buf,buflen,MSG_DONTWAIT);
		if(rc < 0 && errno != 11)
		{
			strcpy(errmsg,strerror(errno));
			sprintf(extra_msg," %d",errno);
			strcat(errmsg,extra_msg);
			strcat(errmsg," put_sock");
			close_tcp();
		}else strcpy(errmsg,"Success\0");
	}
	else
	{
// this keeps printing out until the client logs on
		strcpy(errmsg,"sock closed");
		rc = -1;
	}
	return rc;
}

/*********************************************************************/
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg)
{
	int rc;
	char extra_msg[10];
	if(block)
		rc = recv(global_socket,buf,buflen,MSG_WAITALL);
	else
		rc = recv(global_socket,buf,buflen,MSG_DONTWAIT);
	if(rc < 0 && errno != 11)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," get_sock");
	}else strcpy(errmsg,"Success\0");
	return rc;
}

/*********************************************************************/
static void format_param_msg(void)
{
	int temp_conv;

	temp_conv = (int)msg_buf[2];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[3];
	ps.cooling_fan_on = temp_conv;

	temp_conv = (int)msg_buf[4];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[5];
	ps.cooling_fan_off = temp_conv;

	temp_conv = (int)msg_buf[6];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[7];
	ps.blower_enabled = temp_conv;

	temp_conv = (int)msg_buf[8];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[9];
	ps.blower3_on = temp_conv;

	temp_conv = (int)msg_buf[10];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[11];
	ps.blower2_on = temp_conv;

	temp_conv = (int)msg_buf[12];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[13];
	ps.blower1_on = temp_conv;

	temp_conv = (int)msg_buf[14];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[15];
	ps.high_rev_limit = temp_conv;

	temp_conv = (int)msg_buf[16];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[17];
	ps.low_rev_limit = temp_conv;

	temp_conv = (int)msg_buf[18];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[19];
	ps.rpm_mph_update_rate = temp_conv;
	
	temp_conv = (int)msg_buf[22];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[23];
	ps.fpga_xmit_rate = temp_conv;
	
	temp_conv = (int)msg_buf[24];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[25];
	ps.engine_temp_limit = temp_conv;

	temp_conv = (int)msg_buf[26];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[27];
	ps.test_bank = temp_conv;
	
}
/*********************************************************************/
void send_param_msg(void)
{
	char tempx[40];

	sprintf(tempx, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s\0",
														ps.rpm_mph_update_rate,
														ps.fpga_xmit_rate,
														ps.high_rev_limit,
														ps.low_rev_limit,
														ps.cooling_fan_on,
														ps.cooling_fan_off,
														ps.blower_enabled,
														ps.blower1_on,
														ps.blower2_on,
														ps.blower3_on,
														ps.lights_on_delay,
														ps.engine_temp_limit,
														ps.batt_box_temp,
														ps.test_bank,
														ps.password_timeout,
														ps.password_retries,
														ps.baudrate3,
														password);
	send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SEND_CONFIG);
	printString3(tempx);
	sprintf(tempx, "config file ok: %d",config_file_ok);
	printString3(tempx);
}
/*********************************************************************/
void send_status_msg(char *msg)
{
	send_msg(strlen((char*)msg)*2,(UCHAR*)msg, SEND_STATUS);
}

void set_gps_baudrate(int baudrate)
{
	char tempx[50];
	char tempy[10];
	int test2;
	UCHAR checksum;
	int i;
	
	memset(tempx,0,sizeof(tempx));
	switch(baudrate)
	{
		case 0:
			test2 = 4800;
			break;
		case 1:
			test2 = 9600;
			break;
		case 2:
			test2 = 19200;
			break;
		case 3:
			test2 = 38400;
			break;
		default:
			test2 = 4800;
			break;
	}
	sprintf(tempx,"$PSRF100,1,%d,8,1,0",test2);
	checksum = 0;
	for(i = 1;i < strlen(tempx);i++)
	{
		checksum ^= tempx[i];
	}
	sprintf(tempy,"*%02x",checksum);
	strcat(tempx,tempy);
	strcat(tempx,"\r\n");
	for(i = 0;i < strlen(tempx);i++)
	{
		if(write_serial3(tempx[i]) < 0)
			printf("bad char comm 3\r\n");
		usleep(500);
	}
	close_serial3();
	usleep(1000);
	init_serial3(baudrate);
}