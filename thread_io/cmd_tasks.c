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

extern pthread_mutex_t     tcp_read_lock;
extern pthread_mutex_t     tcp_write_lock;

#define TOGGLE_OTP otp->onoff = (otp->onoff == 1?0:1)

CMD_STRUCT cmd_array[116] =
{
	{		NON_CMD,"NON_CMD\0" },
	{		ENABLE_START,"ENABLE_START\0" },
	{		STARTER_OFF,"STARTER_OFF\0" },
	{		ON_ACC,"ON_ACC\0" },
	{		OFF_ACC,"OFF_ACC\0" },
	{		ON_FUEL_PUMP,"ON_FUEL_PUMP\0" },
	{		OFF_FUEL_PUMP,"OFF_FUEL_PUMP\0" },
	{		ON_FAN,"ON_FAN\0" },
	{		OFF_FAN,"OFF_FAN\0" },
	{		ON_LIGHTS,"ON_LIGHTS\0" },
	{		OFF_LIGHTS,"OFF_LIGHTS\0" },
	{		ON_BRIGHTS,"ON_BRIGHTS\0" },
	{		OFF_BRIGHTS,"OFF_BRIGHTS\0" },
	{		BLANK,"BLANK\0" },
	{		ESTOP_SIGNAL,"ESTOP_SIGNAL\0" },
	{		ON_BRAKES,"ON_BRAKES\0" },
	{		OFF_BRAKES,"OFF_BRAKES\0" },
	{		ON_RUNNING_LIGHTS,"ON_RUNNING_LIGHTS\0" },
	{		OFF_RUNNING_LIGHTS,"OFF_RUNNING_LIGHTS\0" },
	{		SPECIAL_CMD,"SPECIAL_CMD\0" },
	{		START_SEQ,"START_SEQ\0" },
	{		SHUTDOWN,"SHUTDOWN\0" },
	{		ON_LLIGHTS,"ON_LLIGHTS\0" },
	{		OFF_LLIGHTS,"OFF_LLIGHTS\0" },
	{		ON_LBRIGHTS,"ON_LBRIGHTS\0" },
	{		OFF_LBRIGHTS,"OFF_LBRIGHTS\0" },
	{		ON_RLIGHTS,"ON_RLIGHTS\0" },
	{		OFF_RLIGHTS,"OFF_RLIGHTS\0" },
	{		ON_RBRIGHTS,"ON_RBRIGHTS\0" },
	{		OFF_RBRIGHTS,"OFF_RBRIGHTS\0" },
	{		BLOWER1,"BLOWER1\0" },
	{		BLOWER2,"BLOWER2\0" },
	{		BLOWER3,"BLOWER3\0" },
	{		BLOWER_OFF,"BLOWER_OFF\0" },
	{		WIPER1,"WIPER1\0" },
	{		WIPER2,"WIPER2\0" },
	{		WIPER_OFF,"WIPER_OFF\0" },
	{		STOP_MBOX_XMIT,"STOP_MBOX_XMIT\0" },
	{		START_MBOX_XMIT,"START_MBOX_XMIT\0" },
	{		STOP_AVR_XMIT,"STOP_AVR_XMIT\0" },
	{		START_AVR_XMIT,"START_AVR_XMIT\0" },
	{		SEND_TIME_DATA,"SEND_TIME_DATA\0" },
	{		SHUTDOWN_IOBOX,"SHUTDOWN_IOBOX\0" },
	{		REBOOT_IOBOX,"REBOOT_IOBOX\0" },
	{		TEST_ALL_IO,"TEST_ALL_IO\0" },
	{		SEND_ODATA,"SEND_ODATA\0" },
	{		SAVE_TO_DISK,"SAVE_TO_DISK\0" },
	{		GET_DIR,"GET_DIR\0" },
	{		LCD_SHIFT_RIGHT,"LCD_SHIFT_RIGHT\0" },
	{		LCD_SHIFT_LEFT,"LCD_SHIFT_LEFT\0" },
	{		SCROLL_UP,"SCROLL_UP\0" },
	{		SCROLL_DOWN,"SCROLL_DOWN\0" },
	{		ENABLE_LCD,"ENABLE_LCD\0" },
	{		SET_TIME,"SET_TIME\0" },
	{		GET_TIME,"GET_TIME\0" },
	{		SHOW_ODATA,"SHOW_ODATA\0" },
	{		NEW_PASSWORD1,"NEW_PASSWORD1\0" },
	{		SET_SERIAL_RECV_ON,"SET_SERIAL_RECV_ON\0" },
	{		SET_SERIAL_RECV_OFF,"SET_SERIAL_RECV_OFF\0" },
	{		TEST_LEFT_BLINKER,"TEST_LEFT_BLINKER\0" },
	{		TEST_RIGHT_BLINKER,"TEST_RIGHT_BLINKER\0" },
	{		RE_ENTER_PASSWORD,"RE_ENTER_PASSWORD\0" },
	{		DISCONNECT,"DISCONNECT\0" },
	{		CLOSE_DB,"CLOSE_DB\0" },
	{		OPEN_DB,"OPEN_DB\0" },
	{		BAD_MSG,"BAD_MSG\0" },
	{		CURRENT_TIME,"CURRENT_TIME\0" },
	{		SET_PARAMS,"SET_PARAMS\0" },
	{		EXIT_PROGRAM,"EXIT_PROGRAM\0" },
	{		ENGINE_TEMP,"ENGINE_TEMP\0" },
	{		INDOOR_TEMP,"INDOOR_TEMP\0" },
	{		SEND_RT_VALUES,"SEND_RT_VALUES\0" },
	{		ENGINE_RUNTIME,"ENGINE_RUNTIME\0" },
	{		SERVER_UPTIME,"SERVER_UPTIME\0" },
	{		SEND_CONFIG,"SEND_CONFIG\0" },
	{		SEND_MSG,"SEND_MSG\0" },
	{		SEND_RPM,"SEND_RPM\0" },
	{		SEND_MPH,"SEND_MPH\0" },
	{		NAV_UP,"NAV_UP\0" },
	{		NAV_DOWN,"NAV_DOWN\0" },
	{		NAV_SIDE,"NAV_SIDE\0" },
	{		NAV_CLICK,"NAV_CLICK\0" },
	{		NAV_CLOSE,"NAV_CLOSE\0" },
	{		NAV_NUM,"NAV_NUM\0" },
	{		SEND_STATUS,"SEND_STATUS\0" },
	{		SERVER_UP,"SERVER_UP\0" },
	{		SERVER_DOWN,"SERVER_DOWN\0" },
	{		UPLOAD_NEW,"UPLOAD_NEW\0" },
	{		UPLOAD_OTHER,"UPLOAD_OTHER\0" },
	{		UPLOAD_NEW_PARAM,"UPLOAD_NEW_PARAM\0" },
	{		SET_TEMP_LIMIT,"SET_TEMP_LIMIT\0" },
	{		SET_FAN_ON,"SET_FAN_ON\0" },
	{		SET_FAN_OFF,"SET_FAN_OFF\0" },
	{		HIGH_REV_LIMIT,"HIGH_REV_LIMIT\0" },
	{		LOW_REV_LIMIT,"LOW_REV_LIMIT\0" },
	{		LIGHTS_ON_DELAY,"LIGHTS_ON_DELAY\0" },
	{		SET_BLOWER_EN_TEMP,"SET_BLOWER_EN_TEMP\0" },
	{		SET_BLOWER1_TEMP,"SET_BLOWER1_TEMP\0" },
	{		SET_BLOWER2_TEMP,"SET_BLOWER2_TEMP\0" },
	{		SET_BLOWER3_TEMP,"SET_BLOWER3_TEMP\0" },
	{		SET_BATT_BOX_TEMP,"SET_BATT_BOX_TEMP\0" },
	{		TEMP_TOO_HIGH,"TEMP_TOO_HIGH\0" },
	{		GET_VERSION,"GET_VERSION\0" },
	{		SVR_CMD,"SVR_CMD\0" },
	{		HOME_SVR_ON,"HOME_SVR_ON\0" },
	{		HOME_SVR_OFF,"HOME_SVR_OFF\0" },
	{		UPDATE_CONFIG,"UPDATE_CONFIG\0" },
	{		SEND_CONFIG2,"SEND_CONFIG2\0" },
	{		GET_CONFIG2,"GET_CONFIG2\0" },
	{		CLIENT_CONNECTED,"CLIENT_CONNECTED\0" },
	{		SERVER_CONNECTED,"SERVER_CONNECTED\0" },
	{		SET_KEYMODE,"SET_KEYMODE\0" },
	{		PASSWORD_OK,"PASSWORD_OK\0" },
	{		PASSWORD_BAD,"PASSWORD_BAD\0" },
	{		SET_PASSWORD_TIMEOUT,"SET_PASSWORD_TIMEOUT\0" },
	{		SET_PASSWORD_RETRIES,"SET_PASSWORD_RETRIES\0" }
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
	UCHAR test2;
	UCHAR _port,_onoff,_type,_time_delay;
	int _input;
	int testx;
	UCHAR rec_no;
	struct dirent **namelist;
	DIR *d;
	struct dirent *dir;
	int num;
	UCHAR test_io_num = 0;
	char tempx[200];
	UCHAR tempy[30];
	char temp_time[5];
	UCHAR utemp1[2];
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
	char version[15] = "sched v1.30\0";
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
		rc = olLoadConfig(oFileName,&oll,osize,errmsg);
		if(rc > 0)
		{
			myprintf1(errmsg);
		}
	}
	init_ips();

	same_msg = 0;
	lcd_init();

// flash green and red led's to signal we are up (if LCD screen not attached)
//#if 0
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
//#endif
//	myprintf1("start....\0");

	myprintf1(version);
	memset(rt_file_data,0,sizeof(rt_file_data));
	odometer = 0;
	trip = 0;
	tcp_connected_time = 0;
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
			tcp_connected_time = 0;
			if(cmd != LCD_SHIFT_RIGHT && cmd != LCD_SHIFT_LEFT && cmd != SCROLL_DOWN && 
				cmd != SCROLL_UP && cmd > 0)
//					&& cmd != GET_TIME && cmd != SET_TIME && cmd > 0)
				myprintf2(cmd_array[cmd].cmd_str,cmd);
#if 0
			if(cmd > 0)
			{
				sprintf(tempx, "cmd: %d %s\0",cmd,cmd_array[cmd].cmd_str);
				printString2(tempx);
//				printf("%s\r\n",tempx);
//				printf("cmd: %d %s\r\n",cmd,cmd_array[cmd].cmd_str);
			}
#endif
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
					case TEST_ALL_IO:
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
					case BLOWER_OFF:
					case BLOWER1:
					case BLOWER2:
					case BLOWER3:
					case WIPER1:
					case WIPER2:
					case WIPER_OFF:
						//send_serialother(cmd,tempx);
						add_msg_queue(cmd);
						strcpy(tempx,cmd_array[cmd].cmd_str);
//						printString2(tempx);
//						sprintf(tempx,"%d %d %d %d ",cmd,trunning_seconds,trunning_minutes,trunning_hours);
//						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SEND_MSG);
						break;

					// the next 2 turns on or off the serial port to the PIC24 (monster box)
					// so if the monster box is switched off for maintenance, the IO box
					// won't get false signals to turn on or off ports
/*
					case SPECIAL_CMD:
						for(i = 1;i < 20;i++)
							add_msg_queue(i);
						break;
*/
					case SET_SERIAL_RECV_ON:
						if(serial_recv_on == 0)
						{
							if(fd = init_serial() < 0)
							{
								myprintf1("can't open comm port 1\0");
								return 0;
							}
							serial_recv_on = 1;
						}
						break;

					case SET_SERIAL_RECV_OFF:
						if(serial_recv_on == 1)
						{
							close_serial();
							serial_recv_on = 0;
						}
						break;

					// allows password to be changed from laptop via tcp connection
					case NEW_PASSWORD1:
						rc = 0;
//						memset(tempx,0,50);
						send_msg(strlen((char*)password)*2,(UCHAR*)password, NEW_PASSWORD1);
						printString2(password);
						
//						rc += recv_tcp((UCHAR *)&tempx[0],12,1);
//						memcpy((void *)&tempx[0],&msg_buf[0],12);
//						myprintf2("read: ",rc);
						break;

					// clear the lcd screen and redraw the menus and rt labels
					// signal to monster box that the password must be re-entered in case of
					// getting out of vehicle (hijack prevention)
					case RE_ENTER_PASSWORD:
						break;

					// this gets a cmd from the client with extra data that sets the params
					// TODO: first send the current params that are used by the server and
					// have been saved to disk in a config file
					case SET_PARAMS:
						send_param_msg();
						printString2("set params");
#if 0
						memset(tempx,0,sizeof(tempx));
//						printf("msg len: %d\r\n",msg_len/2-2);
//						printf("\r\n");

						j = 0;
						for(i = 2;i < msg_len;i+=2)
						{
							memcpy((void*)&tempx[j++],(char*)&msg_buf[i],1);
//							msg_buf[msg_len/2] = 0;

//							if(msg_buf[i] > 0x1f && msg_buf[i] < 0x7e)
						}
/*
						for(i = 2;i < msg_len;i++)
						{
							printf("%d ",msg_buf[i]);
						}
*/
						format_param_msg();

						sprintf(tempx,"\r\nrpm_update: %d\r\n",ps.rpm_update_rate);
						printString2(tempx);
						sprintf(tempx,"mph_update: %d\r\n",ps.mph_update_rate);
						printString2(tempx);
						sprintf(tempx,"fpga_xmit_rate: %d\r\n",ps.fpga_xmit_rate);
						printString2(tempx);
						sprintf(tempx,"high_rev_limit: %d\r\n",ps.high_rev_limit);
						printString2(tempx);
						sprintf(tempx,"low_rev_limit: %d\r\n",ps.low_rev_limit);
						printString2(tempx);
						sprintf(tempx,"cooling fan: %d\r\n",ps.cooling_fan_on);
						printString2(tempx);
						sprintf(tempx,"fan off: %d\r\n",ps.cooling_fan_off);
						printString2(tempx);
						sprintf(tempx,"blwer en: %d\r\n",ps.blower_enabled);
						printString2(tempx);
						sprintf(tempx,"blower1: %d\r\n",ps.blower1_on);
						printString2(tempx);
						sprintf(tempx,"blower2: %d\r\n",ps.blower2_on);
						printString2(tempx);
						sprintf(tempx,"blower3: %d\r\n",ps.blower3_on);
						printString2(tempx);
						sprintf(tempx,"temp limit: %d\r\n",ps.engine_temp_limit);
						printString2(tempx);
						sprintf(tempx,"test_bank: %d\r\n",ps.test_bank);
						printString2(tempx);

						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							printString2(errmsg);
							myprintf1(errmsg);
						}
#endif
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
//						printString2("\r\n");	

//						printString2(tempx);
//						printString2("\r\n");

//						printString2(tempx);	
//						msg_buf[msg_len/2] = 0;
//							if(msg_buf[i] > 0x1f && msg_buf[i] < 0x7e)
//								printf("%c",msg_buf[i]);
//						printf("set_time: %d %s\r\n",msg_len,tempx);
//						for(i = 0;i < msg_len*2;i++)
//							printf("%02x ",msg_buf[i]);
//#if 0
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

					case SEND_ODATA:
						j = 0;
						k = 0;
//						printf("\r\nmsg_len: %d\r\n",msg_len);
						memset(tempy,0,sizeof(tempy));
						for(i = 2;i < msg_len+2;i+=2)
						{
							tempy[j++] = msg_buf[i];
						}
						pch = (char*)&tempy[0];
						i = 0;
						while(*pch != 0x7B)				// open bracket
						{
							pch++;
							i++;
						}
						pch++;
						memset(tempx,0,sizeof(tempx));
						memcpy(tempx,tempy,i);
						_port = (UCHAR)atoi(tempx);
//						printf("port: %d\r\n",_port);
						i = 0;
						while(*pch != 0x7C)				// bar
						{
							pch++;
							i++;
						}
						pch++;
						memset(tempx,0,sizeof(tempx));
						memcpy(tempx,pch-i-1,i);
						_onoff = (UCHAR)atoi(tempx);
//						printf("onoff: %d\r\n",_onoff);
						i = 0;
						while(*pch != 0x7D)				// close bracket
						{
							pch++;
							i++;
						}
						pch++;
						memset(tempx,0,sizeof(tempx));
						memcpy(tempx,pch-i-1,i);
						_type = (UCHAR)atoi(tempx);
//						printf("type: %d\r\n",_type);

						i = 0;
						while(*pch != 0x7E)				// tilde
						{
							pch++;
							i++;
						}
						pch++;
						memset(tempx,0,sizeof(tempx));
						memcpy(tempx,pch-i-1,i);
						_time_delay = (UCHAR)atoi(tempx);
//						printf("time_delay: %d\r\n",_time_delay);
						
						i = 0;
						while(*pch != 0x7A)				// 'z'
						{
							pch++;
							i++;
						}
						pch++;
						memset(tempx,0,sizeof(tempx));
						memcpy(tempx,pch-i-1,i);
						_input = atoi(tempx);
//						printf("input: %d\r\n",_input);

						i = 0;
					
						memset(tempx,0,sizeof(tempx));
						memcpy(tempx,pch,34);
//						printf("label: %s\r\n",tempx);

						ollist_find_data(_port,&otp,&oll);
						otp->onoff = _onoff;
						otp->type = _type;
						otp->time_delay = _time_delay;
						otp->input_port = (UCHAR)_input;
						strcpy(otp->label,tempx);
//						printf("%s\r\n",otp->label);
						ollist_insert_data(_port,&oll,otp);
/*
						for(i = STARTER;i < TESTOUTPUT24;i++)
						{
							ollist_find_data(i,otpp,&oll);
							printf("%d %d %d %d %s\r\n",otp->port,otp->onoff, otp->type,
								otp->time_delay,otp->label);
						}
*/
						break;

					// send all the data to the laptop

					case SHOW_ODATA:
/* this crashes ???
						rc = 0;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,&otp,&oll);
							printf("%d %d %d %s\r\n",otp->port,otp->onoff,otp->type,otp->label);
//							printString2(tempx);
						}
//						myprintf1("done\0");
*/
						break;

					// get all the data from the laptop
#if 0
					case RECV_ALL_ODATA:
						rc = 0;
						otp = &tempo1;
						recv_tcp((UCHAR*)&uch_fname_index,1,1);
//						myprintf2("index: \0",uch_fname_index);
						if(uch_fname_index > 0)
						{
							uch_fname_index--;
							fname_index = (int)uch_fname_index;
							olLoadConfig(dat_names[fname_index],&oll,isize,errmsg);
						}
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,&otp,&oll);
							rc += send_tcp((UCHAR *)otp,sizeof(O_DATA));
						}
						break;
*/
					// get the names and time/datestamps of only the dat files
					// on the IO box and send to the laptop
#endif
					case GET_DIR:
						d = opendir( "." );
						if( d == NULL )
						{
							myprintf1("bad OPEN_DIR\0");
//							return -1;
						}
						num = 0;

						memset(dat_names,0,NUM_DAT_NAMES*DAT_NAME_STR_LEN);
						memset(tempx,0,sizeof(tempx));

						while( ( dir = readdir( d ) ) && num < NUM_DAT_NAMES-1)
						{
							if(strcmp( dir->d_name, "." ) == 0 || \
								strcmp( dir->d_name, ".." ) == 0 || dir->d_type == DT_DIR)
								continue;

							memset(tempx,0,sizeof(tempx));
							strcpy(tempx,dir->d_name);
//							printf("%s\r\n",tempx);
							pch = tempx;
							j = 0;

							while(*pch++ != '.' && j < DAT_NAME_STR_LEN)
								j++;

							strncpy(tempx,pch,j+1);

							if(dir->d_type == DT_REG && strcmp(tempx,"dat") == 0 )
//							if(dir->d_type == DT_REG)
							{
								strcpy(dat_names[num++],dir->d_name);
							}
						}
						closedir( d );
//						memset(tempx,0x20,sizeof(tempx));
						send_tcp((UCHAR*)&num,1);
						for(i = 0;i < num;i++)
						{
							cmd = (UCHAR)strlen(dat_names[i]);
							send_tcp(&cmd,1);
							send_tcp((UCHAR *)&dat_names[i],cmd);
						}
						for(i = 0;i < num;i++)
						{
							getFileCreationTime(dat_names[i],tempx);
							j = GetFileFormat(dat_names[i]);
							if(j < 0)
								test2 = 0xff;
							else
								test2 = (UCHAR)j;
							send_tcp((UCHAR*)&tempx,TDATE_STAMP_STR_LEN);
							send_tcp((UCHAR*)&test2,1);
						}
						break;

					// save what's currenly in the databases to the disk on the IO box
					case SAVE_TO_DISK:
/*
						for(i = 0;i < 20;i++)
						{
							ollist_find_data(i,otpp,&oll);
							printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->type);
						}
*/
						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);
//						printf("done\r\n");
						break;

					// adjust the 2x20 LCD screen on the IO box
					case LCD_SHIFT_LEFT:
						shift_left();
						break;

					case LCD_SHIFT_RIGHT:
						shift_right();
						break;

					case SCROLL_UP:
						scroll_up();
						break;

					case SCROLL_DOWN:
						scroll_down();
						break;

					case CLOSE_DB:
						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);
						break;

					case OPEN_DB:
						if(access(oFileName,F_OK) != -1)
						{
							rc = olLoadConfig(oFileName,&oll,osize,errmsg);
							if(rc > 0)
							{
								myprintf1(errmsg);
							}
						}
/*
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,otpp,&oll);
							printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->type);
						}
*/
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
							tcp_connected_time = 0;
						}
						break;

					case STOP_MBOX_XMIT:
/*
						sprintf(tempx,"engine temp limit: %.1f\0", convertF(ps.engine_temp_limit));
						printString2(tempx);
						sprintf(tempx,"cooling fan on:    %.1f\0", convertF(ps.cooling_fan_on));
						printString2(tempx);
						sprintf(tempx,"cooling fan off:   %.1f\0", convertF(ps.cooling_fan_off));
						printString2(tempx);
						sprintf(tempx,"blower 1 on: %.1f\0", convertF(ps.blower1_on));
						printString2(tempx);
						sprintf(tempx,"blower 2 on: %.1f\0", convertF(ps.blower2_on));
						printString2(tempx);
						sprintf(tempx,"blower 3 on: %.1f\0", convertF(ps.blower2_on));
						printString2(tempx);
						sprintf(tempx,"blower en: %.1f\0", convertF(ps.blower_enabled));
						printString2(tempx);
						sprintf(tempx,"battery box temp: %.1f\0", convertF(ps.batt_box_temp));
						printString2(tempx);
						sprintf(tempx,"\r\nrpm_update: %d\r\n",ps.rpm_update_rate);
						printString2(tempx);
						sprintf(tempx,"mph_update: %d\r\n",ps.mph_update_rate);
						printString2(tempx);
						sprintf(tempx,"fpga_xmit_rate: %d\r\n",ps.fpga_xmit_rate);
						printString2(tempx);
						sprintf(tempx,"high_rev_limit: %d\r\n",ps.high_rev_limit);
						printString2(tempx);
						sprintf(tempx,"low_rev_limit: %d\r\n",ps.low_rev_limit);
						printString2(tempx);
*/
//						send_serial(STOP_MBOX_XMIT);
						//printString2("System Down\0");
						break;

					case START_MBOX_XMIT:
						send_serial(START_MBOX_XMIT);
						//printString2("System Up\0");
						break;

					case STOP_AVR_XMIT:
						send_serial(STOP_AVR_XMIT);
						//printString2("xmit to AVR off\0");
						break;

					case START_AVR_XMIT:
						send_serial(START_AVR_XMIT);
						//printString2("xmit to AVR on\0");
						break;

						for(i = 4;i < TRRIGHTBLINKER;i++)
						{
							ollist_find_data(i,otpp,&oll);
							sprintf(tempx,"%s %d %d\r\n",otp->label, otp->port, otp->onoff);
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
						}
						break;

					case HIGH_REV_LIMIT:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.high_rev_limit = utemp;
						sprintf(tempx,"high rev limit: %d\0", ps.high_rev_limit);
						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							printString2(tempx);
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
						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
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
							printString2(tempx);
						}
						break;

					case SET_PASSWORD_TIMEOUT:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.password_timeout = utemp;
//						sprintf(tempx,"blower en on: %d\0", ps.batt_box_temp);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							printString2(tempx);
						}
						break;
	
					case SET_PASSWORD_RETRIES:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.password_retries = utemp;
//						sprintf(tempx,"blower en on: %d\0", ps.batt_box_temp);
//						printString2(tempx);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						if(i < 0)
						{
//							printf("%s\r\n",errmsg);
							myprintf1(errmsg);
							sprintf(tempx,"%s %d",errmsg,i);
							printString2(tempx);
						}
						break;
	
					case UPDATE_CONFIG:
						utemp = (UINT)msg_buf[3];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[2];
						ps.rpm_update_rate = utemp;
						
						utemp = (UINT)msg_buf[5];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[4];
						ps.mph_update_rate = utemp;
						
						utemp = (UINT)msg_buf[7];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[6];
						ps.fpga_xmit_rate = utemp;
						
						utemp = (UINT)msg_buf[9];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[8];
						ps.high_rev_limit = utemp;
						actual_high_rev_limit = atoi(hi_rev[ps.high_rev_limit]);
						sprintf(tempx,"hi rev: %d",actual_high_rev_limit);
						printString2(tempx);
						
						utemp = (UINT)msg_buf[11];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[10];
						ps.low_rev_limit = utemp;
						actual_low_rev_limit = atoi(lo_rev[ps.low_rev_limit]);
						sprintf(tempx,"low rev: %d",actual_low_rev_limit);
						printString2(tempx);
						
						utemp = (UINT)msg_buf[13];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[12];
						ps.cooling_fan_on = utemp;
						// start loading serial buffer to send to STM32
						// as a SEND_CONFIG2 msg
						// only need to send temp data - low byte 1st
						write_serial_buffer[0] = msg_buf[12];
						write_serial_buffer[1] = msg_buf[13];
						//sprintf(tempx,"fan on: %d",ps.cooling_fan_on);
						sprintf(tempx,"fan on: %.1f\0", convertF(ps.cooling_fan_on));
						printString2(tempx);

						utemp = (UINT)msg_buf[15];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[14];
						ps.cooling_fan_off = utemp;
						write_serial_buffer[2] = msg_buf[14];
						write_serial_buffer[3] = msg_buf[15];
						sprintf(tempx,"fan off: %.1f\0", convertF(ps.cooling_fan_off));
						printString2(tempx);
						
						utemp = (UINT)msg_buf[17];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[16];
						ps.blower_enabled = utemp;
						write_serial_buffer[4] = msg_buf[16];
						write_serial_buffer[5] = msg_buf[17];
						sprintf(tempx,"blower en: %.1f\0", convertF(ps.blower_enabled));
						printString2(tempx);
						
						utemp = (UINT)msg_buf[19];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[18];
						ps.blower1_on = utemp;
						write_serial_buffer[6] = msg_buf[18];
						write_serial_buffer[7] = msg_buf[19];
						sprintf(tempx,"blower1: %.1f\0", convertF(ps.blower1_on));
						printString2(tempx);
						
						utemp = (UINT)msg_buf[21];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[20];
						ps.blower2_on = utemp;
						write_serial_buffer[8] = msg_buf[20];
						write_serial_buffer[9] = msg_buf[21];
						
						utemp = (UINT)msg_buf[23];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[22];
						ps.blower3_on = utemp;
						write_serial_buffer[10] = msg_buf[22];
						write_serial_buffer[11] = msg_buf[23];
						
						utemp = (UINT)msg_buf[25];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[24];
						ps.lights_on_delay = utemp;
						actual_lights_on_delay = lights_on_delay[ps.lights_on_delay];
						utemp = (UINT)actual_lights_on_delay;
						sprintf(tempx,"lights on delay: %d %d",actual_lights_on_delay,ps.lights_on_delay);
						printString2(tempx);

						utemp = (UINT)msg_buf[27];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[26];
						ps.engine_temp_limit = utemp;
						write_serial_buffer[12] = msg_buf[26];
						write_serial_buffer[13] = msg_buf[27];
						sprintf(tempx,"eng temp limit: %.1f\0", convertF(ps.engine_temp_limit));
						printString2(tempx);

						utemp = (UINT)msg_buf[29];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[28];
						ps.batt_box_temp = utemp;
						write_serial_buffer[14] = msg_buf[28];
						write_serial_buffer[15] = msg_buf[29];

						utemp = (UINT)msg_buf[31];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[30];
						ps.test_bank = utemp;

						utemp = (UINT)msg_buf[33];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[32];
						ps.password_timeout = utemp;
						write_serial_buffer[16] = msg_buf[32];
						write_serial_buffer[17] = msg_buf[33];

						utemp = (UINT)msg_buf[35];
						utemp <<= 8;
						utemp |= (UINT)msg_buf[34];
						ps.password_retries = utemp;
						write_serial_buffer[18] = msg_buf[34];
						write_serial_buffer[19] = msg_buf[35];

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
						printString2(password);
						send_serialother(SEND_CONFIG2, &write_serial_buffer[0]);
						usleep(500);
						i = WriteParams("param.conf", &ps, &password[0], errmsg);
						printString2("update config file");
						break;

					case GET_CONFIG2:
						printString2("get config2");
						send_serialother(GET_CONFIG2,&write_serial_buffer[0]);
						break;

					case GET_VERSION:
						printString2(version);
						send_status_msg(version);
						break;

					case SVR_CMD:
						break;

					case HOME_SVR_ON:
						strcpy(tempx,"HOME_SVR_ON");
						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, HOME_SVR_ON);
						printString2("Home Server On");
						break;

					case HOME_SVR_OFF:
						strcpy(tempx,"HOME_SVR_OFF");
						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, HOME_SVR_OFF);
						printString2("Home Server Off");
						break;

					case SERVER_UP:
						send_serialother(SERVER_UP,(UCHAR *)tempx);
						break;

					case EXIT_PROGRAM:

//printf("exiting program...\r\n");
exit_program:

						j = 0;
//							printf("exit code: %d\r\n",reboot_on_exit);
						// return codes that tell try_sched.sh what to do
						if(reboot_on_exit == 1)
						{
							myprintf1("exit to shell\0");
//								printf("exit to shell\r\n");
						}
						else if(reboot_on_exit == 2)
						{
							myprintf1("rebooting...\0");
//								printf("rebooting...\r\n");
						}
						else if(reboot_on_exit == 3)
						{
							myprintf1("shutting down...\0");
							printf("shutting down...\r\n");
						}
						else if(reboot_on_exit == 4)
						{
							myprintf1("upload new...\0");
							printf("upload new...\r\n");
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
							printString2(errmsg);
							myprintf1(errmsg);
						}
	/*
						printf("%d\r\n",ps.rpm_update_rate);
						printf("%d\r\n",ps.mph_update_rate);
						printf("%d\r\n",ps.high_rev_limit);
						printf("%d\r\n",ps.low_rev_limit);
						printf("%d\r\n",ps.cooling_fan_on);
						printf("%d\r\n",ps.cooling_fan_off);
						printf("%d\r\n",ps.blower_enabled);
						printf("%d\r\n",ps.blower1_on);
						printf("%d\r\n",ps.blower2_on);
						printf("%d\r\n",ps.blower3_on);
	*/
	//						close_tcp();

	//						strcpy(tempx,"sched.log\0");
	//						WriteOdometer(tempx, &odometer, errmsg);
	/*
						logfile_handle = open((const char *)&tempx[0], O_RDWR,
								S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

						if(logfile_handle < 0)
						{
							printf("handle: %d\n",logfile_handle);
							return logfile_handle;
						}
						write(logfile_handle,&odometer,sizeof(int));
						write(logfile_handle,&trip,sizeof(int));
						close(logfile_handle);
	*/

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
		printString2("get_msg error\0");
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
	ps.rpm_update_rate = temp_conv;

	temp_conv = (int)msg_buf[20];
	temp_conv <<= 8;
	temp_conv |= (int)msg_buf[21];
	ps.mph_update_rate = temp_conv;
	
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

	sprintf(tempx,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s\0",
														ps.rpm_update_rate,
														ps.mph_update_rate,
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
														password);
	send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SEND_CONFIG);
//	printString2(tempx);
}
/*********************************************************************/
void send_status_msg(char *msg)
{
	send_msg(strlen((char*)msg)*2,(UCHAR*)msg, SEND_STATUS);
}
