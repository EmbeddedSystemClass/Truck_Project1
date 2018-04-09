#if 1
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
in Truck_Project1 directory
PIC24 UARTS:

UART1 - RS232 to TS-7200
UART2 - AVR
UART3 - monitor
*/
//comm2 is 115
//comm1 is 11o

//******************************************************************************************//
//*************************************** get_sync  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(get_sync)
{
	static int i;
	static UCHAR buf[7];// = {0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA,0};
    static ESOS_TASK_HANDLE get_comm2_handle;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("get_sync started");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


    get_comm2_handle = esos_GetTaskHandle(get_comm2);

	sync = 0;
	memset(buf,0,4);

	for(i = 0;i < 3;i++)
		buf[i] = 0x55;

	for(i = 3;i < 6;i++)
		buf[i] = 0xAA;

	for(i = 0;i < 6;i++)
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[i]);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	for(i = 0;i < 6;i++)
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_ON_SEND_UINT82(buf[i]);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_TICKS(TIME_DELAY);
	}

	while(!sync)
	{
		i = 0;
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("waiting...");
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
			ESOS_TASK_WAIT_ON_GET_UINT82(buf[i]);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
			i++;
		}while(i < 4);

		for(i = 0;i < 4;i++)
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[i]);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
		if(buf[0] == 0x51 && buf[1] == 0x52 && buf[2] == 0xA1 && buf[3] == 0xA2)
		{
			sync = 1;

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		    ESOS_TASK_WAIT_ON_SEND_STRING("get_sync going to sleep  ");
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			ESOS_WAKE_TASK(get_comm2_handle);
			ESOS_TASK_SLEEP();
		}
		ESOS_TASK_WAIT_TICKS(TIME_DELAY);
	}
    ESOS_TASK_END();
}
#endif
//******************************************************************************************//
//*************************************** get_comm2  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(get_comm2)
{
    static  uint8_t data1;

	
    ESOS_TASK_BEGIN();
//    FLUSH_ESOS_COMM_IN_DATA2();
//    ESOS_TASK_SLEEP();
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("get_comm2");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
	avr_ptr = 0;

    while (1)
    {
//		ESOS_TASK_WAIT_SEMAPHORE(comm2_sem,1);

		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

#if 0
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//		ESOS_TASK_WAIT_ON_SEND_UINT8(':');
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif

		aux_string[avr_ptr+(AUX_STRING_LEN - TEMP_EEPROM_OFFSET)] = data1;

		if(++avr_ptr > TEMP_EEPROM_OFFSET - 1)
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_STRING("reset");
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			avr_ptr = 0;
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** send_comm2  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(send_comm2)
{
    static  uint8_t data1;

	
	
    ESOS_TASK_BEGIN();

    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_TICKS(TIME_DELAY);
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(':');
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** menu_task ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(menu_task)
{
    static ESOS_TASK_HANDLE comm2_handle;
    static ESOS_TASK_HANDLE sleeping_handle;

	static uint8_t ret_char, wkey;
    static int i,j,k;

    ESOS_TASK_BEGIN();
    comm2_handle = esos_GetTaskHandle(send_comm2);
    sleeping_handle = esos_GetTaskHandle(get_comm2);

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("****************************************************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("******************** menu_task ********************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("****************************************************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

//	no_menu_labels = NUM_LABELS + NUM_RT_LABELS;
	no_menu_labels = NUM_LABELS;
	no_rt_labels = test - rpm;

	i = 0;
	
//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1C, MENU1D, MENU1E, MENU2A, MENU2B, MENU2C, MAIN);
//	i = update_menu_structs(i, _menu_change, 	MENU2C, MENU2D, MENU2E,  MENU3A, MENU3B, MENU1C, MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2B, MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU1D);
// 1e
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, blank, ckenter, blank, blank, MENU1E);
// 2a
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, blank, ckenter, blank, blank, MENU2A);
// 2b
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, blank, ckenter, blank, blank, MENU2B);
// 2c
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2C);
// 2d
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2D);
// 2e
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2E);
// 3a
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3A);
// 3b
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3B);

	memset(cblabels,0,CBLABEL_SIZE);
	total_offset = 0;
	i = 0;
	i =  update_cblabels(i, "test 1\0");
	i =  update_cblabels(i, "test 2\0");
	i =  update_cblabels(i, "test 3\0");
	i =  update_cblabels(i, "test 4\0");
	i =  update_cblabels(i, "test 5\0");
	i =  update_cblabels(i, "test 6\0");
	i =  update_cblabels(i, "test 7\0");
	i =  update_cblabels(i, "test 8\0");
	i =  update_cblabels(i, "test 9\0");
	i =  update_cblabels(i, "test 10\0");

	i =  update_cblabels(i, "asdf 1\0");
	i =  update_cblabels(i, "asdf 2\0");
	i =  update_cblabels(i, "asdf 3\0");
	i =  update_cblabels(i, "asdf 3\0");
	i =  update_cblabels(i, "asdf 4\0");
	i =  update_cblabels(i, "asdf 6\0");
	i =  update_cblabels(i, "asdf 7\0");
	i =  update_cblabels(i, "asdf 8\0");
	i =  update_cblabels(i, "asdf 9\0");
	i =  update_cblabels(i, "asdf 10\0");

	i =  update_cblabels(i, "hello 1\0");
	i =  update_cblabels(i, "hello 2\0");
	i =  update_cblabels(i, "hello 3\0");
	i =  update_cblabels(i, "hello 4\0");
	i =  update_cblabels(i, "hello 5\0");
	i =  update_cblabels(i, "hello 6\0");
	i =  update_cblabels(i, "hello 7\0");
	i =  update_cblabels(i, "hello 8\0");
	i =  update_cblabels(i, "hello 9\0");
	i =  update_cblabels(i, "hello 10\0");

	i =  update_cblabels(i, "funny 1\0");
	i =  update_cblabels(i, "funny 2\0");
	i =  update_cblabels(i, "funny 3\0");
	i =  update_cblabels(i, "funny 4\0");
	i =  update_cblabels(i, "funny 5\0");
	i =  update_cblabels(i, "funny 6\0");
	i =  update_cblabels(i, "funny 7\0");
	i =  update_cblabels(i, "funny 8\0");
	i =  update_cblabels(i, "funny 9\0");
	i =  update_cblabels(i, "funny 10\0");

	i =  update_cblabels(i, "quick 1\0");
	i =  update_cblabels(i, "quick 2\0");
	i =  update_cblabels(i, "quick 3\0");
	i =  update_cblabels(i, "quick 4\0");
	i =  update_cblabels(i, "quick 5\0");
	i =  update_cblabels(i, "quick 6\0");
	i =  update_cblabels(i, "quick 7\0");
	i =  update_cblabels(i, "quick 8\0");
	i =  update_cblabels(i, "quick 9\0");
	i =  update_cblabels(i, "quick 10\0");
/*
	i =  update_cblabels(i, "fox and  1\0");
	i =  update_cblabels(i, "fox and  2\0");
	i =  update_cblabels(i, "fox and  3\0");
	i =  update_cblabels(i, "fox and  4\0");
	i =  update_cblabels(i, "fox and  5\0");
	i =  update_cblabels(i, "fox and  6\0");
	i =  update_cblabels(i, "fox and  7\0");
	i =  update_cblabels(i, "fox and  8\0");
	i =  update_cblabels(i, "fox and  9\0");
	i =  update_cblabels(i, "fox and  10\0");
*/

/*
	i =  update_cblabels(i, "ending 1\0");
	i =  update_cblabels(i, "ending 2\0");
	i =  update_cblabels(i, "ending 3\0");
	i =  update_cblabels(i, "ending 4\0");
	i =  update_cblabels(i, "ending 5\0");
*/

	memset((void*)aux_string,0xFF,AUX_STRING_LEN);

	k = 0;
	for(i = 0;i < NUM_LABELS;i++)
	{
		j = strlen((const char *)labels[i]);
		memcpy((void *)aux_string+k, (void *)&labels[i],j);
		aux_string[k+j] = 0;
		k++;
//		*(aux_string+j+k) = 0;
		k += j;
	}
//	ESOS_TASK_WAIT_TICKS(100);

	list_size = LIST_SIZE;
	first_menu = MAIN;
	current_fptr = first_menu;
	prev_fptr = first_menu;
	memset((void*)&menu_list[0],0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	dirty_flag = 0;
	curr_checkbox = 0;
//	last_checkbox = NUM_CHECKBOXES-1;
//	last_execchoice = NUM_EXECCHOICES-1;
	goffset = 0;
	get_label_offsets();
/*
	do
	{
		
		ESOS_TASK_WAIT_TICKS(200);

	}while(ESOS_IS_TASK_SLEEPING(sleeping_handle));
*/
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("******************** starting ********************");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
//		while(1)
        {
			wkey = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			if(wkey >= KP_POUND && wkey <= KP_D)
			{
				ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(wkey);
				display_menus();
/* start */
#if 0
//#if 0
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("key: ");
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey);
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("curr_menu: ");
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)get_curr_menu());
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("fptr: ");
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)menu_structs[get_curr_menu()].fptr);
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
/* end */
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//#endif

/*
				low_byte = 0x55;
				if(wkey >= KP_0 && wkey <= KP_9)
					wkey -= 0xE2 + 0x30;
				else if(wkey >= KP_A && wkey <= KP_D)
					wkey -= 0xEC + 	0x41;
				else wkey = 0xAA;
*/
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,wkey);
				ESOS_TASK_WAIT_TICKS(TIME_DELAY);

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

				low_byte = menu_structs[get_curr_menu()].fptr;
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);
				ESOS_TASK_WAIT_TICKS(TIME_DELAY);

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

				i = get_curr_menu();
//				m++;
				unpack(i,&low_byte,&high_byte);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,high_byte);
				ESOS_TASK_WAIT_TICKS(TIME_DELAY);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);
				ESOS_TASK_WAIT_TICKS(TIME_DELAY);


				for(i = 0;i < 6;i++)
				{
//					m += i+14;
					m = menu_structs[get_curr_menu()].menus[i];
					unpack(m,&low_byte,&high_byte);

//					high_byte = (UCHAR)(m>>8);

					__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,high_byte);
//					low_byte = (UCHAR)m;
					ESOS_TASK_WAIT_TICKS(TIME_DELAY);
					__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);
					ESOS_TASK_WAIT_TICKS(TIME_DELAY);

					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(high_byte);
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


				} // end of for loop

//				send_aux_data = 30;
#if 0
				unpack(send_aux_data,&low_byte,&high_byte);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,high_byte);
				ESOS_TASK_WAIT_TICKS(TIME_DELAY);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);
				ESOS_TASK_WAIT_TICKS(TIME_DELAY);
/*
				for(j = 0;j < send_aux_data;j++)
				{
					__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,j);
					ESOS_TASK_WAIT_TICKS(TIME_DELAY);
				}
*/
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)send_aux_data);
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(send_aux_data>>8));
#endif
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
#endif
//				ESOS_TASK_SIGNAL_SEMAPHORE(comm2_sem);
			}	// end of mail
		}		// end of while
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** send_comm1  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(send_comm1)
{
	UCHAR test;
	
    ESOS_TASK_BEGIN();
	while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
		{
			test = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);			
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//***************************************** test1  *****************************************//
//******************************************************************************************//
ESOS_USER_TASK(test1)
{
    static  uint8_t data1, data2;

    ESOS_TASK_BEGIN();

	data1 = 0x21;
	data2 = 0x7e;
//	CONFIG_R0_DIG_OUTPUT();
	while(1)
	{

		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
	    ESOS_TASK_WAIT_ON_GET_UINT8(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
	    ESOS_TASK_WAIT_ON_SEND_UINT82(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_TICKS(1);
/*
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_TICKS(1);
		R0 = 1;
		ESOS_TASK_WAIT_TICKS(1);
		R0 = 0;

		if(++data1 > 0x7e)
			data1 = 0x21;
		if(--data2 < 0x21)
			data2 = 0x7e;
*/
	}

#if 0
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			wkey = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
    } // endof while()
#endif
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

	ESOS_INIT_SEMAPHORE(key_sem,0);
	ESOS_INIT_SEMAPHORE(comm2_sem,0);
	esos_RegisterTask(keypad);
	esos_RegisterTask(poll_keypad);
	esos_RegisterTask(poll_comm1);
	esos_RegisterTask(menu_task);
	esos_RegisterTask(send_comm2);
	esos_RegisterTask(get_comm2);
	esos_RegisterTask(send_comm1);
//	esos_RegisterTask(get_sync);

//	esos_RegisterTask(test1);
//	esos_RegisterTask(convADC);
} // end user_init()


