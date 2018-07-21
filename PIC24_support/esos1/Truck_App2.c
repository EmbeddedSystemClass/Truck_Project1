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
//*************************************** send_comm1 ***************************************//
//******************************************************************************************//
// send data to TS-7200
ESOS_USER_TASK(send_comm1)
{
    static UCHAR data2;
    static int i;
	
    ESOS_TASK_BEGIN();
    i = 0;
    while (1)
    {
 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8(COMM_CMD);
		ESOS_TASK_WAIT_TICKS(1);

//		u16_pot[2] = 0x5A5A;
//		u16_pot[3] = 0xA5A5;

		for(i = 0;i < NUM_CHANNELS;i++)
		{
/*
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u16_pot[i]>>8);
			ESOS_TASK_WAIT_TICKS(1);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u16_pot[i]);
			ESOS_TASK_WAIT_TICKS(1);
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
*/
			ESOS_TASK_WAIT_ON_SEND_UINT8(u16_pot[i]>>8);
//			ESOS_TASK_WAIT_TICKS(1);
			ESOS_TASK_WAIT_ON_SEND_UINT8(u16_pot[i]);
//			ESOS_TASK_WAIT_TICKS(1);
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
		}
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_TICKS(1000);

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
    static UCHAR buff[50];
	static ESOS_TASK_HANDLE send_handle;
	
	send_handle = esos_GetTaskHandle(send_char);
	
    ESOS_TASK_BEGIN();
    i = 0;

    while (1)
    {
		do{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			buff[i] = data1;
			i++;
			if(i > 50)
				i = 0;
		}while(data1 != 0xFE);

		j = 0;
/*
		do{
			__esos_CB_WriteUINT8(send_handle->pst_Mailbox->pst_CBuffer,buff[j]);
			j++;
		}	
		while(j < i);
*/
		i = 0;
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

			if(curr_num > NUM_SIZE)
				curr_num_ptr = 0;
//#if 0
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	//		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
	//		ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//#endif
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//***************************************** goto ******************************************//
//******************************************************************************************//
ESOS_USER_TASK(goto1)
{
    static  UCHAR temp1, temp2;
    static UINT data2;
	
    ESOS_TASK_BEGIN();
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);
			temp2 = (UCHAR)data2;
			temp1 = (UCHAR)(data2 >> 8);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(GOTO_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp1);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** set_cursor ***************************************//
//******************************************************************************************//
// send mode, row, col, & type as 4 byte buffer
ESOS_USER_TASK(set_cursor)
{
    static UCHAR buff[5];
	
    ESOS_TASK_BEGIN();
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			__esos_CB_ReadUINT8Buffer(__pstSelf->pst_Mailbox->pst_CBuffer,buff,4);
			
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(SET_MODE_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[0]);
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[1]);
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[2]);
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[3]);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************ cursor_blink_on *************************************//
//******************************************************************************************//
// send only the row/col as a UINT to set the cursor to blink
ESOS_USER_TASK(cursor_blink_on)
{
    static UINT data2;
    static UCHAR temp1, temp2;
	
    ESOS_TASK_BEGIN();
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);
			temp2 = (UCHAR)data2;
			temp1 = (UCHAR)(data2 >> 8);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(SET_MODE_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(TEXT_ON | CURSOR_BLINK_ON);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp1);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(LINE_8_CURSOR);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** send_char  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(send_char)
{
    static  UCHAR row, col, data2;
	
    ESOS_TASK_BEGIN();
    row = col = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(CHAR_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	//		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//**************************************** put_str  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(put_str)
{
    static  UCHAR row, col, data2;
    static UCHAR buff[3];
	
    ESOS_TASK_BEGIN();
    row = col = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			ESOS_TASK_WAIT_SEMAPHORE(lcd_sem,1);

			__esos_CB_ReadUINT8Buffer(__pstSelf->pst_Mailbox->pst_CBuffer,buff,3);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(PUT_STRING);
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[0]);	// row
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[1]);	// col
			ESOS_TASK_WAIT_ON_SEND_UINT82(buff[2]);	// which string (starting w/ '1')
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buff[0]);	// row
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buff[1]);	// col
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buff[2]);	// which string (starting w/ '1')
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
*/
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** eeprom_cmd ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(eeprom_cmd)
{
    static  UCHAR row, col, data2;
	
    ESOS_TASK_BEGIN();
    row = col = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
/*
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		}
    } // endof while()
    ESOS_TASK_END();
}

//******************************************************************************************//
//************************************** clr_screen  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(clr_screen)
{
    static  UCHAR row, col, data2;
	
    ESOS_TASK_BEGIN();
    row = col = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			ESOS_TASK_WAIT_SEMAPHORE(lcd_sem,1);
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(LCD_CLRSCR);
			ESOS_TASK_WAIT_ON_SEND_UINT82(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************ get_curr_num  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(get_curr_num)
{
    static  UCHAR row, col, data2;
	
    ESOS_TASK_BEGIN();
    row = col = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(LCD_MSG1);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
#endif
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
    static ESOS_TASK_HANDLE send_handle;
    static ESOS_TASK_HANDLE goto_handle;
    static ESOS_TASK_HANDLE clrscr_handle;
    static ESOS_TASK_HANDLE fpga_handle;
    static ESOS_TASK_HANDLE mode_handle;
    static ESOS_TASK_HANDLE blink_handle;
    static ESOS_TASK_HANDLE num_handle;
	static ESOS_TASK_HANDLE string_handle;
    static int i,j,k;
    static UCHAR row, col;
    static UCHAR mode, type;
    static int retry_counter;

	send_handle = esos_GetTaskHandle(send_char);
	goto_handle = esos_GetTaskHandle(goto1);
	clrscr_handle = esos_GetTaskHandle(clr_screen);
	fpga_handle = esos_GetTaskHandle(send_fpga);
	blink_handle = esos_GetTaskHandle(cursor_blink_on);
	mode_handle = esos_GetTaskHandle(set_cursor);
	num_handle = esos_GetTaskHandle(get_curr_num);
	string_handle = esos_GetTaskHandle(put_str);
	UCHAR buffer[5];

    ESOS_TASK_BEGIN();

	row = col = 0;
	i = 0;
	j = 0;
	k = 0x1f;

	data3 = FPGA_LCD_PWM;
	data3 <<= 8;
	data3 &= 0xFF00;
	data3 |= PWM_ON_PARAM;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
//	__esos_CB_WriteUINT8(clrscr_handle->pst_Mailbox->pst_CBuffer,0);
//	__esos_CB_WriteUINT8(string_handle->pst_Mailbox->pst_CBuffer,0);

/*
	data3 = (UINT)row;
	data3 <<= 8;
	data3 |= (UINT)col;
	__esos_CB_WriteUINT16(goto_handle->pst_Mailbox->pst_CBuffer,data3);


	ESOS_TASK_WAIT_TICKS(1000);
	__esos_CB_WriteUINT8(num_handle->pst_Mailbox->pst_CBuffer,data2);
*/
    password_valid = 0;
    password_ptr = 0;
	password_retries = 0;
//	key_mode = PASSWORD;
//	key_mode = EEPROM;
	key_mode = NORMAL;
	strcpy(correct_password,"2354795\0");
	memset(password,0,sizeof(password));
	i = 0;
	j = 0;
	k = 0;

	data2 = 0x21;	
	while(1)
	{
		ESOS_TASK_WAIT_TICKS(50);

		buffer[0] = j;
		buffer[1] = k;	// col 0
		buffer[2] = i;	// string 1 ("enter password")
		
		__esos_CB_WriteUINT8Buffer(string_handle->pst_Mailbox->pst_CBuffer, &buffer, 3);
//		__esos_CB_WriteUINT8(send_handle->pst_Mailbox->pst_CBuffer,data2);
		if(++data2 > 0x7e)
			data2 = 0x21;

		if(++i > 22)
			i = 1;
		if(++j > 15)
		{
			j = 0;
			if(k == 20)
			{
				__esos_CB_WriteUINT8(clrscr_handle->pst_Mailbox->pst_CBuffer,0);
				k = 0;
			}else k = 20;
		}

#if 0
		if(password_valid == 0)
		{
			key_mode = PASSWORD;
		}

		if(password_retries > 2 && ESOS_IS_TASK_SLEEPING(password_task_handle))
		{
			retry_counter++;
			ESOS_TASK_WAIT_TICKS(10000);
			if(retry_counter > 60)
			{
				retry_counter = 0;
				password_retries = 0;
				ESOS_WAKE_TASK(password_task_handle);
			}
		}
#endif
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** user_init  ***************************************//
//******************************************************************************************//
void user_init(void)
{
// can't use SPI for now
//	CONFIG_SPI_SLAVE();
//	esos_RegisterTask(spi_task);

	ESOS_INIT_SEMAPHORE(key_sem,0);
	ESOS_INIT_SEMAPHORE(lcd_sem,2);
	esos_RegisterTask(keypad);
	esos_RegisterTask(poll_keypad);
	esos_RegisterTask(send_char);
	esos_RegisterTask(put_str);
	esos_RegisterTask(set_cursor);
	esos_RegisterTask(cursor_blink_on);
	esos_RegisterTask(goto1);
	esos_RegisterTask(clr_screen);
	esos_RegisterTask(send_fpga);
	esos_RegisterTask(recv_fpga);
	esos_RegisterTask(recv_lcd);
	esos_RegisterTask(main_proc);
	esos_RegisterTask(convADC);
	esos_RegisterTask(get_curr_num);
//	esos_RegisterTask(send_comm1);
	esos_RegisterTask(recv_comm1);
	esos_RegisterTask(eeprom_task);
	esos_RegisterTask(eeprom_cmd);
	esos_RegisterTask(menu_task);
	esos_RegisterTask(password_task);
	esos_RegisterTask(numentry_task);
} // end user_init()
