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
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(param);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(cmd);
			ESOS_TASK_WAIT_ON_SEND_UINT8(end_char);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
//	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
 //			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(cmd);
//			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
//			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(param);
//			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
//			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

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

 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
//		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
		if(++i > 20)
		{
			i = 0;
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
		}
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    } // endof while()
    ESOS_TASK_END();
}

//******************************************************************************************//
//***************************************** goto ******************************************//
//******************************************************************************************//
ESOS_USER_TASK(goto1)
{
    static  UCHAR temp1, temp2, temp4;
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
			temp4 = 0xfe;
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp4);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** set_cursor ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(set_cursor)
{
    static  UCHAR temp1, temp2, temp3, temp4;
    static UCHAR buff[5];
	
    ESOS_TASK_BEGIN();
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			__esos_CB_ReadUINT8Buffer(__pstSelf->pst_Mailbox->pst_CBuffer,buff,4);
			temp1 = buff[0];
			temp2 = buff[1];
			temp3 = buff[2];
			temp4 = buff[3];
			
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
//			ESOS_TASK_WAIT_ON_SEND_UINT82(SET_CURSOR_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp1);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp3);
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp4);
			temp4 = 0xfe;
			ESOS_TASK_WAIT_ON_SEND_UINT82(temp4);
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
		}
    } // endof while()
    ESOS_TASK_END();
}

ESOS_USER_TASK(test3)
{
    static  UCHAR data2;
    static UINT data3;
    static UCHAR index;
    static int i;
//    static ESOS_TASK_HANDLE send_handle;
	
//	send_handle = esos_GetTaskHandle(send_char);
    static ESOS_TASK_HANDLE fpga_handle;

	fpga_handle = esos_GetTaskHandle(send_fpga);

	// cmd in low byte and param in high    
    ESOS_TASK_BEGIN();
	data2 = 0;
	data3 = FPGA_DTMF_TONE_ON;
	index = 0;
	i = 0;

	ESOS_TASK_WAIT_TICKS(100);

    while (1)
    {
		data3 = FPGA_DTMF_TONE_ON;
//		data3 = FPGA_SPECIAL_TONE_ON;
		data3 <<= 8;
		data3 &= 0xFF00;
		data3 |= index;
		index++;

		if(index > 15)
			index = 0;

		__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

		ESOS_TASK_WAIT_TICKS(200);

		data3 = FPGA_DTMF_TONE_OFF;
		data3 <<= 8;
		data3 &= 0xFF00;

		__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

		ESOS_TASK_WAIT_TICKS(200);

    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** test2  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(test2)
{
    static  UCHAR data2;
    static UINT data3;
    static UCHAR index;
    static int i;
//    static ESOS_TASK_HANDLE send_handle;
	
//	send_handle = esos_GetTaskHandle(send_char);
    static ESOS_TASK_HANDLE fpga_handle;

	fpga_handle = esos_GetTaskHandle(send_fpga);

	// cmd in low byte and param in high    
    ESOS_TASK_BEGIN();
	data2 = 0;
	data3 = FPGA_DTMF_TONE_ON;
	index = 5;
	i = 0;

	ESOS_TASK_WAIT_TICKS(5000);

    while (1)
    {
		data3 = FPGA_SET_UPDATE_RATE_CMD;
		data3 <<= 8;
		data3 &= 0xFF00;
		data3 |= index;
		index += 10;

		if(index > 0x70)
			index = 20;

		__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

		ESOS_TASK_WAIT_TICKS(1000);
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
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(DEBUG_CLRSCR3);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//***************************************** test1  *****************************************//
//******************************************************************************************//
// simulate the key presses
//#if 0
ESOS_USER_TASK(test1)
{
	static UCHAR data2 = 0x21;
	static UCHAR cmd, param;
	static UINT data3;
    static ESOS_TASK_HANDLE send_handle;
    static ESOS_TASK_HANDLE goto_handle;
    static ESOS_TASK_HANDLE clrscr_handle;
    static ESOS_TASK_HANDLE fpga_handle;
    static int i,j,k;
    static UCHAR param_array[11];
//    static UCHAR buff[5];
    static UCHAR row, col;

	send_handle = esos_GetTaskHandle(send_char);
	goto_handle = esos_GetTaskHandle(goto1);
	clrscr_handle = esos_GetTaskHandle(clr_screen);
	fpga_handle = esos_GetTaskHandle(send_fpga);
//	handle1 = esos_GetTaskHandle(send_string);
    ESOS_TASK_BEGIN();

	row = col = 0;
	param_array[0] = PWM_80DC_PARAM;
	param_array[1] = PWM_75DC_PARAM;
	param_array[2] = PWM_60DC_PARAM;
	param_array[3] = PWM_50DC_PARAM;
	param_array[4] = PWM_50DC_PARAM;
	param_array[5] = PWM_30DC_PARAM;
	param_array[6] = PWM_25DC_PARAM;
	param_array[7] = PWM_12DC_PARAM;
//	param_array[8] = PWM_ON_PARAM;
	param_array[8] = PWM_OFF_PARAM;
	param_array[9] = PWM_ON_PARAM;
	param_array[10] = PWM_ON_PARAM;
	i = 0;
	j = 0;
	k = 0;
/*
	for(data2 = 0x21;data2 < 0x7e;data2++)
	{
		ESOS_TASK_WAIT_TICKS(200);
		__esos_CB_WriteUINT8(send_handle->pst_Mailbox->pst_CBuffer,data2);
	}

	ESOS_TASK_WAIT_TICKS(1000);
	__esos_CB_WriteUINT8(clrscr_handle->pst_Mailbox->pst_CBuffer,data2);
*/
	ESOS_TASK_WAIT_TICKS(1000);
	data3 = FPGA_LCD_PWM;
	data3 <<= 8;
	data3 &= 0xFF00;
	data3 |= PWM_OFF_PARAM;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

	while(1)
	{
/*
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
*/
// 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
//		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
//			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(param);
//			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
//		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		data2++;

		if(data2 > 0x7e)
			data2 = 0x21;

		ESOS_TASK_WAIT_TICKS(10);

//		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
//		ESOS_TASK_WAIT_ON_GET_UINT8(data2);
//		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

//		__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data2);
		__esos_CB_WriteUINT8(send_handle->pst_Mailbox->pst_CBuffer,data2);

		if(++j > 1200)
		{
			j = 0;
			data3 = FPGA_LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= param_array[k];
			if(++k > 10)
				k = 0;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}

		if(++i > 639)
		{
			i = 0;
			__esos_CB_WriteUINT8(clrscr_handle->pst_Mailbox->pst_CBuffer,data2);
			data3 = (UINT)row;
			data3 <<= 8;
			data3 |= (UINT)col;
			__esos_CB_WriteUINT16(goto_handle->pst_Mailbox->pst_CBuffer,data3);
			if(++col > 16)
			{
				col = 0;
				if(++row > 40)
					row = 0;
			}
		}			
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
	esos_RegisterTask(keypad);
	esos_RegisterTask(poll_keypad);

	esos_RegisterTask(send_char);
	esos_RegisterTask(set_cursor);
	esos_RegisterTask(goto1);
	esos_RegisterTask(clr_screen);
	esos_RegisterTask(send_fpga);
	esos_RegisterTask(recv_fpga);

	esos_RegisterTask(test1);
	esos_RegisterTask(test2);
	esos_RegisterTask(test3);
//	esos_RegisterTask(convADC);
	
} // end user_init()


/*
		ESOS_TASK_WAIT_TICKS(100);
		__esos_CB_WriteUINT8(handle2->pst_Mailbox->pst_CBuffer,data1+1);
*/
//#endif
#if 0
ESOS_USER_TASK(test1)
{
	static UCHAR mode, row, col, type;
	static char test_str[10];
	static UCHAR buff[5];
	static UINT size_buff;
//    static ESOS_TASK_HANDLE handle1;
    static ESOS_TASK_HANDLE handle2;

//	handle1 = esos_GetTaskHandle(send_comm1);
	handle2 = esos_GetTaskHandle(set_cursor);

    ESOS_TASK_BEGIN();
//	CONFIG_R0_DIG_OUTPUT();
	// RA0 - p17 - 5th up - outside right				//	col 0

	mode = 1;
	col = row = 0;
	type = 2;
	size_buff = 4;
	
	while(1)
	{
		memcpy(buff+0,&mode,sizeof(UCHAR));
		memcpy(buff+1,&row,sizeof(UCHAR));
		memcpy(buff+2,&col,sizeof(UCHAR));
		memcpy(buff+3,&type,sizeof(UCHAR));

		__esos_CB_WriteUINT8Buffer(handle2->pst_Mailbox->pst_CBuffer,buff,size_buff);

		if(++col > COLUMN-1)
		{
			col = 0;
			if(++row > ROWS-1)
			{
				row = 0;
			}
		}

		if(++mode > 4)
			mode = 0;
		if(++type > 5)
			type = 0;
		ESOS_TASK_WAIT_TICKS(100);
	}
	
    ESOS_TASK_END();
}
#endif

