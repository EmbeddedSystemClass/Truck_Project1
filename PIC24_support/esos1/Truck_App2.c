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
//*************************************** get_comm2  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(get_comm2)
{
    static  uint8_t data1;
    static int i;

    ESOS_TASK_BEGIN();
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("get_comm2");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
	i = 0;
    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

//#if 0
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT83(':');
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
		ESOS_TASK_WAIT_ON_SEND_UINT83('\n');
		ESOS_TASK_WAIT_ON_SEND_UINT83('\r');
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//#endif
		avr_send_data[i+AVR_SEND_DATA_SIZE - TEMP_EEPROM_OFFSET] = data1;

		if(++i > AVR_SEND_DATA_SIZE - TEMP_EEPROM_OFFSET)
			i = 0;
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
		}
    } // endof while()
    ESOS_TASK_END();
}

//******************************************************************************************//
//************************************** poll_comm2  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(poll_comm2)
{
    static ESOS_TASK_HANDLE cmd_param_task;
    static uint8_t key;
    static uint8_t wkey;
    static uint16_t wkey16;

    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(comm2_task);
//    cmd_param_task = esos_GetTaskHandle(test1);
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("poll comm2 input task");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/

	while (TRUE)
	{

		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(key);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

		wkey = get_keypress(key);
		__esos_CB_WriteUINT8(cmd_param_task->pst_Mailbox->pst_CBuffer,wkey);

//		if(wkey <= KP_D && wkey >= KP_POUND)
#if 0
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("key: ");
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(key);
		if(wkey == KP_POUND)
			ESOS_TASK_WAIT_ON_SEND_STRING("POUND_SIGN");
		if(wkey == KP_AST)
			ESOS_TASK_WAIT_ON_SEND_STRING("ASTERICK");
		else
		{
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey-0xE2);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)wkey16);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(wkey16>>8));
		}
		ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
		ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm2_task ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(comm2_task)
{
    static ESOS_TASK_HANDLE comm2_handle;

	static uint8_t ret_char, wkey;
    static int i,j,k, m;
	static uint8_t low_byte, high_byte;
	static char tlabel[MAX_LABEL_LEN];
	static uint8_t t1, t2, t3, t4;
//	static int menus[6];

//    int i, size, start_addr;
//    static int once = 1;

    ESOS_TASK_BEGIN();
    comm2_handle = esos_GetTaskHandle(send_comm2);

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("****************************************************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("******************** comm2_task ********************");
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
	memset((void*)avr_send_data,0xFF,AVR_SEND_DATA_SIZE);
	for(i = 0;i < NUM_LABELS;i++)
	{
		j = strlen((const char *)labels[i]);
		memcpy((void *)avr_send_data+k, (void *)&labels[i],j);
		avr_send_data[k+j] = 0;
		k++;
//		*(avr_send_data+j+k) = 0;
		k += j;
	}
	// fill last 1/2 of avr_send_data with something
/*
	for(i = AVR_SEND_DATA_SIZE - TEMP_EEPROM_OFFSET;i < AVR_SEND_DATA_SIZE;i++)
	{
		avr_send_data[i] = i+20;
	}
*/
	ESOS_TASK_WAIT_TICKS(100);

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
	curr_execchoice = 0;
	goffset = 0;
	get_label_offsets();

	while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
//		while(1)
        {
			wkey = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
#if 0
			if(key1 == KP_AST)
				ESOS_TASK_WAIT_ON_SEND_UINT8('*');
			else if(key1 == KP_POUND)
				ESOS_TASK_WAIT_ON_SEND_UINT8('#');
			else
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(key1-0xE2);
#endif
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(wkey);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
#endif
			__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,wkey);

			if(wkey >= KP_POUND && wkey <= KP_D)
			{
				ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(wkey);
	/* start */
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
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
				low_byte = menu_structs[get_curr_menu()].fptr;
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);

				i = get_curr_menu();
				unpack(i,&low_byte,&high_byte);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,high_byte);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);

	#if 0
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(wkey);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
				low_byte = menu_structs[get_curr_menu()].fptr;
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				i = get_curr_menu();
				unpack(i,&low_byte,&high_byte);
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
				ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	#endif
				k = get_label(get_curr_menu(),tlabel);
	/* start */
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();

				for(i = 0;i < k;i++)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8(tlabel[i]);
				}
				ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
				ESOS_TASK_WAIT_ON_SEND_STRING("->");
				ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);

				i = 0;
				do
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8(state[i]);
					i++;
				}while(i < 20 && state[i] != 0);

				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	/* end */
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

				for(i = 0;i < 6;i++)
				{
					k = get_label(menu_structs[get_curr_menu()].menus[i] ,tlabel);
	//				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[get_curr_menu()].menus[i]);
					m = menu_structs[get_curr_menu()].menus[i];
					unpack(m,&low_byte,&high_byte);

					__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,high_byte);
					__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);
	/* start */
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();

					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(high_byte);
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
					ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(m>>8));
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)m);
					ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
	/* end */
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
					ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	#endif
	/*
					ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
					ESOS_TASK_WAIT_ON_GET_UINT82(t1);
					ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
	*/
	#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
					ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	#endif
	/*
					ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
					ESOS_TASK_WAIT_ON_GET_UINT82(t2);
					ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
	*/
					for(j = 0;j < k;j++)
					{
						ESOS_TASK_WAIT_ON_SEND_UINT8(tlabel[j]);
					}
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

	#if 0
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)m);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(m>>8));
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(t1);
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(t2);
	#endif
	//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	//				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				}
	//			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	//			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

				j = TEMP_EEPROM_OFFSET;	// this should be no of bytes to write for avr_send_data

				unpack(j,&low_byte,&high_byte);

				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,high_byte);
				__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);

	#if 0
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(t3);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(t4);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
	//#if 0
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(t3);
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(t4);
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	#endif
	//#if 0
				for(i = AVR_SEND_DATA_SIZE - TEMP_EEPROM_OFFSET;i < AVR_SEND_DATA_SIZE;i++)
				{
					low_byte = (UCHAR)i;
					__esos_CB_WriteUINT8(comm2_handle->pst_Mailbox->pst_CBuffer,low_byte);
				}

	#if 0
	/* start */
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				for(i = TEMP_EEPROM_OFFSET;i < AVR_SEND_DATA_SIZE;i++)
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
	/* end */
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	#endif
	/*
				for(i = 0;i < send_aux_data;i++)
				{
					ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
					avr_send_data[i] = low_byte;
				}
	*/
	//#endif
	/* start */
	#if 0

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	//			for(i = 0;i < j;i++)
				for(i = 0;i < 250;i++)
				{
					if(avr_send_data[i] > 0x1f && avr_send_data[i] < 0x7e)
						ESOS_TASK_WAIT_ON_SEND_UINT8(avr_send_data[i]);
					else if(avr_send_data[i] == 0)
						ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
					else
						ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
				}
	//			for(i = AVR_SEND_DATA_SIZE-NUM_CHECKBOXES-1;i < AVR_SEND_DATA_SIZE-1;i++)
	//				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
	/* end */
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	#endif

	// if key returned is one of the non_func codes then do something...
	#if 0
				switch(ret_char)
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
			else if(wkey == READ_EEPROM)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				for(i = 0;i < 300;i++)
				{
					if(avr_send_data[i] > 0x1f && avr_send_data[i] < 0x7e)
						ESOS_TASK_WAIT_ON_SEND_UINT8(avr_send_data[i]);
					else if(avr_send_data[i] == 0)
						ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
					else if( avr_send_data[i] == 0xff)
						ESOS_TASK_WAIT_ON_SEND_UINT8('_');
					else
						ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
				}
				for(i = 300;i < AVR_SEND_DATA_SIZE-300;i++)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
				}
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			}
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
    static uint8_t data1;
    static uint8_t ret_key;
    static int i;
    static int j;
    static int k;
    static int size;
    static int start_addr;
    static int type;
	static UCHAR high_byte, low_byte, conf;
	static char temp[20];

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("****************************************************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("******************** comm1_task ********************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("****************************************************");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	memset((void*)avr_send_data,0,AVR_SEND_DATA_SIZE);

	while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_STRING("in key: ");
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			switch(data1)
			{
				case KP_A:
					size = 234;
					start_addr = 0;
					j = k = 0;
					memset((void*)avr_send_data,0,AVR_SEND_DATA_SIZE);
					for(i = 0;i < NUM_LABELS;i++)
					{
						j = strlen((const char *)labels[i]);
						memcpy((void *)avr_send_data+k, (void *)&labels[i],j);
						k++;
						*(avr_send_data+j+k) = 0;
						k += j;
					}
					ret_key = BURN_PART1;
					break;
				case KP_B:
					size = sizeof(RT_PARAM)*NUM_RT_PARAMS;
					start_addr = RT_PARAMS_OFFSET_EEPROM_LOCATION;
					j = k = 0;
					memset((void *)avr_send_data,0,AVR_SEND_DATA_SIZE);
					for(i = 0;i < NUM_RT_PARAMS;i++)
					{
						j = sizeof(RT_PARAM);
						memcpy((void *)avr_send_data+k, (void *)&rt_params[i],j);
						k += j;
					}
					ret_key = BURN_PART2;
					break;
				case KP_C:
					size = NO_MENUS_EEPROM_LOCATION-NO_MENU_LABELS_EEPROM_LOCATION;
					start_addr = NO_MENU_LABELS_EEPROM_LOCATION;
					ret_key = BURN_PART3;
					break;
				case KP_D:
					strcpy(temp,"~Dan Hampleman~\0");
//					memset(temp,0xFE,16);
					memcpy((void*)avr_send_data,(const void*)temp,16);
					size = 16;
					start_addr = 0x140;
					ret_key = BURN_PART4;
					break;
				case READ_EEPROM:
					ret_key = READ_EEPROM;
					start_addr = 0;
					size = AVR_SEND_DATA_SIZE;
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
					ret_key = 0;
					break;

				default:
					ret_key = 0;
					break;
			}

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		    ESOS_TASK_WAIT_ON_SEND_STRING("ret_key ");
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(ret_key);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_WAIT_ON_SEND_STRING("size & start_addr:   ");
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)size);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(size>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)start_addr);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(start_addr>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

//#if 0
			if(ret_key >= READ_EEPROM && ret_key <= BURN_PART4 && size > 0)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("starting writes/reads   ");
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
// write the key char
				if(ret_key == READ_EEPROM)
					data1 = READ_EEPROM;
				else
					data1 = BURN_PART;
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(data1);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();


				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
// unpack the size
//#if 0
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("size: ");

				unpack(size,&low_byte,&high_byte);

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)size);
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(size>>8));
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

// write the high_byte
//				high_byte = 0;
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
// read back the high_byte
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(conf);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(conf);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

// write the low_byte
//				low_byte = 0xEA;
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
// read back the low_byte
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
// unpack the start_addr
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("start_addr: ");

				unpack(start_addr,&low_byte,&high_byte);
// write the high_byte
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
				ESOS_TASK_WAIT_ON_SEND_UINT82(high_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(high_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
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

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
// read back the low_byte
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

//#endif
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("starting block write/read   ");
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

				j = 0;
//				for(i = start_addr;i < start_addr+size;i++)
//				size = 0xEA;
				for(i = 0;i < size;i++)
				{
// write 1st then read
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
					ESOS_TASK_WAIT_ON_SEND_UINT82(avr_send_data[i]);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
//					ESOS_TASK_WAIT_TICKS(1);
					ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
					ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
					ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
//					avr_send_data[i] = low_byte;
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					if(ret_key == BURN_PART1)
					{
						if(low_byte < 0x1f || low_byte > 0x7e)
							if(low_byte == 0) ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
							else ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
						else
							ESOS_TASK_WAIT_ON_SEND_UINT8(low_byte);
					}else
						ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
#if 0
					if(low_byte == 0)
						ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
					else
						ESOS_TASK_WAIT_ON_SEND_UINT8(low_byte);
#endif
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

					j++;
				}
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("bytes sent:   ");
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)j);
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(j >> 8));
//				for(i = 0;i < size;i++)
//					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			}
//#endif
		}	// end while got mail
	}	// end while true
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm3_task ***************************************//
//******************************************************************************************//
// comm 3 is the monitor
ESOS_USER_TASK(comm3_task)
{
    static  uint8_t key;
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
//***************************************** test1  *****************************************//
//******************************************************************************************//
ESOS_USER_TASK(test1)
{
    static  uint8_t wkey;

    ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("test1");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

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

//	ESOS_INIT_SEMAPHORE(key_sem,0);
//	esos_RegisterTask(keypad);
//	esos_RegisterTask(poll_keypad);
/*
	ESOS_INIT_SEMAPHORE(send_sem,0);

	esos_RegisterTask(recvFPGA);
	esos_RegisterTask(sendFPGA);
	esos_RegisterTask(sendFPGA2);
	esos_RegisterTask(send_cmd_param);
*/
//	esos_RegisterTask(comm3_task);
//	esos_RegisterTask(comm1_task);
	esos_RegisterTask(poll_comm2);
	esos_RegisterTask(comm2_task);
	esos_RegisterTask(send_comm2);
	esos_RegisterTask(get_comm2);
//	esos_RegisterTask(test1);
//	esos_RegisterTask(convADC);
} // end user_init()


