// sim_read.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <ncurses.h>
#include "../sfr_helper.h"
//#include "../key_defs.h"
#include "../main.h"
#include "../avr_main.h"
#include "../t6963.h"

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
// really cranking
#define TIME_DELAY 20000
// readable
//#define TIME_DELAY 300000

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

UCHAR *peeprom_sim;

//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
#if 1
	int fd;
	int i,j,k;
	UCHAR ch;
	UCHAR key;
	struct termios oldtio,newtio;
	WINDOW *win;
	int res;
	int display_offset = 1;
	char temp_label[MAX_LABEL_LEN];
	peeprom_sim = (UCHAR *)&eeprom_sim;
	void *vptr;
	
#endif
	// reserve an extra sample_data space for in case of 'escape'
	initscr();			/* Start curses mode 		*/
	clear();
	noecho();
//	nodelay(stdscr,TRUE);
	raw();				/* Line buffering disabled	*/
	cbreak();	/* Line buffering disabled. pass on everything */
	win = newwin(60, 65, 0,0);
	keypad(win, TRUE);
//	nodelay(win, TRUE);
	box(win,0,0);
	set_win(win);

	wrefresh(win);

	memset(&newtio, 0, sizeof newtio);

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }
	global_fd = fd;
	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);	// block on read
//	set_blocking (fd, 0);	// non-blocking

#if 0
	for(i = 0;i < EEPROM_SIZE/2;i++)
		eeprom_sim[i] = i;

	for(i = EEPROM_SIZE/2;i < EEPROM_SIZE;i++)
		eeprom_sim[i] = ~i;
#endif

	res = 0;
	for(i = 0;i < EEPROM_SIZE;i++)
	{
		res += read(global_fd,&eeprom_sim[i],1);
		mvwprintw(win, LAST_ROW-1,1,"%2x %d ",eeprom_sim[i],res);
		if(i < NO_RT_LABELS_EEPROM_LOCATION)
			usleep(tdelay*20);
		wrefresh(win);
	}

	for(i = 0;i < TOTAL_NUM_CHECKBOXES;i++)
	{

		res += read(global_fd,&check_boxes[i].index,1);
		res += read(global_fd,&check_boxes[i].checked,1);
		for(j = 0;j < CHECKBOX_STRING_LEN;j++)
			res += read(global_fd,&check_boxes[i].string[j],1);
		mvwprintw(win, LAST_ROW-2,1,"%2d %2d ",i,j);
		wrefresh(win);
	}

	mvwprintw(win, LAST_ROW-1,1,"res: %2d            ",res);
	wrefresh(win);

	j = 0;
	for(i = NO_RT_LABELS_EEPROM_LOCATION;i < NO_MENU_LABELS_EEPROM_LOCATION+2;i++)
	{
		mvwprintw(win, LAST_ROW-2,1+(j*3),"%2x ",eeprom_sim[i],res);
		wrefresh(win);
		j++;
	}

	memcpy((void*)&no_rt_labels,(void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
	memcpy((void*)&no_rtparams,(void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
	memcpy((void*)&no_menu_labels,(void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

	goffset = 0;
	get_label_offsets(win);

//	no_menu_labels = 27;
//	no_rt_labels = 10;

//	mvwprintw(win, LAST_ROW-3,1,"eeprom locations: %2d %2d %2d ",no_rt_labels,no_rtparams,no_menu_labels);
	wrefresh(win);

	j = 0;
	res = 0;

	curr_menus[0] = MENU1A;
	curr_menus[1] = MENU1B;
	curr_menus[2] = MENU1C;
	curr_menus[3] = MENU1D;
	curr_menus[4] = MENU2A;
	curr_menus[5] = MENU2B;

	init_list();
	memset(aux_string,0,AUX_STRING_LEN);
	memset(sync_buf,0,sizeof(sync_buf));
	sync_buf[0] = 0x55;
	sync_buf[1] = 0x55;
	sync_buf[2] = 0x55;
	sync_buf[3] = 0xAA;
	sync_buf[4] = 0xAA;
	sync_buf[5] = 0xAA;

	syncup = 0;
	bp = 0;
	tdelay = 1500;
	while(1)
	{
		read(global_fd,&key,1);
		mvwprintw(win, LAST_ROW-3,1,"key: %x ",key);
		read_get_key(key);
//		parse_sync();
		j++;
		if(j > 200)
			j = 0;
	}
	delwin(win);
	clrtoeol();
	refresh();
	endwin();
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	exit(0);
}

//******************************************************************************************//
//*********************************** set_interface_attribs ********************************//
//******************************************************************************************//
int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
				perror(" ");
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
				perror(" ");
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                perror(" ");
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
//        tty.c_cc[VTIME] = 10;			// 1 second read timeout
//        tty.c_cc[VTIME] = 0.5;
        tty.c_cc[VTIME] = 1;            // 0.1 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}

