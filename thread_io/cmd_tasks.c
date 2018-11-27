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

extern pthread_mutex_t     tcp_read_lock;
extern pthread_mutex_t     tcp_write_lock;

#define TOGGLE_OTP otp->onoff = (otp->onoff == 1?0:1)

CMD_STRUCT cmd_array[58] =
{
	{   	NON_CMD,"NON_CMD\0" },
	{   	ENABLE_START,"ENABLE_START\0" },
	{   	STARTER_OFF,"STARTER_OFF\0" },
	{   	ON_ACC,"ON_ACC\0" },
	{   	OFF_ACC,"OFF_ACC\0" },
	{   	ON_FUEL_PUMP,"ON_FUEL_PUMP\0" },
	{   	OFF_FUEL_PUMP,"OFF_FUEL_PUMP\0" },
	{   	ON_FAN,"ON_FAN\0" },
	{   	OFF_FAN,"OFF_FAN\0" },
	{   	ON_LIGHTS,"ON_LIGHTS\0" },
	{   	OFF_LIGHTS,"OFF_LIGHTS\0" },
	{   	ON_BRIGHTS,"ON_BRIGHTS\0" },
	{   	OFF_BRIGHTS,"OFF_BRIGHTS\0" },
	{   	ON_LLIGHTS,"ON_LLIGHTS\0" },
	{   	OFF_LLIGHTS,"OFF_LLIGHTS\0" },
	{   	ON_LBRIGHTS,"ON_LBRIGHTS\0" },
	{   	OFF_LBRIGHTS,"OFF_LBRIGHTS\0" },
	{   	ON_RLIGHTS,"ON_RLIGHTS\0" },
	{   	OFF_RLIGHTS,"OFF_RLIGHTS\0" },
	{   	ON_RBRIGHTS,"ON_RBRIGHTS\0" },
	{   	OFF_RBRIGHTS,"OFF_RBRIGHTS\0" },
	{		ON_BRAKES,"ON_BRAKES\0" },
	{		OFF_BRAKES,"OFF_BRAKES\0" },
	{		ON_RUNNING_LIGHTS,"ON_RUNNING_LIGHTS\0" },
	{		OFF_RUNNING_LIGHTS,"OFF_RUNNING_LIGHTS\0" },
	{   	SPECIAL_CMD,"SPECIAL_CMD\0" },
	{   	START_SEQ,"START_SEQ\0" },
	{   	SHUTDOWN,"SHUTDOWN\0" },
	{   	SHUTDOWN_IOBOX,"SHUTDOWN_IOBOX\0" },
	{   	REBOOT_IOBOX,"REBOOT_IOBOX\0" },
	{   	SEND_ODATA,"SEND_ODATA\0" },
	{   	SAVE_TO_DISK,"SAVE_TO_DISK\0" },
	{		BLOWER_OFF,"BLOWER_OFF\0" },
	{		BLOWER1,"BLOWER1\0" },
	{		BLOWER2,"BLOWER2\0" },
	{		BLOWER3,"BLOWER3\0" },
	{   	GET_DIR,"GET_DIR\0" },
	{   	LCD_SHIFT_RIGHT,"LCD_SHIFT_RIGHT\0" },
	{   	LCD_SHIFT_LEFT,"LCD_SHIFT_LEFT\0" },
	{   	SCROLL_UP,"SCROLL_UP\0" },
	{   	SCROLL_DOWN,"SCROLL_DOWN\0" },
	{   	ENABLE_LCD,"ENABLE_LCD\0" },
	{   	SET_TIME,"SET_TIME\0" },
	{   	GET_TIME,"GET_TIME\0" },
	{   	UPLOAD_NEW,"UPLOAD_NEW\0" },
	{		NEW_PASSWORD1,"NEW_PASSWORD1\0" },
	{		SET_SERIAL_RECV_ON,"SET_SERIAL_RECV_ON\0" },
	{		SET_SERIAL_RECV_OFF,"SET_SERIAL_RECV_OFF\0" },
	{		TEST_LEFT_BLINKER,"TEST_LEFT_BLINKER\0" },
	{		TEST_RIGHT_BLINKER,"TEST_RIGHT_BLINKER\0" },
	{		RE_ENTER_PASSWORD,"RE_ENTER_PASSWORD\0" },
	{		DISCONNECT,"DISCONNECT\0" },
	{		STOP_MBOX_RECV,"STOP_MBOX_RECV\0" },
	{		CLOSE_DB,"CLOSE_DB\0" },
	{		OPEN_DB,"OPEN_DB\0" },
	{		BAD_MSG,"BAD_MSG\0" },
	{		CURRENT_TIME,"CURRENT_TIME\0" },
	{   	EXIT_PROGRAM,"EXIT_PROGRAM\0" }
};

//extern illist_t ill;
extern ollist_t oll;

UCHAR msg_buf[1000];

int shutdown_all;
int live_window_on;
static UCHAR pre_preamble[] = {0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00};

//extern int tcp_window_on;
//int serial_recv_on;
int time_set;

// array of structs to list all the inputs that have outputs assigned
// to them 

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
	char tempx[100];
	UCHAR tempy[30];
	char temp_time[5];
	UCHAR utemp1;
	UCHAR utemp2;
	char *pch;
	int fname_index;
	UCHAR uch_fname_index;
	UCHAR mask;
	time_t curtime2;
	time_t *pcurtime2 = &curtime2;
	tcp_window_on = 0;	
	int fp;
	off_t fsize;
	int cur_fsize;
	struct timeval mtv;
	struct tm t;
	struct tm *pt = &t;
	int msg_len;
	serial_recv_on = 1;
	time_set = 0;
	shutdown_all = 0;

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
/*
	for(i = 0;i < 20;i++)
	{
		ollist_find_data(i,&otp,&oll);
		printf("%d %d %d %d %d %s\r\n",otp->port, otp->onoff, otp->input_port, 
				otp->type, otp->time_delay, otp->label);
	}
*/
	same_msg = 0;
	lcd_init();

// flash green and red led's to signal we are up (if LCD screen not attached)

	for(i = 0;i < 10;i++)
	{
		red_led(1);
		usleep(30000);
		red_led(0);
		green_led(1);
		usleep(30000);
		green_led(0);
		red_led(1);
		usleep(30000);
		red_led(0);
		green_led(1);
		usleep(30000);
		green_led(0);
	}

//	myprintf1("start....\0");

	myprintf1("sched v1.20\0");
//	printf("sched v1.19\r\n");
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
// 			rc = recv_tcp(&cmd,1,1);			  // blocking
			memset(msg_buf,0,sizeof(msg_buf));
			msg_len = get_msg();
			if(msg_len < 0)
			{
//				printf("bad msg\r\n");
				cmd = BAD_MSG;
			}else
			{
				rc = recv_tcp(&msg_buf[0],msg_len,1);
				cmd = msg_buf[0];
			}
			tcp_connected_time = 0;
			if(cmd != LCD_SHIFT_RIGHT && cmd != LCD_SHIFT_LEFT && cmd != SCROLL_DOWN && cmd != SCROLL_UP
					&& cmd != GET_TIME && cmd != SET_TIME && cmd > 0)
				myprintf2(cmd_array[cmd].cmd_str,cmd);

//			if(cmd > 0)
//				printf("cmd: %d %s\r\n",cmd,cmd_array[cmd].cmd_str);

			if(rc > 0 && cmd > 0)
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
					// START_SEQ turns on acc, fp, fan and enables starter button
					case START_SEQ:
					// SHUTDOWN shuts off the starter enable, fp, acc and fan
					case SHUTDOWN:
					case SHUTDOWN_IOBOX:
					case REBOOT_IOBOX:
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
						basic_controls(cmd);
						strcpy(tempx,cmd_array[cmd].cmd_str);
//						sprintf(tempx,"%d %d %d %d ",cmd,trunning_seconds,trunning_minutes,trunning_hours);
						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SEND_MSG);
						break;

					// the next 2 turns on or off the serial port to the PIC24 (monster box)
					// so if the monster box is switched off for maintenance, the IO box
					// won't get false signals to turn on or off ports
					case SET_SERIAL_RECV_ON:
//						send_live_code("cmd);
						if(fd = init_serial() < 0)
						{
							myprintf1("can't open comm port 1\0");
							return 0;
						}
						serial_recv_on = 1;
						break;

					case SET_SERIAL_RECV_OFF:
//						send_live_code(cmd);
						close_serial();
						serial_recv_on = 0;
						break;

					// allows password to be changed from laptop via tcp connection
					case NEW_PASSWORD1:
						rc = 0;
						memset(tempx,0,50);
//						rc += recv_tcp((UCHAR *)&tempx[0],12,1);
						memcpy((void *)&tempx[0],&msg_buf[0],12);
						myprintf2("read: ",rc);

						send_serialother(NEW_PASSWORD2,tempx[0],
							tempx[1],tempx[2],tempx[3]);
						send_serialother(NEW_PASSWORD3,tempx[4],
							tempx[5],tempx[6],tempx[7]);
						send_serialother(NEW_PASSWORD4,tempx[8],
							tempx[9],tempx[10],tempx[11]);
//						for(i = 0;i < 12;i++)
//							myprintf2("tempx: ",tempx[i]);
//						myprintf1("done");
						break;

					// clear the lcd screen and redraw the menus and rt labels
					// signal to monster box that the password must be re-entered in case of
					// getting out of vehicle (hijack prevention)
					case RE_ENTER_PASSWORD:
						send_serialother(RE_ENTER_PASSWORD1,0,0,0,0);
						break;

					case SET_TIME:
						curtime2 = 0L;
						j = 0;
						memset(tempx,0,sizeof(tempx));

						for(i = 2;i < msg_len;i+=2)
							memcpy((void*)&tempx[j++],(char*)&msg_buf[i],1);
						msg_buf[msg_len/2] = 0;
//							if(msg_buf[i] > 0x1f && msg_buf[i] < 0x7e)
//								printf("%c",msg_buf[i]);
//						printf("%s\r\n",tempx);
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
						time_set = 1;
						break;

					case GET_TIME:
						gettimeofday(&mtv, NULL);
						curtime2 = mtv.tv_sec;
						strftime(tempx,30,"%m-%d-%Y %T\0",localtime(&curtime2));
						myprintf1(tempx);
						send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx,GET_TIME2);
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
/*
					case SEND_ALL_ODATA:
						rc = 0;
						otp = &tempo1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)otp,sizeof(O_DATA),1);
							ollist_insert_data(i,&oll,otp);
						}
//						myprintf1("done\0");
						break;

					// get all the data from the laptop
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
						shutdown_all = 1;
						break;

					case DISCONNECT:
						if(test_sock() > 0)
						{
							close_tcp();
//							printf("disconnected from socket\r\n");
							live_window_on = 0;
							tcp_connected_time = 0;
						}
						break;

					case STOP_MBOX_RECV:
						send_serialother(STOP_SERIAL_RECV,0,0,0,0);
						break;

					// upload this program and then goto reboot so it comes up using the
					// newly uploaded program (see try_sched.sh in /home/dan/dev/sched)
					case UPLOAD_NEW:
						recv_tcp((UCHAR*)&fsize,8,1);
						cur_fsize = (int)fsize;
						memset(upload_buf,0,UPLOAD_BUFF_SIZE);
						strcpy(filename,"sched2");
						fp = open((const char *)filename, O_RDWR | O_CREAT | O_TRUNC, 700);
						if(fp < 0)
						{
//							printf("could not create file: %s\n",filename);
							goto exit_program;
							break;
						}
						do {
							rc = 0;
							rc1 = 0;
							rc += recv_tcp((UCHAR *)&upload_buf[0],UPLOAD_BUFF_SIZE,1);
							rc1 += write(fp, upload_buf, rc);
							cur_fsize -= rc;

						}while(cur_fsize > UPLOAD_BUFF_SIZE);

						rc += recv_tcp((UCHAR *)&upload_buf[0],cur_fsize,1);
						rc1 += write(fp,upload_buf,cur_fsize);

						close(fp);

					case EXIT_PROGRAM:
exit_program:
						if(cmd == UPLOAD_NEW)
						{
							reboot_on_exit = 1;
							myprintf1("rebooting...\0");
						}
						else
						{
//							printf("exit program\r\n");
							send_serialother(STOP_SERIAL_RECV,0,0,0,0);
							recv_tcp((UCHAR*)&reboot_on_exit,1,1);
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
//								printf("shutting down...\r\n");
							}
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

						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,otpp,&oll);
//							printf("%d %d\r\n",otp->port,otp->onoff);
							usleep(1000);
							change_output(otp->port,otp->onoff);
							change_output(otp->port,0);
							usleep(1000);
							ollist_insert_data(i,&oll,otp);
						}

						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);

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
/*
						for(i = 0;i < 20;i++)
						{
							red_led(1);
							usleep(20000);
							red_led(0);
							green_led(1);
							usleep(20000);
							green_led(0);
						}
*/
						shutdown_all = 1;
						return 0;
						break;
				}								  // end of switch
			}									  // if rc > 0
		}else									  // if test_sock
		{
			uSleep(1,1000);
			if(shutdown_all == 1)
				return 0;
		}
	}
	return test + 1;
}
/*********************************************************************/
// get preamble & msg len from client
// preamble is: {0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00,
//	 msg_len(lowbyte),msg_len(highbyte),0x00,0x00,0x00,0x00,0x00,0x00}
int get_msg(void)
{
	int len;
	int ret;
	int i;

	UCHAR preamble[20];
	ret = recv_tcp(preamble,16,1);
//	for(i = 0;i < 15;i++)
//		printf("%02x ",preamble[i]);
//	printf("\n");
	if(memcmp(preamble,pre_preamble,8) != 0)
		return -1;
	len = (int)preamble[8];
//	printf("len: %d\n",len);
	return len;
}

/*********************************************************************/
void send_msg(int msg_len, UCHAR *msg, UCHAR msg_type)
{
	int len;
	int ret;
	int i;

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

