/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA USART interrupt driven driver example source.
 *
 *      This file contains an example application that demonstrates the
 *      interrupt driven USART driver. The code example sends three bytes, waits
 *      for three bytes to be received and tests if the received data equals the
 *      sent data.
 *
 * \par Application note:
 *      AVR1307: Using the XMEGA USART
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1694 $
 * $Date: 2008-07-29 14:21:58 +0200 (ti, 29 jul 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include "usart_driver.h"
#include "TC_driver.h"
#include "avr_compiler.h"

uint32_t dec2bcd_r(uint16_t dec);
uint32_t dec2bcd(uint16_t dec);
#define USART1 USARTC0
#define USART2 USARTD0
//#define USART3 USARTE0
void printHexByte(uint8_t byte);
char nibbleToHexCharacter(uint8_t nibble);
void process_digits(uint16_t val, int uart);
void sendChar(UCHAR ch, int which);

#define LED_CLRDISP		0x76
#define LED_DECIMAL		0X77
#define LED_CURSOR		0x79
#define LED_BRIGHT		0x7A
#define LED_BAUDRATE	0x7F
#define LED_RESET		0x81
#define LED_ZEPPLIN		0xFF

USART_data_t USART_data1;
USART_data_t USART_data2;

static UCHAR out_ch;
static uint16_t rpm, mph, rpm2, mph2;
static int digit;
static int done_rpm, done_mph, done_cmd;
static UCHAR cmd, cmd2;

/*********************************************************************************************/
int main(void)
{
	uint8_t i;
	uint8_t ch;
	uint8_t j;
	uint8_t k;
	UCHAR bright;
	UCHAR colon;
//	uint16_t rpm3, mph3;

	out_ch = 0x21;
	digit = 0;

	PORTC.DIRSET   = PIN3_bm;
	PORTC.DIRCLR   = PIN2_bm;
	PORTD.DIRSET   = PIN3_bm;
	PORTD.DIRCLR   = PIN2_bm;

	PORTE.DIRSET = 2;

	USART_InterruptDriver_Initialize(&USART_data1, &USART1, USART_DREINTLVL_LO_gc);
	USART_InterruptDriver_Initialize(&USART_data2, &USART2, USART_DREINTLVL_LO_gc);

	USART_Format_Set(USART_data1.usart, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);
	USART_Format_Set(USART_data2.usart, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);

	USART_RxdInterruptLevel_Set(USART_data1.usart, USART_RXCINTLVL_LO_gc);
	USART_RxdInterruptLevel_Set(USART_data2.usart, USART_RXCINTLVL_LO_gc);

/* Set Baudrate to 19200 bps:
 * Use the default I/O clock frequency that is 2 MHz.
 * Do not use the baudrate scale factor
 *
 * Baudrate select = (1/(16*(((I/O clock frequency)/Baudrate)-1)
 * USART_Baudrate_Set(&USART1, 12 , 0);	(for 9600)
 */
	USART_Baudrate_Set(&USART1, 705 , -7);	// 19200 bps
	USART_Baudrate_Set(&USART2, 705 , -7);

/* Enable both RX and TX. */
	USART_Rx_Enable(USART_data1.usart);
	USART_Tx_Enable(USART_data1.usart);

	USART_Rx_Enable(USART_data2.usart);
	USART_Tx_Enable(USART_data2.usart);

	/* Set period/TOP value. */
//	TC_SetPeriod( &TCC0, 61 );		// 1/32 second
//	TC_SetPeriod( &TCC0, 122 );		// 1/16 second
//	TC_SetPeriod( &TCC0, 244 );		// 1/8 second
	TC_SetPeriod( &TCC0, 488 );		// 1/4 second
//	TC_SetPeriod( &TCC0, 976 );		// 1/2 second
//	TC_SetPeriod( &TCC0, 1953 );	// one second
//	TC_SetPeriod( &TCC0, 3906 );	// two seconds

	/* Select clock source. */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1024_gc );
	TC0_ConfigWGM(&TCC0, TC_WGMODE_NORMAL_gc);
	TC0_SetOverflowIntLevel(&TCC0, TC_OVFINTLVL_HI_gc);

/* Enable PMIC interrupt level low. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;
	PMIC.CTRL |= PMIC_HILVLEN_bm;

/* Enable global interrupts. */
	sei();

	i = 0;

	rpm = 6010;
	mph = 10;
	j = 0;
	bright = 100;
	colon = 1;
	k = 0;
	done_rpm = 0;
	done_mph = 0;
	done_cmd = 0;
	cmd = 0xFF;
	cmd2 = cmd;

	sendChar(LED_CLRDISP,1);
	sendChar(LED_CLRDISP,0);
	sendChar(LED_CURSOR,1);
	sendChar(0,1);
	sendChar(LED_CURSOR,0);
	sendChar(0,0);
	_delay_ms(1);
	sendChar(LED_BRIGHT,1);
	_delay_ms(1);
	sendChar(bright,1);
	_delay_ms(1);
	sendChar(LED_BRIGHT,0);
	_delay_ms(1);
	sendChar(bright,0);
	_delay_ms(1);

//	rpm3 = 500;
//	mph3 = 10;
	while(1)
	{
		_delay_ms(5000);
/*
		if(++rpm3 > 5600)
			rpm3 = 500;
		if(++mph3 > 160)
			mph3 = 10;	
		process_digits(rpm3,1);
		process_digits(mph3,0);
*/
	}
}

/*********************************************************************************************/
ISR(USARTC0_RXC_vect)	// USART1 is the data/cmd channel for the rpm LED
{
	UCHAR ch;
	uint16_t temp;
	USART_RXComplete(&USART_data1);
	ch = USART_RXBuffer_GetByte(&USART_data1);

	if(ch == 0xFF)
	{
		digit = 0;
		PORTE_OUTTGL = 2;
		rpm = 0;
		mph = 0;
	}
	else
	{
		switch(digit)
		{
			case 0:
				digit = 1;
				rpm = (uint16_t)ch;
				break;
			case 1:
				digit = 2;
				temp = (uint16_t)ch;
				temp <<= 8;
				rpm |= temp;
				if(rpm != rpm2)
				{
					rpm2 = rpm;
					done_rpm = 1;
					rpm = 0;
				}
				break;
			case 2:	
				digit = 3;
				mph = (uint16_t)ch;
				break;
			case 3:
				digit = 4;
				temp = (uint16_t)ch;
				temp <<= 8;
				mph |= temp;
				if(mph != mph2)
				{
					mph2 = mph;
					done_mph = 1;
					mph = 0;
				}
				break;
			case 4:
				digit = 5;
				cmd = ch;
				if(cmd != cmd2)
				{
					cmd2 = cmd;
					done_cmd = 1;
				}
				break;
		}
	}
}

/*********************************************************************************************/
ISR(USARTD0_RXC_vect)	// USART2
{
	UCHAR ch;
	USART_RXComplete(&USART_data2);
	ch = USART_RXBuffer_GetByte(&USART_data2);
//	USART_TXBuffer_PutByte(&USART_data2, ch);
}

/*********************************************************************************************/
ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(&USART_data1);
}

/*********************************************************************************************/
ISR(USARTD0_DRE_vect)
{
	USART_DataRegEmpty(&USART_data2);
}

/*********************************************************************************************/
ISR(TCC0_OVF_vect)
{
	if(done_rpm == 1)
	{
		PORTE.OUTTGL = 2;
		process_digits(rpm2,1);
		done_rpm = 0;
	}
	if(done_mph == 1)
	{
		process_digits(mph2,0);
		done_mph = 0;
	}
	if(done_cmd == 1)
	{
		sendChar(LED_BRIGHT,1);
		_delay_ms(1);
		sendChar(cmd,1);
		_delay_ms(1);
		sendChar(LED_BRIGHT,0);
		_delay_ms(1);
		sendChar(cmd,0);
		_delay_ms(1);
		done_cmd = 0;
	}
}
/*********************************************************************************************/
void sendChar(UCHAR ch, int which)
{
	if(which)
	{
		if(USART_IsTXDataRegisterEmpty(&USART2))
			USART_PutChar(&USART2,ch);
	}else
	{
		if(USART_IsTXDataRegisterEmpty(&USART1))
			USART_PutChar(&USART1,ch);
	}
	_delay_ms(3);
}

/*********************************************************************************************/
void process_digits(uint16_t val, int uart)
{
	UCHAR buff[10];
	int i;
	uint16_t utemp;
	UCHAR temp;
	int digit_ptr;
	uint32_t res;
	int digit_len;

	digit_ptr = 0;
	res = dec2bcd(val);
	utemp = (uint16_t)res;

	temp = (UCHAR)(utemp & 0x000f);
	buff[digit_ptr++] = temp;

	utemp >>= 4;
	temp = (UCHAR)(utemp & 0x000f);
	buff[digit_ptr++] = temp;

	utemp >>= 4;
	temp = (UCHAR)(utemp & 0x000f);
	buff[digit_ptr++] = temp;

	utemp >>= 4;
	temp = (UCHAR)(utemp & 0x000f);
	buff[digit_ptr] = temp;

	sendChar(LED_CLRDISP,uart);

	if(val < 1000 && val > 99)
	{
		sendChar(LED_CURSOR,uart);
		sendChar(1,uart);
		digit_len = 2;
	}
	else if(val < 100)
	{
		sendChar(LED_CURSOR,uart);
		sendChar(2,uart);
		digit_len = 1;
	}
	else if(val < 10)
	{
		sendChar(LED_CURSOR,uart);
		sendChar(3,uart);
		digit_len = 0;
	}
	else
	{
		sendChar(LED_CURSOR,uart);
		sendChar(0,uart);
		digit_len = 3;
	}

	for(i = digit_len;i >= 0;i--)
	{
		sendChar(buff[i],uart);
	}
	sendChar(LED_CURSOR,uart);
	sendChar(0,uart);
}

/*********************************************************************************************/
char nibbleToHexCharacter(UCHAR nibble)
{
/* Converts 4 bits into hexadecimal */
	if (nibble < 10)
	{
		return ('0' + nibble);
	}
	else
	{
		return ('A' + nibble - 10);
	}
}

/*********************************************************************************************/
void printHexByte(UCHAR byte)
{
	UCHAR nibble;
	nibble = (byte & 0b11110000) >> 4;
	if(USART_IsTXDataRegisterEmpty(&USART2))
		USART_PutChar(&USART2, nibbleToHexCharacter(nibble));

	nibble = byte & 0b00001111;
	if(USART_IsTXDataRegisterEmpty(&USART2))
		USART_PutChar(&USART2, nibbleToHexCharacter(nibble));
}

/*********************************************************************************************/
uint32_t dec2bcd(uint16_t dec)
{
    uint32_t result = 0;
    int shift = 0;

    while (dec)
    {
        result +=  (dec % 10) << shift;
        dec = dec / 10;
        shift += 4;
    }
    return result;
}

// does the same as above
/*********************************************************************************************/
uint32_t dec2bcd_r(uint16_t dec)
{
    return (dec) ? ((dec2bcd_r( dec / 10 ) << 4) + (dec % 10)) : 0;
}

