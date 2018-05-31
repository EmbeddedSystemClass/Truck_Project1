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
	static UCHAR data1;
	static UCHAR data2 = 0x21;
	static UINT data3;
    static ESOS_TASK_HANDLE send_handle;
    static ESOS_TASK_HANDLE goto_handle;
    static ESOS_TASK_HANDLE clrscr_handle;
    static int i,j;
    static UCHAR buff[5];
    static UCHAR row, col;
//    static ESOS_TASK_HANDLE handle2;

	send_handle = esos_GetTaskHandle(send_char);
	goto_handle = esos_GetTaskHandle(goto1);
	clrscr_handle = esos_GetTaskHandle(clr_screen);
//	handle1 = esos_GetTaskHandle(send_string);
    ESOS_TASK_BEGIN();

	data1 = 0x21;
	row = col = 0;
	j = 0;

	while(1)
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

		__esos_CB_WriteUINT8(send_handle->pst_Mailbox->pst_CBuffer,data2);

		if(++i > 642)
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

/*
		data3 = (UINT)row;
		data3 <<= 8;
		data3 |= (UINT)col;
		ESOS_TASK_WAIT_TICKS(1);
		__esos_CB_WriteUINT16(handle2->pst_Mailbox->pst_CBuffer,data3);

		if(++col > COLUMN-1)
		{
			col = 0;
			if(++row > ROWS-1)
			{
				row = 0;
			}
		}
		buff[1] = row;
		buff[2] = col;

		if(++i > 639)
		{
			ESOS_TASK_WAIT_TICKS(500);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(DEBUG_CLRSCR3);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
			row = col = 0;
			i = 0;
		}

 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		ESOS_TASK_WAIT_TICKS(2);

//	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(dim);
//			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		if(data2 > 0x2F && data2 < 0x7B)
		{
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}else
		{
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}

		data2++;
		if(data2 > 0x7e)
			data2 = 0x21;

		ESOS_TASK_WAIT_TICKS(10);
*/
	}
	
    ESOS_TASK_END();
}
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

	esos_RegisterTask(send_char);
	esos_RegisterTask(set_cursor);
	esos_RegisterTask(goto1);
	esos_RegisterTask(clr_screen);

//	esos_RegisterTask(test1);
//	esos_RegisterTask(convADC);
//	esos_RegisterTask(dimmer_task);
	
} // end user_init()



