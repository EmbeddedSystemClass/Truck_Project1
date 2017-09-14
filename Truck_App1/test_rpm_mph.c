// .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
//    All rights reserved.
//    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
//    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
//    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
// ***************************************************************************************************************
// adc7scan1.c - Samples 7 channels sequentially with automatic channel scanning (only for PIC24 CPUs without DMA)
// ***************************************************************************************************************
// Performs a basic config of the ADC and samples seven channels sequentially
// with automatic channel scanning.  ADC values are 12-bit results.
// Samples are obtained continuously.  Uses ADC completion interrupts to get
// values from ADCxBUFn registers.  Main routine fetches the "latest" values
// from memory.
//
// use this as a test input for the rpm/mph input on the FPGA
// 2 pots are connected to the 1st 2 ADC channels - one is rpm, the other is mph
// the time delay for each is scaled and shifted so 0->5v results in 800 -> 5000rpm and 25mph -> 90mph
// assuming the rpm sensor is on 1 spark plug and there are 2 magnets opposite each other on the driveshaft

//#include "pic24_all.h"
#include "/home/dan/dev/PIC24_support/lib/include/pic24_all.h"
#include "/home/dan/dev/PIC24_support/lib/include/pic24_adc.h"
#include "/home/dan/dev/PIC24_support/lib/include/pic24_timer.h"
#include "/home/dan/dev/PIC24_support/lib/include/pic24_util.h"
#include "/home/dan/dev/PIC24_support/lib/include/pic24_ports_config.h"
#include "stdio.h"

#ifdef _DMA0IF
# warning "This processor selection has the DMA module; this code example is incompatible with a PIC24 CPU that has DMA."
int main(void)
{
	return 0;
}
#else
//UART1 RX - p68 - 5th from top inside left
//UART1 TX - p72 - 3rd from top inside left
//RA9 (VREF-) - p28 - 2nd from right outside bottom
//RA10 (VREF+) - p29 - 2nd from right inside bottom
//RB0 (ADC0) - p25 - 1st from bottom outside right
//RB1 (ADC1) - p24 - 2nd from bottom inside right
#define CONFIG_LED1() CONFIG_RF3_AS_DIG_OUTPUT()  // pin 51 first from bottom outside left
#define LED1 (_LATF3)
#define CONFIG_LED2() CONFIG_RF2_AS_DIG_OUTPUT()  // pin 52 first from bottom inside left
#define LED2 (_LATF2)

// NET "rpm" LOC = "P69";	inputs on FPGA
// NET "mph" LOC = "P64";

#define TRUE 1
#define FALSE 0
//#define VREF 3.3  //assume Vref = 3.3 volts
#define VREF 5.0
#define NUM_CHANNELS 2

// uncomment the next line to setup this project for a 12-bit ADC
//#define USE_12BIT_ADC

#ifdef  USE_12BIT_ADC
#define   ADC_LEN           12
#define   ADC_NSTEPS        4096
#define   ADC_12BIT_FLAG    1
#else
#define   ADC_LEN           10
#define   ADC_NSTEPS        1024
#define   ADC_12BIT_FLAG    0
#endif

volatile uint16_t au16_buffer[NUM_CHANNELS];
volatile uint8_t  u8_waiting;
volatile uint16_t div1;
volatile uint16_t div2;
volatile uint16_t div1max;
volatile uint16_t div2max;
volatile uint8_t  cmd_array[24] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

void _ISR _ADC1Interrupt (void)
{
	uint8_t         u8_i;
	uint16_t*       au16_adcHWBuff = (uint16_t*) &ADC1BUF0;

	for ( u8_i=0; u8_i<NUM_CHANNELS; u8_i++)
	{
		au16_buffer[u8_i] = au16_adcHWBuff[u8_i];
	}											  //end for()
	u8_waiting = 0;								  // signal main() that data is ready
	_AD1IF = 0;									  //clear the interrupt flag

	if(++div1 > div1max)						  // blue on scope (rpm)
	{
		LED1 = !LED1;
		div1 = 0;
	}
	if(++div2 > div2max)						  // yellow on scope (mph)
	{
		LED2 = !LED2;
		div2 = 0;
	}
// toggle an LED so we can measure how often ADC IRQs are coming in
}


int main (void)
{
	uint8_t   u8_i, i;
	uint16_t  u16_pot;
	float   f_pot;
	uint8_t temp1, temp2;
	uint16_t res, res2;
	uint16_t res3, res4;

//	configBasic(HELLO_MSG);
	configClock();
	configDefaultUART(DEFAULT_BAUDRATE);

// make RA0/AN0/VREF+ a digital input to kill the pullup and
// set the TRISA bit, then make it ANALOG so the ADC will work
	CONFIG_RB0_AS_ANALOG();
	CONFIG_RB1_AS_ANALOG();
/*
	CONFIG_RB2_AS_ANALOG();
	CONFIG_RB3_AS_ANALOG();
	CONFIG_RB14_AS_ANALOG();
	CONFIG_RB13_AS_ANALOG();
	CONFIG_RB12_AS_ANALOG();
*/
	CONFIG_LED1();
	CONFIG_LED2();
	printf("starting program\n");
/*	configADC1_AutoScanIrqCH0( ADC_SCAN_AN0 | ADC_SCAN_AN1 | ADC_SCAN_AN4 |    \
		ADC_SCAN_AN5 | ADC_SCAN_AN10 | ADC_SCAN_AN11 | ADC_SCAN_AN12,
		31, ADC_12BIT_FLAG);
*/
//	div1max = 227; - 10ms cycle
//
//	div1max = 272;	= 12ms
//	div2max = 1725;	= 76ms
	configADC1_AutoScanIrqCH0( ADC_SCAN_AN0 | ADC_SCAN_AN1, 31, ADC_12BIT_FLAG);
	temp1 = 0;
// wait for first conversion to finish before proceeding
	while ( !AD1CON1bits.DONE)
	{
	};
	while (1)
	{
		while (u8_waiting)
		{
		};										  // wait for valid data in ISR
		u8_waiting = 0;
		for ( u8_i=0; u8_i<NUM_CHANNELS; u8_i++)
		{
			u16_pot = au16_buffer[u8_i];
			f_pot = u16_pot;
//			printf("%d = %x %1.2f\n",u8_i,u16_pot,(double)f_pot);
// first channel represents rpm
// 800rpm = 75ms
// 5000rpm = 12ms
// 76ms = 1702
// 12ms = 272
// 1702 - 272 = 1430 (range)
// 272 (shift)
// 1430/1023 = 1.3983 (scale)
// use (ADC result * 1.3983) + 272 as time delay for rpm
// 0 -> 5v should show 272 -> 1702
			if(u8_i == 0)
			{
				f_pot *= 1.3983;
				f_pot += 272.0;
				div1max = (uint16_t)f_pot;
				printf("rpm: %d  \t\t",div1max);
				DELAY_MS(5);					  // this was set to 20ms
			}
// second channel represents mph
// 25mph = 125ms
// 90mph = 35ms
// 125ms = 2837.5
// 35ms = 794.5
// 2837.5 - 794.5 = 2043 (range)
// 794.5 (shift)
// 2043/1023 = 1.997 (scale)
// use (ADC result * 1.997) + 794.5
// 0 -> 5v should show 794 -> 2837
			else
			{
				f_pot *= 1.997;
				f_pot += 794.5;
				div2max = (uint16_t)f_pot;
				printf("mph: %d  \r",div2max);
				DELAY_MS(5);
			}
/*
			f_pot = 3.3 / ADC_NSTEPS * u16_pot;
			f_pot = 5.0 / ADC_NSTEPS * u16_pot;
			printf("r");
			if (u8_i < 10) outChar( '0'+u8_i );
			else outChar( 'A'-10+u8_i );
			printf(":0x%04X=%1.3fV  ",  u16_pot, (double) f_pot );
			if ((u8_i % 4) == 3) printf("\n");
*/
		}										  //end for()
//		printf("\n");
//#if 0
		for(i = 0;i < 24;i++)
		{
			temp1 = cmd_array[i];
			outChar2(temp1);
			DELAY_MS(2);
			temp2 = inChar2();
//			printf(" %2x ",temp2);

			switch(i)
			{
//#if 0
				case 2:
				case 3:
				case 4:
				case 5:
					printf(" ");
				case 6:
				case 7:
				case 8:
				case 9:
					res = res2 = 0;
					printf("%2d ",temp2);
					break;
//#endif
				case 10:
					res = (uint16_t)temp2;
					printf("%2x ",res);
					break;
				case 11:
					res2 = (uint16_t)temp2;
					printf("%2x ",res2);
					res2 <<= 8;
					res2 &= 0xff00;
					res |= res2;
					printf("  %4d ", res);
					break;
				case 12:
					res3 = temp2;
					break;
				case 13:
					res4 = temp2;
					res4 <<= 8;
					res4 &= 0xff00;
					res3 |= res4;
					printf("  %4d ", res3);
					break;

			}

		}
		printf("\t");
//#endif
		doHeartbeat();
	}											  //endof while()
}												  // endof main()
#endif
