#if 1
//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//

#include <avr/io.h>
#include "/home/dan/dev/Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "macros.h"
//#include "spi.h"
#include <string.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
//#include "pinDefines.h"

#define LED_PORT PORTB
#define LED_DDR DDRB
#define LEN 100
//int channels[8][LEN];
char test_string[] = "what the fuck?\0";
#endif

int main(void)
{
	int i,j,k;
	UCHAR xbyte;
	float frpm;
	float ft;
	float tfloat;
	int milisec, microsec;
	int mydelay;
	int irpm;

	LED_DDR = 0xff;
//	initUSART();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_us(10);

	xbyte = 0x21;

	frpm = 5000;	
	j = 0;
	
	while (1)
	{
		ft = (1/frpm*60);
		milisec = (int)(ft * 1000);
//		printf("%d  ",milisec);
		tfloat = (float)milisec;
//		printf("%.3f  ",tfloat*1000);
		tfloat = (ft*1000) - tfloat;
		tfloat *= 1000;
		microsec = (int)tfloat;
		milisec = (int)(ft*1000.0);
//		printf("rpm: %.1f   us: %.3f   ms: %.5f %d  %d\n",frpm, tfloat, ft*1000.0, microsec, milisec);

/*
		printString("rpm:  ");
		irpm = (int)frpm;
		printWord(irpm);
		printString("   mili: ");
		printWord(milisec);
		printString("   micro: ");
		printWord(microsec);
//		transmitByte(0xFF);
		printString("\r\n");
*/
		mydelay = 1000/milisec;
//		mydelay = 10;
//		printWord(mydelay);

//		if(microsec > 500)
//			milisec++;

		for(k = 0;k < mydelay;k++)
		{
			LED_PORT = 0x21;
			for(i = 0;i < milisec;i++)
				_delay_us(500);
			for(i = 0;i < microsec/2;i++)
				_delay_us(1);	

			LED_PORT = 0;
			for(i = 0;i < milisec;i++)
				_delay_us(500);
			for(i = 0;i < microsec/2;i++)
				_delay_us(1);	
		}
/*
		if(frpm > 5800)
		{
			i = frpm = 500;
			j = 0;
		}

		if(frpm == 500 && j < 8)
			j++;
		else
			if(frpm > 4000)
				frpm += 20;
			else
				frpm += 70;
*/
		frpm -= 20;
		if(frpm < 500)
		{
			i = frpm = 5000;
			j = 0;
		}
		
	}
	return (0);									  // this should never happen
}
