// .. "Copyright (c) 2016 Dan Hampleman ("AUTHOR")"
//    All rights reserved.
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
// *********************************************
// Truck_App2.c
// *********************************************

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need

/************************************************************************
 * User supplied functions
 ************************************************************************
 */
#define _TRUCK_APP
#include "Truck_App.h"

/*
PIC24 UARTS:

UART1 - RS232 to TS-7200
UART2 - AVR
UART3 - monitor
*/

//******************************************************************************************//
//************************************** data_to_AVR ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(data_to_AVR)
{
    static  uint8_t data = 2;
    static  uint8_t data1 = 0;
	static  uint16_t data2 = 0;
	static char tempx[20];
//    static  uint8_t code = RT_RPM;

    ESOS_TASK_BEGIN();
//    h_Task3 = esos_GetTaskHandle(task3);
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
    ESOS_TASK_WAIT_ON_SEND_UINT83('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT83('\r');
	ESOS_TASK_WAIT_ON_SEND_STRING("data_to_AVR");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING3(0x41);
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();

	while(TRUE)
    {

    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm2_task ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(comm2_task)
{
	uint8_t key1;

//    int i, size, start_addr;
//    static int once = 1;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm2_task");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	while(TRUE)
    {
//		if(++data1 > 0x7e)
//			data1 = 0x21;
#if 0
		if(once == 1)
		{
			size = AVR_SEND_DATA_SIZE;
			ret_key = get_key((UCHAR)key1,size,(UCHAR*)avr_send_data);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			for(i = 0;i < AVR_SEND_DATA_SIZE;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT82(avr_send_data[i]);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
			once =0;
		}
#endif
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			key1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(key1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

// UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type)

#if 0
			ret_key = get_key((UCHAR)key1,size,start_addr,(UCHAR*)avr_send_data,0);
		    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		    for(i = 0;i < AVR_SEND_DATA_SIZE;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT82(avr_send_data[i]);
		    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
#endif
			ESOS_TASK_WAIT_TICKS(100);

// if key returned is one of the non_func codes then do something...
#if 0
			switch(ret_key)
			{
				case NF_1:
				break;
				case NF_2:
				break;
				case NF_3:
				break;
				case NF_4:
				break;
				case NF_5:
				break;
				case NF_6:
				break;
				case NF_7:
				break;
				case NF_8:
				break;
				case NF_9:
				break;
				case NF_10:
				break;
				default:
				break;
			}
#endif
		}

	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//*************************************** comm1_task ***************************************//
//******************************************************************************************//
// comm 1 is the TS-7200

ESOS_USER_TASK(comm1_task)
{
    static  uint8_t data1, ret_key;
    static int i,j,k, size, start_addr, type;
    UCHAR high_byte, low_byte;


    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm1_task on comm1");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	memset(avr_send_data,0,AVR_SEND_DATA_SIZE);

	while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			switch(data1)
			{
				case KP_A:
					ret_key = BURN_PART1;
					break;
				case KP_B:
					ret_key = BURN_PART2;
					break;
				case KP_C:
					ret_key = BURN_PART3;
					break;
				case KP_D:
					ret_key = BURN_PART4;
					break;
				case KP_AST:
					i = 0;
					i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B,  MAIN);
					i = update_menu_structs(i, _menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A);
					i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
					i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1C);
					i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1D);
					i = update_menu_structs(i, _non_func,		test, blank, blank,   blank, blank, blank, MENU1E);
					i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2A);
					i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2B);
					i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2C);
					i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2D);
					i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2E);
					i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3A);
					i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3B);
					ret_key = data1;
					break;					
				default:
					ret_key = 0;
					break;
			}

			switch(ret_key)
			{
				case BURN_PART1:		// burn the labels section
				size = 234;
				start_addr = 0;
				j = k = 0;
				memset(avr_send_data,0,AVR_SEND_DATA_SIZE);
				for(i = 0;i < NUM_LABELS;i++)
				{
					j = strlen(labels[i]);
					memcpy((void *)avr_send_data+k, (void *)&labels[i],j);
					k++;
					*(avr_send_data+j+k) = 0;
					k += j;
				}
				break;
				case BURN_PART2:		// burn the rt_params section starting at 0x0350
//						size = sizeof(RT_PARAM)*no_rtparams;
				size = sizeof(RT_PARAM)*NUM_RT_PARAMS;
				start_addr = RT_PARAMS_OFFSET_EEPROM_LOCATION;
				j = k = 0;
				memset(avr_send_data,0,AVR_SEND_DATA_SIZE);
				for(i = 0;i < NUM_RT_PARAMS;i++)
				{
					j = sizeof(RT_PARAM);
					memcpy((void *)avr_send_data+k, (void *)&rt_params[i],j);
					k += j;
				}
				break;
				case BURN_PART3:		// burn the section for the number of's
				size = NO_MENUS_EEPROM_LOCATION-NO_MENU_LABELS_EEPROM_LOCATION;
				start_addr = NO_MENU_LABELS_EEPROM_LOCATION;
				break;
				case BURN_PART4:		// test section starting at 0x0140 (14 chars like: my name)
				size = 14;
				start_addr = 0x140;
				break;
				default:
				size = 0;
				start_addr = 0;
				break;
			}
			if(ret_key == BURN_PART1)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(k);
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				for(i = 0;i < k;i++)
				{
					if(avr_send_data[i] == 0)
						ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
					else ESOS_TASK_WAIT_ON_SEND_UINT8(avr_send_data[i]);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//					ESOS_TASK_WAIT_TICKS(1);
				}
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}

			if(ret_key == BURN_PART2)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(k);
				k = sizeof(RT_PARAM);
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(k);
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				for(i = 0;i < sizeof(RT_PARAM)*NUM_RT_PARAMS;i++)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
				}
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}
			if(ret_key == KP_AST)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				
				for(i = 0;i < NUM_MENUS;i++)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[i].fptr);
					for(j = 0;j < 6;j++)
						ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[i].menus[j]);
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[i].index);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				}
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)size);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(size>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)start_addr);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(start_addr>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

#if 0
	// write the key char
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	// unpack the size
			unpack(size,&low_byte,&high_byte);
	// write the high_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	// read back the high_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
			ESOS_TASK_WAIT_ON_GET_UINT82(high_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(high_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	// write the low_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	// read back the low_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
			ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	// unpack the start_addr
			unpack(start_addr,&low_byte,&high_byte);
	// write the high_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	// read back the high_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
			ESOS_TASK_WAIT_ON_GET_UINT82(high_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(high_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	// write the low_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	// read back the low_byte
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
			ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
// write 1st then read
			for(i = start_addr;i < start_addr+size;i++)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(avr_send_data[i]);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			}
#endif
			ESOS_TASK_WAIT_TICKS(10);
		}	// end while got mail
	}	// end while true
    ESOS_TASK_END();
}
UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type)
{
	return ch;
}
//******************************************************************************************//
//*************************************** comm3_task ***************************************//
//******************************************************************************************//
// comm 3 is the monitor
ESOS_USER_TASK(comm3_task)
{
    static  uint8_t data;
    static  uint8_t data2;
    static	uint8_t dec;
//    static ESOS_TASK_HANDLE h_Task3;

    ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm3_task");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


	data = 0x21;
	data2 = 0;
    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_ON_SEND_UINT82(data);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

		if(++data2 > 0x0f)
			data2 = 0;

		ESOS_TASK_WAIT_TICKS(TDELAY2/2);

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_UINT8(data);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        if(++data > 0x7e)
        	data = 0x21;
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** user_init  ***************************************//
//******************************************************************************************//
void user_init(void)
{
//    __esos_unsafe_PutString( HELLO_MSG );
//	CONFIG_SPI_MASTER()
//	CONFIG_SPI_SLAVE();
//	esos_RegisterTask(echo_spi_task);
	configKeypad();
	ESOS_INIT_SEMAPHORE(key_sem,0);
	esos_RegisterTask(keypad);
	esos_RegisterTask(poll_keypad);
/*
	ESOS_INIT_SEMAPHORE(send_sem,0);

	esos_RegisterTask(recvFPGA);
	esos_RegisterTask(sendFPGA);
	esos_RegisterTask(sendFPGA2);
	esos_RegisterTask(send_cmd_param);
*/
//	esos_RegisterTask(comm3_task);
	esos_RegisterTask(comm1_task);
//	esos_RegisterTask(comm2_task);

//	esos_RegisterTask(convADC);
} // end user_init()
