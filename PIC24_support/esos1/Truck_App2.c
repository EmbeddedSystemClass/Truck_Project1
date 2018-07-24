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
    data2 = 0x21;
    i = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(0x7E);
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
//			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
/*
			for(i = 0;i < NUM_CHANNELS;i++)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8(u16_pot[i]>>8);
				ESOS_TASK_WAIT_ON_SEND_UINT8(u16_pot[i]);
				ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			}
			ESOS_TASK_WAIT_TICKS(10);
*/
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
    static UCHAR buffer[50];
    static UCHAR temp_buffer[50];
    static ESOS_TASK_HANDLE avr_handle;
	avr_handle = esos_GetTaskHandle(AVR_cmd);
	
    ESOS_TASK_BEGIN();
    i = 0;

	buffer[0] = CHAR_CMD;

    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		if(data1 == 0xFC)
		{	
			buffer[0] = LCD_CLRSCR;
			buffer[1] = 1;
			AVR_CALL();
			buffer[0] = PUT_STR;
			buffer[1] = 7;	// row 7
			buffer[2] = 0;	// col 0
			buffer[3] = 1;	// string 1 ("enter password")
			AVR_CALL();
			buffer[0] = GOTO_CMD;
			buffer[1] = 7;
			buffer[2] = 14;
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
/*
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
    static UCHAR buffer[6];
	curr_num_ptr = 0;
    static ESOS_TASK_HANDLE avr_handle;
	avr_handle = esos_GetTaskHandle(AVR_cmd);

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
    static ESOS_TASK_HANDLE avr_handle;
    static ESOS_TASK_HANDLE comm1_handle;
    static int i,j,k;
    static UCHAR row, col;
    static UCHAR mode, type;
    static int retry_counter;

	avr_handle = esos_GetTaskHandle(AVR_cmd);
	fpga_handle = esos_GetTaskHandle(send_fpga);
	comm1_handle = esos_GetTaskHandle(send_comm1);
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
//	data3 |= PWM_50DC_PARAM;
//	data3 |= PWM_75DC_PARAM;
//	data3 |= PWM_80DC_PARAM;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

    password_valid = 0;
    password_ptr = 0;
	password_retries = 0;
	key_mode = PASSWORD;
//	key_mode = EEPROM;
//	key_mode = NORMAL;
	strcpy(correct_password,"2354795\0");
	memset(password,0,sizeof(password));
	i = 0;
	j = 0;
	k = 0;

//	ESOS_TASK_WAIT_TICKS(500);

	buffer[0] = PUT_STR;
	buffer[1] = 0;	// row 7
	buffer[2] = 0;	// col 0
	buffer[3] = 5;	// string 1 ("enter password")
	AVR_CALL();

	buffer[0] = LCD_CLRSCR;
	buffer[1] = 0;
	AVR_CALL();

	data2 = 0x21;	
	while(1)
	{
		ESOS_TASK_WAIT_TICKS(1000);
//		__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,data2);

/*
 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8(data2++);
		if(++i > 40)
		{
			i = 0;
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
		}
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		if(++data2 > 0x7e)
			data2 = 0x21;
#if 0
		buffer[0] = PUT_STRING;
		buffer[1] = j;
		buffer[2] = k;
		buffer[3] = i;
		AVR_CALL();

		if(++i > 22)
			i = 1;
		if(++j > 15)
		{
			j = 0;
			if(k == 20)
			{
				buffer[0] = LCD_CLRSCR;
				buffer[1] = 0;
				AVR_CALL();
				k = 0;
			}else k = 20;
		}
#endif
//#if 0
		if(password_valid == 0)
		{
			key_mode = PASSWORD;
		}

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
//#endif
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

	ESOS_INIT_SEMAPHORE(key_sem,1);
	ESOS_INIT_SEMAPHORE(lcd_sem,1);
	esos_RegisterTask(keypad);
	esos_RegisterTask(poll_keypad);
/*
	esos_RegisterTask(send_char);
	esos_RegisterTask(put_str);
	esos_RegisterTask(set_cursor);
	esos_RegisterTask(cursor_blink_on);
	esos_RegisterTask(clr_screen);
	esos_RegisterTask(get_curr_num);
*/
	esos_RegisterTask(AVR_cmd);
	esos_RegisterTask(send_fpga);
	esos_RegisterTask(recv_fpga);
	esos_RegisterTask(recv_lcd);
	esos_RegisterTask(main_proc);
	esos_RegisterTask(convADC);
	esos_RegisterTask(send_comm1);
	esos_RegisterTask(recv_comm1);
	esos_RegisterTask(eeprom_task);
	esos_RegisterTask(eeprom_cmd);
	esos_RegisterTask(menu_task);
	esos_RegisterTask(password_task);
	esos_RegisterTask(numentry_task);
} // end user_init()

