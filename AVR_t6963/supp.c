#include "sfr_helper.h"
#ifndef TEST_WRITE_DATA
#include <avr/io.h>
#include "avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#else
#include <ncurses.h>
//#warning "TEST_WRITE_DATA defined"
#endif
#include "main.h"
#ifdef SIM_AVR
#include "avr_main.h"
#endif
#ifdef SIM_PIC
#include "pic_main.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef TEST_WRITE_DATA
extern char eepromString[EEPROM_SIZE] EEMEM;
#endif

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int pack(UCHAR low_byte, UCHAR high_byte)
{
	int temp;
	int myint;
	low_byte = ~low_byte;
	myint = (int)low_byte;
	temp = (int)high_byte;
	temp <<= 8;
	myint |= temp;
	return myint;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte)
{
	*low_byte = (UCHAR)myint;
	myint >>= 8;
	*high_byte = (UCHAR)myint;
	*low_byte = ~(*low_byte);
}


//******************************************************************************************//
//*********************************** get_label_offsets ************************************//
//******************************************************************************************//
void get_label_offsets(void)
{
	int i, j;
	char *ch;
	char temp_label[MAX_LABEL_LEN];

	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		label_offsets[i] = goffset;
		j = 0;
#ifdef TEST_WRITE_DATA
		memcpy(temp_label,eeprom_sim+goffset,MAX_LABEL_LEN);
#else
		eeprom_read_block(temp_label, eepromString+goffset, MAX_LABEL_LEN);
#endif
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
char *get_label(int index, char *str)
{

		// void *dest, const void *src, size_t n
#ifdef TEST_WRITE_DATA
	memcpy(str,eeprom_sim+label_offsets[index],MAX_LABEL_LEN);
#else
	eeprom_read_block((void *)str,eepromString+label_offsets[index],MAX_LABEL_LEN);
#endif
#ifdef SCREEN_EN
	printString(str);
	printString("\r\n");
#endif
	return str;
}

