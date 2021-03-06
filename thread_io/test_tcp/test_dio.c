#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include "../../mytypes.h"
#include "lcd_func.h"
//#include "../serial_io.h"

#ifndef TS_7800

#define LCD_BUF_LINES 30
#define LCD_BUF_COLS 32
static UCHAR buffer[LCD_BUF_LINES][LCD_BUF_COLS];
static UCHAR *buf_ptr;
static int cur_buf_line;
static int cur_disp_line;
static int cur_buf_size;

/*********************************************************************/
int uSleep(time_t sec, long nanosec)
{
/* Setup timespec */
	struct timespec req;
	req.tv_sec = sec;
	req.tv_nsec = nanosec;

/* Loop until we've slept long enough */
	do
	{
/* Store remainder back on top of the original required time */
		if( 0 != nanosleep( &req, &req ) )
		{
/* If any error other than a signal interrupt occurs, return an error */
			if(errno != EINTR)
			{
				myprintf1("uSleep error\n");
//             return -1;
			}
		}
		else
		{
/* nanosleep succeeded, so exit the loop */
			break;
		}
	} while ( req.tv_sec > 0 || req.tv_nsec > 0 );

	return 0;									  /* Return success */
}



/*********************************************************************************************************/
void lcd_init(void)
{
	lcdinit();
	lcd_wait();
	memset(buffer,0,LCD_BUF_LINES*LCD_BUF_COLS);
	buf_ptr = buffer[0];
	cur_disp_line = cur_buf_line = cur_buf_size = 0;
	lcd_enabled = 1;
}

/*********************************************************************************************************/
void lcd_home(void)
{
	if(!lcd_enabled)
		return;
	lcd_cmd(HOME);
}

/*********************************************************************************************************/
void lcd_cls(void)
{
	if(!lcd_enabled)
		return;
	lcd_cmd(CLEAR);
	lcd_cmd(HOME);
	memset(buffer,0,LCD_BUF_LINES*LCD_BUF_COLS);
	buf_ptr = buffer[0];
	cur_disp_line = cur_buf_line = cur_buf_size = 0;
}
/**********************************************************************************************************/
int myprintf1(char *str)
{
	char *ptr = str;
	char temp[10];
	char temp2[35];

	if(!lcd_enabled)
	{
//		printf("%s\n",str);
		return 1;
	}

	cur_buf_line = cur_buf_size;
//	printf("%d\n",cur_buf_line);
	inc_bufptrs();
	memset(buffer[cur_buf_line],0,LCD_BUF_COLS);
	sprintf((char*)buffer[cur_buf_line],"%d: %s",cur_disp_line,str);
	display_current(0);

	sprintf(temp2,"%d: %s",cur_disp_line,str);
	return 1;
}

/**********************************************************************************************************/
int myprintf2(char *str, int x)
{
	char *ptr = str;
	char temp[35];
	char temp2[10];
	int i;

	if(!lcd_enabled)
	{
//		printf("%s %d\n",str,x);
		return 1;
	}

	sprintf(temp,"%d: %s",cur_disp_line, str);
	sprintf(temp2,"% d",x);
	strcat(temp,temp2);
	cur_buf_line = cur_buf_size;
	inc_bufptrs();
	memset(buffer[cur_buf_line],0,LCD_BUF_COLS);
	strcpy((char *)buffer[cur_buf_line],temp);
	display_current(0);
	return 0;
}

/**********************************************************************************************************/
int myprintf3(char *str, int x, int y)
{
	char *ptr = str;
	char temp[35];
	char temp2[10];
	int i;

	if(!lcd_enabled)
	{
//		printf("%s %d %d\n",str,x,y);
		return 1;
	}

	cur_buf_line = cur_buf_size;
//	printf("%d\n",cur_buf_line);
	inc_bufptrs();
	memset(buffer[cur_buf_line],0,LCD_BUF_COLS);

	sprintf(temp,"%d: %s",cur_disp_line,str);
	sprintf(temp2,"% d",x);
	strcat(temp,temp2);
	sprintf(temp2,"% d",y);
	strcat(temp,temp2);
	strcpy((char *)buffer[cur_buf_line],temp);
	display_current(0);
	return 0;
}

/**********************************************************************************************************/
static void inc_bufptrs(void)
{
	int i;
	if(cur_buf_size > LCD_BUF_LINES-2)
		for(i = 0;i < cur_buf_size;i++)
		{
			memcpy(buffer[i],buffer[i+1],LCD_BUF_COLS);
//			printf("%s\n",buffer[i]);
		}

	cur_buf_size = (cur_buf_size>=LCD_BUF_LINES-1?LCD_BUF_LINES-1:++cur_buf_size);

	if(++cur_buf_line >= LCD_BUF_LINES-1)
	{
		cur_buf_line = LCD_BUF_LINES-1;
	}

	if(++cur_disp_line > LCD_BUF_LINES)
		cur_disp_line = 0;

//	printf("cur_buf_size: %d cur_buf_line %d (inc)\n",cur_buf_size,cur_buf_line);
}

/**********************************************************************************************************/
static void display_current(int special_case)
{
	int i;

	lcd_cmd(CLEAR);
	lcd_cmd(HOME);

//	printf("cur_buf_size: %d cur_buf_line %d\n",cur_buf_size,cur_buf_line);

	if(cur_buf_size == 0)
	{
		lcd_write(buffer[0]);
	}else
	{
		if(special_case == 0)
		{
			lcd_write(buffer[cur_buf_line-1]);
			lcd_wait();
			lcd_cmd(ROW2);
			lcd_wait();
			lcd_write(buffer[cur_buf_line]);
			lcd_wait();
		}
		else if(special_case == 1)
		{
			lcd_write(buffer[cur_buf_size-1]);
			lcd_wait();
			lcd_cmd(ROW2);
			lcd_wait();
			lcd_write(buffer[cur_buf_size]);
			lcd_wait();
		}

	}
//	printf("%s\n",buffer[cur_buf_line]);
}

/*********************************************************************************************************/
void scroll_up(void)
{
	if(cur_buf_size < 3)
		return;

//	printf("line: %d size: %d\n",cur_buf_line,cur_buf_size);

	cur_buf_line--;
	if(cur_buf_line < 1)
		cur_buf_line = cur_buf_size-1;

//	printf("scroll_up: %d\n",cur_buf_line);
	display_current(0);
}

/*********************************************************************************************************/
void scroll_down(void)
{
	if(cur_buf_size < 3)
		return;
//	printf("line: %d size: %d\n",cur_buf_line,cur_buf_size);

	if(++cur_buf_line > cur_buf_size-1)
	{
		display_current(1);
		cur_buf_line = 1;
	}
//	printf("line: %d size: %d\n",cur_buf_line,cur_buf_size);

//	printf("scroll_down: %d\n",cur_buf_line);
	display_current(0);
}

/**********************************************************************************************************/
void shift_left(void)
{
	if(!lcd_enabled)
		return;
	lcd_cmd(SHIFTLEFT);
}


/**********************************************************************************************************/
void shift_right(void)
{
	if(!lcd_enabled)
		return;
	lcd_cmd(SHIFTRIGHT);
}


/**********************************************************************************************************/
static void lcdinit(void)
{
	int fd = open("/dev/mem", O_RDWR);
	int psize = getpagesize();
	int i;

// if compiling with -static we have to use the MAP_FIXED flag
//	gpio = (UINT *)mmap(0, getpagesize(),PROT_READ|PROT_WRITE, MAP_SHARED, fd, LCDBASEADD);
	gpio = (UINT *)mmap(0, getpagesize(),PROT_READ|PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, LCDBASEADD);
	phdr = &gpio[PHDR];
	padr = &gpio[PADR];
	paddr = &gpio[PADDR];
	phddr = &gpio[PHDDR];
	dio_addr = &gpio[DIOADR];
	dio_ddr = &gpio[DIODDR];
	portfb = &gpio[PORTFB];
	portfd = &gpio[PORTFD];
	portled = &gpio[PORTLED];

/*
volatile UINT *gpio;
volatile UINT *phdr;
volatile UINT *phddr;
volatile UINT *padr;
volatile UINT *paddr;
volatile UINT *dio_addr;
volatile UINT *dio_ddr;
volatile UINT *portfb;
volatile UINT *portfd;

#define PADR    0							// address offset of LCD
#define PADDR   (0x10 / sizeof(UINT))		// address offset of DDR LCD
#define PHDR    (0x40 / sizeof(UINT))		// bits 3-5: EN, RS, WR
#define PHDDR   (0x44 / sizeof(UINT))		// DDR for above
#define DIODDR	(0x14 / sizeof(UINT))
#define DIOADR	(0x04 / sizeof(UINT))
#define PORTFB  (0x30 / sizeof(UINT))
#define PORTFD	(0x34 / sizeof(UINT))
*/

//	printf("1:  %4x %4x \n", gpio[PADR],gpio[PADDR]);
	*paddr = 0x0;								  // All of port A to inputs
	*phddr |= 0x38;								  // bits 3:5 of port H to outputs
	*phdr &= ~0x18;								  // de-assert EN, de-assert RS
	usleep(15000);
	lcd_cmd(0x38);								  // two rows, 5x7, 8 bit
	usleep(4100);
	lcd_cmd(0x38);								  // two rows, 5x7, 8 bit
	usleep(100);
	lcd_cmd(0x38);								  // two rows, 5x7, 8 bit
	lcd_cmd(0x6);								  // cursor increment mode
	lcd_wait();
	lcd_cmd(0x1);								  // clear display
	lcd_wait();
	lcd_cmd(0xc);								  // display on, blink off, cursor off
	lcd_wait();
	lcd_cmd(0x2);								  // return home

	for(i = 0;i < 6;i++)							// set the 1st 6 bits of DIO to inputs
		setdioddr(i,0);

	setdioddr(7,1);									// set last one to output
}


/**********************************************************************************************************/
static UINT lcd_wait(void)
{
	int i, dat, tries = 0;
	UINT ctrl = *phdr;

	*paddr = 0x0;								  // set port A to inputs
	ctrl = *phdr;

	do
	{
// step 1, apply RS & WR
		ctrl |= 0x30;							  // de-assert WR
		ctrl &= ~0x10;							  // de-assert RS
		*phdr = ctrl;

// step 2, wait
		i = SETUP;
		COUNTDOWN(i);

// step 3, assert EN
		ctrl |= 0x8;
		*phdr = ctrl;

// step 4, wait
		i = PULSE;
		COUNTDOWN(i);

// step 5, de-assert EN, read result
		dat = *padr;
		ctrl &= ~0x8;							  // de-assert EN
		*phdr = ctrl;

// step 6, wait
		i = HOLD;
		COUNTDOWN(i);
	} while (dat & 0x80 && tries++ < 1000);
	return dat;
}


/**********************************************************************************************************/
static void lcd_cmd(UINT cmd)
{
	int i;
	UINT ctrl = *phdr;

	*paddr = 0xff;								  // set port A to outputs

// step 1, apply RS & WR, send data
	*padr = cmd;
	ctrl &= ~0x30;								  // de-assert RS, assert WR
	*phdr = ctrl;

// step 2, wait
	i = SETUP;
	COUNTDOWN(i);

// step 3, assert EN
	ctrl |= 0x8;
	*phdr = ctrl;

// step 4, wait
	i = PULSE;
	COUNTDOWN(i);

// step 5, de-assert EN
	ctrl &= ~0x8;								  // de-assert EN
	*phdr = ctrl;

// step 6, wait
	i = HOLD;
	COUNTDOWN(i);
}


/**********************************************************************************************************/
static void lcd_write(UCHAR *dat)
{
	int i;
	UINT ctrl = *phdr;

	do
	{
		lcd_wait();
		*paddr = 0xff;							  // set port A to outputs

// step 1, apply RS & WR, send data
		*padr = *dat++;
		ctrl |= 0x10;							  // assert RS
		ctrl &= ~0x20;							  // assert WR
		*phdr = ctrl;

// step 2
		i = SETUP;
		COUNTDOWN(i);

// step 3, assert EN
		ctrl |= 0x8;
		*phdr = ctrl;

// step 4, wait 800 nS
		i = PULSE;
		COUNTDOWN(i);

// step 5, de-assert EN
		ctrl &= ~0x8;							  // de-assert EN
		*phdr = ctrl;

// step 6, wait
		i = HOLD;
		COUNTDOWN(i);
	} while(*dat);
}

void red_led(int onoff)
{
	setbiobit(portled,1,onoff);
}
void green_led(int onoff)
{
	setbiobit(portled,0,onoff);
}


static int setbiobit(UCHAR *ptr,int n,int v)
{
	unsigned char d;

	if (n>7 || n<0 || v>1 || v<0) return(-1);
	d=*ptr;
	d= (d&(~(1<<n))) | (v<<n);
	*ptr=d;
	return(v);
}


static UCHAR dio_get_ddr(void)
{
	return(*dio_ddr);
}


static UCHAR dio_set_ddr(UCHAR b)
{
	*dio_ddr=b;
	return(b);
}


//#if 0
int getdioline(int n)
{
	int d;

	if (n>8 || n<0) return(-1);
	if (n==8) return(((*portfb)>>1)&1);
	return((*dio_addr>>n)&1);
}


/* setdioline(n,v) set DIO Line n to value v
 *                 return v, or -1 on error.
 * This does a read-modify-write sequence, so only the line
 * specified is altered. If the line is not set as output in the DDR
 * then this routine sets it as output
 */

int setdioline(int n,int v)
{
	unsigned char d;

	if (n>8 || n<0 || v>1 || v<0) return(-1);
	if (n==8)
	{
		d=(*portfd)&2;
		if (!d) *portfd|=2;
		d=*portfb & 0xFD;
		*portfb=d|(v<<1);
	}
	else
	{
		d=*dio_ddr;
		if ( ! (d&(1<<n)) )
		{
			d|=(1<<n);
			*dio_ddr=d;
		}
		d=*dio_ddr;
		d= (d&(~(1<<n))) | (v<<n);
		*dio_ddr=d;
	}
	return(v);
}


/* getdioddr(n)  read the DDR of the DIO Line n
 *                return 0 or 1, or -1 on error
 */

static int getdioddr(int n)
{
	int d;

	if (n>8 || n<0) return(-1);
	if (n==8) return((*portfd>>1)&1);
	return((dio_get_ddr()>>n)&1);
}


/* setdioddr(n,v) set DDR for line n to value v
 *                 return v, or -1 on error.
 * This does a read-modify-write sequence, so only the line
 * specified is altered.
 */

static int setdioddr(int n,int v)
{
	unsigned char d;

	if (n>8 || n<0 || v>1 || v<0) return(-1);
	if (n==8)
	{
		dio_set_ddr8(v);
	}
	else
	{
		d=dio_get_ddr();
		d= (d&(~(1<<n))) | (v<<n);
		dio_set_ddr(d);
	}
	return(v);
}


//#endif

#ifndef MAKE_TARGET
/*********************************************************************************************************/
static void mydelay(unsigned long i)
{
	unsigned long j;

	do
	{
		for(j = 0;j < 10000;j++);
		i--;
	}while(i > 0);
}

void print_mem(void)
{
	int i;
/*
	phdr = &gpio[PHDR];
	padr = &gpio[PADR];
	paddr = &gpio[PADDR];
	phddr = &gpio[PHDDR];
	dio_addr = &gpio[DIOADR];
	dio_ddr = &gpio[DIODDR];

#define PADR    0							// address offset of LCD
#define PADDR   (0x10 / sizeof(UINT))		// address offset of DDR LCD
#define PHDR    (0x40 / sizeof(UINT))		// bits 3-5: EN, RS, WR
#define PHDDR   (0x44 / sizeof(UINT))		// DDR for above
#define DIODDR	(0x14 / sizeof(UINT))
#define DIOADR	(0x04 / sizeof(UINT))
#define PORTFB  (0x30 / sizeof(UINT))
#define PORTFD	(0x34 / sizeof(UINT))

*/
	printf("phdr:     %4x %4x\n",phdr,*phdr);	  // 40
	printf("padr:     %4x %4x\n",padr,*padr);	  // 0
	printf("paddr:    %4x %4x\n",paddr,*paddr);	  // 10
	printf("phddr:    %4x %4x\n",phddr,*phddr);	  // 44
// 4
	printf("dio_addr: %4x %4x\n",dio_addr,*dio_addr);
// 14
	printf("dio_ddr:  %4x %4x\n",dio_ddr,*dio_ddr);
	printf("portfb:   %4x %4x\n",portfb,*portfb);  // 30
	printf("portfd:   %4x %4x\n",portfd,*portfd);  // 34
}


int main(void)
{
	int i,j,key;

	i = j = 0;
	lcd_init();

	printf("a - set DIO 0\n");
	printf("b - clear DIO 0\n");
	printf("c - set DIO 1\n");
	printf("d - clear DIO 1\n");
	printf("i - toggle DIO 0\n");
	printf("j - toggle DIO 1\n");
	printf("k - toggle DIO 2\n");
	printf("2 - toggle DIO 0,1,2\n");

printf("try and guess what keys to push...\r\n");
	do
	{
		key = getc(stdin);
		switch(key)
		{
			case 'a':
				setdioline(0,1);
				break;
			case 'b':
				setdioline(0,0);
				break;
			case 'c':
				setdioline(1,1);
				break;
			case 'd':
				setdioline(1,0);
				break;
			case 'e':
				setdioline(0,1);
				break;
			case 'f':
				setdioline(0,0);
				break;
			case 'g':
				setdioline(1,1);
				break;
			case 'h':
				setdioline(1,0);
				break;
			case 'i':
				printf("working...\r\n");
				for(i = 0;i < 500;i++)
				{
					setdioline(0,1);
//					usleep(100);
					uSleep(0,500);
					setdioline(0,0);
					uSleep(0,500);
//					usleep(100);
				}
				setdioline(0,1);
				printf("next key...\r\n");
				break;
			case 'j':
				printf("working...\r\n");
				for(i = 0;i < 100;i++)
				{
					setdioline(1,1);
					usleep(100);
					setdioline(1,0);
					usleep(1000);
				}
				setdioline(1,1);
				printf("next key...\r\n");
				break;
			case 'k':
				printf("working...\r\n");
				for(i = 0;i < 100;i++)
				{
					setdioline(2,1);
					usleep(100);
					setdioline(2,0);
					usleep(100);
				}
				setdioline(2,1);
				printf("next key...\r\n");
				break;
			case '2':
				printf("working...\r\n");
				for(i = 0;i < 10;i++)
				{
					setdioline(0,1);
					usleep(100);
					setdioline(0,0);
					usleep(100);
					setdioline(1,1);
					usleep(100);
					setdioline(1,0);
					usleep(100);
					setdioline(2,1);
					usleep(100);
					setdioline(2,0);
					usleep(100);
				}	
				printf("next key...\r\n");
				setdioline(0,1);
				setdioline(1,1);
				setdioline(2,1);
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7':
				break;
			case '8':
				break;
			case '9':
				break;
			default:
				break;
		}
	}while(key != 'q');
	setdioline(0,1);
	setdioline(1,1);
	setdioline(2,1);
	setdioline(3,1);
	setdioline(4,1);
	setdioline(5,1);
	setdioline(6,1);
	setdioline(7,1);
	printf("\r\n\r\n         done!\r\n\r\n");
}
#endif
#endif
