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
//************************************** poll_comm2  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(poll_comm2)
{
    static ESOS_TASK_HANDLE cmd_param_task;
    static uint8_t key;
    static uint8_t wkey;

    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(comm1_task);
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

		switch(key)
		{
			case '0':
				wkey = KP_0;
				break;
			case '1':
				wkey = KP_1;
				break;
			case '2':
				wkey = KP_2;
				break;
			case '3':
				wkey = KP_3;
				break;
			case '4':
				wkey = KP_4;
				break;
			case '5':
				wkey = KP_5;
				break;
			case '6':
				wkey = KP_6;
				break;
			case '7':
				wkey = KP_7;
				break;
			case '8':
				wkey = KP_8;
				break;
			case '9':
				wkey = KP_9;
				break;
			case '*':
				wkey = KP_AST;
				break;
			case '#':
				wkey = KP_POUND;
				break;
			case 'A':
			case 'a':
				wkey = KP_A;
				break;
			case 'B':
			case 'b':
				wkey = KP_B;
				break;
			case 'C':
			case 'c':
				wkey = KP_C;
				break;
			case 'D':
			case 'd':
				wkey = KP_D;
				break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
			case 'Y':
			case 'y':
				wkey = KP_POUND;
				break;
			case 'Z':
			case 'z':
				wkey = KP_AST;
				break;
			case 'V':
			case 'v':
				wkey = BURN_PART1;
				break;
			case 'W':
			case 'w':
				wkey = BURN_PART2;
				break;
			case 'U':
			case 'u':
				wkey = BURN_PART3;
				break;
			case 'R':
			case 'r':
				wkey = BURN_PART4;
				break;
			case 'S':
			case 's':
				wkey = SHOW_EEPROM;
				break;
			case 'L':
			case 'l':
				wkey = LOAD_MENU_STRUCT;
				break;
			case 'T':
			case 't':
				wkey = 0xff;
				break;
			case 'E':
			case 'e':
				wkey = READ_EEPROM;
				break;
			case 'P':
			case 'p':
				wkey = 0xff;
				break;
			case 'I':
			case 'i':
				wkey = INIT;
				break;
			case ' ':
				wkey = SPACE;
				break;
			default:
				wkey = 0xff;
				break;
		}

		if(wkey <= KP_D && wkey >= KP_POUND)
		{
			__esos_CB_WriteUINT8(cmd_param_task->pst_Mailbox->pst_CBuffer,wkey);
//#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(key);
			if(wkey == KP_POUND)
				ESOS_TASK_WAIT_ON_SEND_STRING("POUND_SIGN");
			if(wkey == KP_AST)
				ESOS_TASK_WAIT_ON_SEND_STRING("ASTERICK");
			else
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey-0xE2);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//#endif
		}
		key = 0;
		wkey = 0;
	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//*************************************** comm2_task ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(comm2_task)
{
	static uint8_t key1, ret_char;
    static int i,j,k;
    static uint8_t size, low_byte, high_byte;
	static char tlabel[MAX_LABEL_LEN];
	static MENU_FUNC_STRUCT mf;
	static MENU_FUNC_STRUCT *pmf;

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

//	no_menu_labels = NUM_LABELS + NUM_RT_LABELS;
	no_menu_labels = NUM_LABELS;
	no_rt_labels = test - rpm;
	pmf = &mf;
	memset(pmf,0,sizeof(MENU_FUNC_STRUCT));

	i = 0;
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B,  MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1D);
// 1e
	i = update_menu_structs(i, _non_func,		test, blank, blank,   blank, blank, blank, MENU1E);
// 2a
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2A);
// 2b
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2B);
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

#if 0
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU1E, MENU2A,  MAIN);
	i = update_menu_structs(i, _menu_change,	MENU2B, MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1A);
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
	i = update_menu_structs(i, _exec_choice,	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B, MENU1C);
	i = update_menu_structs(i, _exec_choice,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1D);
	i = update_menu_structs(i, _do_chkbox,		MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1E);
	i = update_menu_structs(i, _do_chkbox,	 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B, MENU2A);
	i = update_menu_structs(i, _non_func,		MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU2A);
	i = update_menu_structs(i, _non_func,		MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU2B);
	i = update_menu_structs(i, _do_numentry, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B, MENU2C);
	i = update_menu_structs(i, _do_numentry,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU2D);
	i = update_menu_structs(i, _do_numentry,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B, MENU1C);
#endif
	memset((void*)avr_send_data,0,AVR_SEND_DATA_SIZE);
	for(i = 0;i < NUM_LABELS;i++)
	{
		j = strlen((const char *)labels[i]);
		memcpy((void *)avr_send_data+k, (void *)&labels[i],j);
		k++;
		*(avr_send_data+j+k) = 0;
		k += j;
	}

#if 0
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
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
	goffset = 0;
	get_label_offsets();
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	for(i = 0;i < no_menu_labels;i++)
	{
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(label_offsets[i]);
	}
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	for(i = 0;i < no_menu_labels;i++)
	{
		k = get_label(i,tlabel);
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(k);
		ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
		for(j = 0;j < k;j++)
		{
			if(tlabel[j] > 0x1f && tlabel[j] < 0x7e)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8(tlabel[j]);
			}
			else
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(tlabel[j]);
		}
		ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
		ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	}
#endif
	ESOS_TASK_WAIT_TICKS(100);
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();

	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

	for(i = 0;i < NUM_MENUS;i++)
	{
//		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[i].fptr);
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(i);
		ESOS_TASK_WAIT_ON_SEND_UINT8(':');
		ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
		for(j = 0;j < 6;j++)
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[i].menus[j]);
//		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[i].index);
		ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
		ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	}
#if 0
	for(i = 0;i < NUM_MENUS;i++)
	{
		memcpy(avr_send_data+RT_PARAMS_OFFSET_EEPROM_LOCATION+(i*sizeof(MENU_FUNC_STRUCT)),
			&menu_structs[i],sizeof(MENU_FUNC_STRUCT));
	}
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	for(i = 0;i < AVR_SEND_DATA_SIZE;i++)
	{
		if(avr_send_data[i] > 0x1F && avr_send_data[i] < 0x7e)
			ESOS_TASK_WAIT_ON_SEND_UINT8(avr_send_data[i]);
		else if(avr_send_data[i] == 0)
			ESOS_TASK_WAIT_ON_SEND_UINT8('_');
		else
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);
	}
#endif

	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

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
        {
			key1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
#if 0
			if(key1 == KP_AST)
				ESOS_TASK_WAIT_ON_SEND_UINT8('*');
			else if(key1 == KP_POUND)
				ESOS_TASK_WAIT_ON_SEND_UINT8('#');
			else
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(key1-0xE2);
#endif
			ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(key1);
		    ESOS_TASK_WAIT_ON_SEND_STRING("curr_menu: ");
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)get_curr_menu());
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		    ESOS_TASK_WAIT_ON_SEND_STRING("fptr: ");
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)menu_structs[get_curr_menu()].fptr);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

			k = get_label(get_curr_menu(),tlabel);

			for(i = 0;i < k;i++)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8(tlabel[i]);
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
		    ESOS_TASK_WAIT_ON_SEND_STRING("->");
			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);

			for(i = 0;i < 20;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT8(state[i]);

			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
			for(i = 0;i < 6;i++)
			{
				k = get_label(menu_structs[get_curr_menu()].menus[i] ,tlabel);
//				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(menu_structs[get_curr_menu()].menus[i]);
				ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
				for(j = 0;j < k;j++)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8(tlabel[j]);
				}
				ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

			for(i = AVR_SEND_DATA_SIZE-NUM_CHECKBOXES-1;i < AVR_SEND_DATA_SIZE-1;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(avr_send_data[i]);

			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


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
	static UCHAR high_byte, low_byte;
	static char temp[20];

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm1_task on comm1");
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
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1-0xE2);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

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
					ret_key = 0;
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
				memset((void*)avr_send_data,0,AVR_SEND_DATA_SIZE);
				for(i = 0;i < NUM_LABELS;i++)
				{
					j = strlen((const char *)labels[i]);
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
				memset((void *)avr_send_data,0,AVR_SEND_DATA_SIZE);
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
//				strcpy(temp,"~Dan Hampleman~\0");
				memset(temp,0xFE,16);
				memcpy(avr_send_data,temp,16);
				size = 16;
				start_addr = 0x140;
				break;
				default:
				size = 0;
				start_addr = 0;
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
			if(ret_key >= BURN_PART && ret_key <= BURN_PART4 && size > 0)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_STRING("starting writes/reads   ");
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
// write the key char
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

ESOS_USER_TASK(test1)
{
    static  uint8_t data1;
    static  uint8_t data2;

    ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("test1");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


	data1 = 0x21;
	data2 = 0;
    while (1)
    {

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();


		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

		if(++data1 > 0x7e)
			data1 = 0x21;

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_UINT8(~data2);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

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
//	esos_RegisterTask(test1);
	esos_RegisterTask(comm1_task);
	esos_RegisterTask(poll_comm2);
//	esos_RegisterTask(comm2_task);

//	esos_RegisterTask(convADC);
} // end user_init()


