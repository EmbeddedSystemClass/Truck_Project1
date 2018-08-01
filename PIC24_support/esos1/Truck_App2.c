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
#include "Truck_App.h"
//******************************************************************************************//
//*************************************** send_fpga ****************************************//
//******************************************************************************************//
// send cmd/param as UINT with cmd in low byte and param in high
ESOS_USER_TASK(send_fpga)
{
    static  UCHAR param, cmd, end_char;
    static UINT data2;
	
    ESOS_TASK_BEGIN();
    end_char = 0xFE;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);
			param = (UCHAR)data2;
			cmd = (UCHAR)(data2 >> 8);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
			ESOS_TASK_WAIT_ON_SEND_UINT83(param);
			ESOS_TASK_WAIT_TICKS(2);
			ESOS_TASK_WAIT_ON_SEND_UINT83(cmd);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** recv_fpga ****************************************//
//******************************************************************************************//
// recv data from FPGA
ESOS_USER_TASK(recv_fpga)
{
    static UCHAR data2;
    static int i;
	
    ESOS_TASK_BEGIN();
    i = 0;
    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM3();
		ESOS_TASK_WAIT_ON_GET_UINT83(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM3();
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************* display_menu ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(display_menu)
{
	static UCHAR i;
	static UCHAR data2;

	ESOS_TASK_BEGIN();
	while (1)
	{
        ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			avr_buffer[0] = PUT_STRING;

			for(i = 0;i < NUM_MENU_LABELS;i++)
			{
				avr_buffer[1] = menu_str[i].row;
				avr_buffer[2] = menu_str[i].col;
				avr_buffer[3] = menu_str[i].str;
				AVR_CALL();
			}
		}
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//********************************** display_rtlabels **************************************//
//******************************************************************************************//
ESOS_USER_TASK(display_rtlabels)
{
	static UCHAR data2, i;

	ESOS_TASK_BEGIN();
	while (1)
	{
        ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			avr_buffer[0] = PUT_STRING;

			for(i = 0;i < NUM_RT_LABELS;i++)
			{
				avr_buffer[1] = rtlabel_str[i].row;
				avr_buffer[2] = rtlabel_str[i].col;
				avr_buffer[3] = rtlabel_str[i].str;
				AVR_CALL();
			}
		}
	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//*************************************** send_comm1 ***************************************//
//******************************************************************************************//
// send data to TS-7200
ESOS_USER_TASK(send_comm1)
{
    static UCHAR data1;
    static int i;
	
    ESOS_TASK_BEGIN();
    i = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	 		if(data1 != RT_DATA)
				ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
			else
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8(RT_DATA);
				for(i = 0;i < NUM_ADC_CHANNELS;i++)
				{
//					ESOS_TASK_WAIT_ON_SEND_UINT8(u16_pot[i]>>8);
//					ESOS_TASK_WAIT_ON_SEND_UINT8(u16_pot[i]);
					ESOS_TASK_WAIT_ON_SEND_UINT8(u8_pot[i]);
				}
			}
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** recv_comm1 ***************************************//
//******************************************************************************************//
// recv data from TS-7200
ESOS_USER_TASK(recv_comm1)
{
    static UCHAR data1, data2;
    static int i,j;
    static UCHAR temp_buffer[20];
	
    ESOS_TASK_BEGIN();
    i = 0;

	avr_buffer[0] = CHAR_CMD;

    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		if(data1 == 0xFC)
		{	
			avr_buffer[0] = LCD_CLRSCR;
			avr_buffer[1] = 1;
			AVR_CALL();
			avr_buffer[0] = PUT_STR;
			avr_buffer[1] = 7;	// row 7
			avr_buffer[2] = 0;	// col 0
			avr_buffer[3] = 1;	// string 1 ("enter password")
			AVR_CALL();
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 7;
			avr_buffer[2] = 14;
			AVR_CALL();
			password_valid = 0;
			password_ptr = 0;
			password_retries = 0;
			key_mode = PASSWORD;
			memset(password,0,sizeof(password));
		}
		else if(data1 == 0xFB)
		{
			i = 0;
			do
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				ESOS_TASK_WAIT_ON_GET_UINT8(data1);
				temp_buffer[i] = data1;
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}while(data1 != 0xFE && i < PASSWORD_SIZE);
			if(i < PASSWORD_SIZE)
			{
				temp_buffer[i] = 0;
				strcpy(correct_password,temp_buffer);
			}
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** recv_lcd *****************************************//
//******************************************************************************************//
// recv data from AVR
ESOS_USER_TASK(recv_lcd)
{
    static UCHAR data2;
    static int i;
	curr_num_ptr = 0;

	memset(curr_num,0,NUM_SIZE);

    ESOS_TASK_BEGIN();
    i = 0;

    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

		if(data2 == 0xFD)
			ESOS_SIGNAL_SEMAPHORE(lcd_sem,1);
		else
		{
			curr_num[curr_num_ptr] = data2;
			curr_num_ptr++;
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);

			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFD);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
			if(curr_num > NUM_SIZE)
				curr_num_ptr = 0;
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** AVR_cmd ******************************************//
//******************************************************************************************//
ESOS_USER_TASK(AVR_cmd)
{
    static  UCHAR temp1, temp2;
    static UCHAR buff[10];
    static int i;

    ESOS_TASK_BEGIN();
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			__esos_CB_ReadUINT8Buffer(__pstSelf->pst_Mailbox->pst_CBuffer,buff,5);

			ESOS_TASK_WAIT_SEMAPHORE(lcd_sem,1);

	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();

			for(i = 0;i < 5;i++)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT82(buff[i]);
			}

			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			for(i = 0;i < 5;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT8(buff[i]+0x30);

			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/		
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** main_proc ****************************************//
//******************************************************************************************//
// simulate the key presses
//#if 0
ESOS_USER_TASK(main_proc)
{
	static UCHAR data2 = 0x55;
	static UCHAR cmd, param;
	static UINT data3;
	static UCHAR data4;
    static ESOS_TASK_HANDLE fpga_handle;
    static ESOS_TASK_HANDLE menu_handle;
    static ESOS_TASK_HANDLE rt_handle;
    static int i,j,k;
    static UCHAR row, col, str;
    static UCHAR mode, type;
    static int retry_counter;

	avr_handle = esos_GetTaskHandle(AVR_cmd);
	fpga_handle = esos_GetTaskHandle(send_fpga);
	menu_handle = esos_GetTaskHandle(display_menu);
	rt_handle = esos_GetTaskHandle(display_rtlabels);

    ESOS_TASK_BEGIN();

	for(row = 8,col = 0,str = 0;str < 14;str++)
	{
		rtlabel_str[str].str = str + 10;
		rtlabel_str[str].row = row;
		rtlabel_str[str].col = col;
		rtlabel_str[str].onoff = 0;
		if(++row > 15)
		{
			row = 9;
			col += 20;
		}
	}

	for(row = 0,col = 2,str = 0;str < 6;str++)
	{
		menu_str[str].str = str + 5;
		menu_str[str].row = row;
		menu_str[str].col = col;
		menu_str[str].onoff = 0;
		if(++row > 5)
		{
			row = 12;
			col += 19;
		}
	}

	row = col = 0;
	i = 0;
	j = 0;
	k = 0x1f;

	data3 = FPGA_LCD_PWM;
	data3 <<= 8;
	data3 &= 0xFF00;
//	data3 |= PWM_ON_PARAM;
//	data3 |= PWM_50DC_PARAM;
//	data3 |= PWM_75DC_PARAM;
	data3 |= PWM_80DC_PARAM;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

	avr_buffer[0] = GOTO_CMD;
	avr_buffer[1] = 0;
	avr_buffer[2] = 0;
	AVR_CALL();

	avr_buffer[0] = LCD_CLRSCR;
	avr_buffer[1] = 0;
	AVR_CALL();
/*
	ESOS_TASK_WAIT_TICKS(100);
	__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,data4);

	ESOS_TASK_WAIT_TICKS(100);
	__esos_CB_WriteUINT8(rt_handle->pst_Mailbox->pst_CBuffer,data4);
*/
	menu_ptr = 0;
    password_valid = 0;
    password_ptr = 0;
	password_retries = 0;
	key_mode = PASSWORD;
//	key_mode = NORMAL;
	strcpy(correct_password,"2354795\0");
	memset(password,0,PASSWORD_SIZE);
	i = 0;
	j = 0;
	k = 0;

//	ESOS_TASK_WAIT_TICKS(500);
/*
	avr_buffer[0] = PUT_STR;
	avr_buffer[1] = 0;	// row 7
	avr_buffer[2] = 0;	// col 0
	avr_buffer[3] = 5;	// string 1 ("enter password")
	AVR_CALL();
*/
	data2 = 0x21;	
	while(1)
	{
		ESOS_TASK_WAIT_TICKS(1000);
/*
		if(password_valid == 0)
		{
			key_mode = PASSWORD;
		}
*/
		if(password_retries > PASSWORD_RETRIES && ESOS_IS_TASK_SLEEPING(password_task_handle))
		{
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(retry_counter);
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
			retry_counter++;
			ESOS_TASK_WAIT_TICKS(10000);

			if(retry_counter > 60)
			{
				retry_counter = 0;
				password_retries = 0;
				ESOS_WAKE_TASK(password_task_handle);
			}
		}
	}
    ESOS_TASK_END();
}
/*
	{"enter password:"},
	{"bad password"},
	{"start engine"},
	{"retries left"},
	{"enable starter"},
	{"ignition"},
	{"fuel pump"},
	{"cooling fan"},
	{"running lights"},
	{"headlights"},
	{"RPM"},
	{"MPH"},
	{"TIME"},
	{"ODOM"},
	{"TRIP"},
	{"ENG TEMP"},
	{"AIR TEMP"},
	{"OIL PRES"},
	{"MAP"},
	{"OIL TEMP"},
	{"O2"},
	{"AUX1"},
	{"AUX2"}
*/
//******************************************************************************************//
//*************************************** user_init  ***************************************//
//******************************************************************************************//
void user_init(void)
{
// can't use SPI for now
//	CONFIG_SPI_SLAVE();
//	esos_RegisterTask(spi_task);

	ESOS_INIT_SEMAPHORE(key_sem,1);
	ESOS_INIT_SEMAPHORE(lcd_sem,1);
	esos_RegisterTask(keypad);
	esos_RegisterTask(poll_keypad);
	esos_RegisterTask(AVR_cmd);
	esos_RegisterTask(send_fpga);
	esos_RegisterTask(recv_fpga);
	esos_RegisterTask(recv_lcd);
	esos_RegisterTask(main_proc);
	esos_RegisterTask(convADC);
	esos_RegisterTask(send_comm1);
	esos_RegisterTask(recv_comm1);
	esos_RegisterTask(menu_task);
	esos_RegisterTask(password_task);
	esos_RegisterTask(numentry_task);
	esos_RegisterTask(display_menu);
	esos_RegisterTask(display_rtlabels);
} // end user_init()

