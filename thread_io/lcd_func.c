#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "mytypes.h"
#include "lcd_func.h"

#ifndef TS_7800

#define BUF_SIZE 1000;
static int line;
static int col;
//static char buffer[BUF_SIZE];
//static char *buf_ptr;

static void mydelay(unsigned long i)
{
	unsigned long j;

	do
	{
		for(j = 0;j < 10000;j++);
			i--;
	}while(i > 0);
}

/*********************************************************************************************************/
void lcd_init(void)
{
	lcd_enabled = 1;
	lcdinit();
	lcd_wait();
	line = 0;
	col = 0;
//	memset(buffer,0,BUF_SIZE);
//	buf_ptr = buffer;
}
/*********************************************************************************************************/
// row 0 ends at 39
// row 1 starts at 64 and goes to 83 on 1st page
// and starts at 84 and goes to 103 on 2nd page
static void lcd_cursor(int row, int col, int page)
{
	int offset;

	offset = (page*20)+(row*64)+col;

	lcd_cmd(HOME+offset);
}
/*
static add_buf(char *str)
{
	memcpy(buf_ptr,str);
	buf_ptr += strlen(str);
	buf_ptr++;
	buf_ptr = 0;
}
*/
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
	col = line = 0;
}

static void add_col(void)
{
	if(col > 103)
		col = 0;	

	if(col > 39 && col < 64)
	{
		col = 64;
	}
}

/**********************************************************************************************************/
void myprintf1(char *str)
{
	char *ptr = str;
	char temp[2];

	if(!lcd_enabled)
	{
		printf("%s\n",str);
		return;
	}
#if 0
	printf("line: %2d col: %2d ",line,col);
	
	if(col > 63)
	{
		line = (~line) & 0x01;
		col = 0;
	}
	lcd_cursor(line,col,0);
#endif
	add_col();
	lcd_cmd(HOME+col);
	col += (int)strlen(str);
	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	ptr = temp;
	lcd_write((UCHAR *)ptr);
	col++;
}

/**********************************************************************************************************/
void myprintf2(char *str, int x)
{
	char *ptr = str;
	char temp[4];
	int i;

	if(!lcd_enabled)
	{
		printf("%s %d\n",str,x);
		return;
	}

	add_col();
	lcd_cmd(HOME+col);
	col += (int)strlen(str);

	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	col += (int)strlen(temp);
	lcd_write((UCHAR *)temp);
	sprintf(temp,"%2d",x);
	col += (int)strlen(temp);
	ptr = temp;
	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	ptr = temp;
	lcd_write((UCHAR *)ptr);
	col += 2;
}

/**********************************************************************************************************/
void myprintf3(char *str, int x, int y)
{
	char *ptr = str;
	char temp[4];
	int i;

	if(!lcd_enabled)
	{
		printf("%s %d %d\n",str,x,y);
		return;
	}

	add_col();
	lcd_cmd(HOME+col);
	col += (int)strlen(str);

	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	lcd_write((UCHAR *)temp);
	sprintf(temp,"%2d",x);
	col += (int)strlen(temp);
	ptr = temp;
	lcd_write((UCHAR *)ptr);

	temp[0] = 0x20;
	temp[1] = 0;
	col += (int)strlen(temp);
	lcd_write((UCHAR *)temp);
	sprintf(temp,"%2d",y);
	col += (int)strlen(temp);
	ptr = temp;
	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	ptr = temp;
	lcd_write((UCHAR *)ptr);
	col += 2;
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
	printf("phdr:     %4x %4x\n",phdr,*phdr);			// 40
	printf("padr:     %4x %4x\n",padr,*padr);			// 0
	printf("paddr:    %4x %4x\n",paddr,*paddr);			// 10
	printf("phddr:    %4x %4x\n",phddr,*phddr);			// 44
	printf("dio_addr: %4x %4x\n",dio_addr,*dio_addr);	// 4
	printf("dio_ddr:  %4x %4x\n",dio_ddr,*dio_ddr);		// 14
	printf("portfb:   %4x %4x\n",portfb,*portfb);		// 30
	printf("portfd:   %4x %4x\n",portfd,*portfd);		// 34
}

int main(void)
{
	char buf[300];
	static buf_ptr;
	UCHAR test2;
	int i,j,key;

	test2 = 0x21;
	for(i = 0;i < 300;i++)
	{
		buf[i] = test2;
		if(++test2 > 0x7e)
			test2 = 0x21;
	}
	for(i = 0;i < 300;i++)
	{
		if((i % 9) == 0)
			buf[i] = 0;
	}

	lcd_init();

	do
	{
		key = getc(stdin);
		switch(key)
		{
			case 'c':
				lcd_cls();			
			break;
			case 'l':
				for(i = 0;i < 6;i++)
				{
					mydelay(5);
					shift_left();
				}
			break;
			case 'r':
				for(i = 0;i < 6;i++)
				{
					mydelay(5);
					shift_right();
				}
			break;
			case 'm':
				print_mem();
			break;
			case '1':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(0,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '2':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(1,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '3':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(2,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '4':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(3,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '5':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(4,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '6':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(5,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '7':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(6,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;
			case '8':
				for(i = 0;i < 2000;i++)
				{
					if(setdioline(7,i&1) == -1)
						printf("a");
					mydelay(1);
				}
				printf("done\n");
			break;	
			case 'p':
				do
				{
					buf_ptr++;
					if(buf_ptr > 100)
						buf_ptr = 0;
				}while(buf[buf_ptr] != 0);

				buf_ptr++;
				myprintf1(buf+buf_ptr);
				printf("%s\n",buf+buf_ptr);
			break;
			case 'h':
				lcd_home();
			break;
			default:
			break;				
		}		
	}while(key != 'q');	
}
#endif
#endif

