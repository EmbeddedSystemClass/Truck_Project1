#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
#include "mytypes.h"
#include "lcd_func.h"

#ifndef TS_7800

#define BUF_SIZE 1000;
static int line;
static int col;
//static char buffer[BUF_SIZE];
//static char *buf_ptr;
/*********************************************************************************************************/
void lcd_init(void)
{
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

	printf("offset = %d\n",offset);
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
	lcd_cmd(HOME);
}
/*********************************************************************************************************/
void lcd_cls(void)
{
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
	col = col + strlen(str);
//	printf("col: %d\n",col);
	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	ptr = temp;
	lcd_write((UCHAR *)ptr);
	col += 2;
}

/**********************************************************************************************************/
void myprintf2(char *str, int x)
{
	char *ptr = str;
	char temp[4];
	int i;

	add_col();
	lcd_cmd(HOME+col);
	col += strlen(str);
	printf("col: %d\n",col);

	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	col += strlen(temp);
	lcd_write((UCHAR *)temp);
	sprintf(temp,"%2d",x);
	col += strlen(temp);
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

	add_col();
	lcd_cmd(HOME+col);
	col += strlen(str);
	printf("col: %d\n",col);

	lcd_write((UCHAR *)ptr);
	temp[0] = 0x20;
	temp[1] = 0;
	lcd_write((UCHAR *)temp);
	sprintf(temp,"%2d",x);
	col += strlen(temp);
	ptr = temp;
	lcd_write((UCHAR *)ptr);

	temp[0] = 0x20;
	temp[1] = 0;
	col += strlen(temp);
	lcd_write((UCHAR *)temp);
	sprintf(temp,"%2d",y);
	col += strlen(temp);
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
	lcd_cmd(SHIFTLEFT);
}

/**********************************************************************************************************/
void shift_right(void)
{
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
UINT lcd_wait(void)
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
void lcd_cmd(UINT cmd)
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
void lcd_write(UCHAR *dat)
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
#endif

