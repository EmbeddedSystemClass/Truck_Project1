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
//*************************************** get_comm1  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(get_comm1)	// simulate the keypad
{
    static  uint8_t data1;
//    static ESOS_TASK_HANDLE handle1;
    static ESOS_TASK_HANDLE handle2;

//	handle1 = esos_GetTaskHandle(send_comm1);

//	handle2 = esos_GetTaskHandle(send_comm2);

    ESOS_TASK_BEGIN();
	data1 = 0x21;
    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_TICKS(1);
//		__esos_CB_WriteUINT8(handle1->pst_Mailbox->pst_CBuffer,data1);
		ESOS_TASK_WAIT_TICKS(1);
//		__esos_CB_WriteUINT8(handle2->pst_Mailbox->pst_CBuffer,data1);
		ESOS_TASK_WAIT_TICKS(1);
		if(++data1 > 0x7e)
			data1 = 0x21;
    } // endof while()
    ESOS_TASK_END();
}
/*
#define CHAR_AT_CMD			0
#define STRING_AT_CMD		1
#define SET_CURSOR_CMD		2
#define SET_MODE_CMD 		3
*/
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
			ESOS_TASK_WAIT_ON_SEND_UINT82(SET_CURSOR_CMD);
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
//************************************ send_string  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(send_string)
{
    static  UCHAR row, col, str_len, data2;
    static char test_str[5][20];
	static int str_ptr;
	int i;
	
	strcpy(test_str[0],"test 123\0");
	strcpy(test_str[1],"hello xyz\0");
	strcpy(test_str[2],"what's up\0");
	strcpy(test_str[3],"going south\0");
	strcpy(test_str[4],"good bye!\0");
	
    ESOS_TASK_BEGIN();
    row = col = 0;
    i = 0;
	str_len = 0;
	str_ptr = 0;
	    
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
						
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(STRING_AT_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(row);
			ESOS_TASK_WAIT_ON_SEND_UINT82(col);
			str_len = (UCHAR)strlen(test_str[str_ptr]);
			ESOS_TASK_WAIT_ON_SEND_UINT82(str_len);
			for(i = 0;i < str_len;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT82(test_str[str_ptr][i]);

			if(++str_ptr > 4)
				str_ptr = 0;
			
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
			if((col += COLUMN/3) > COLUMN-5)
			{
				col = 0;
				if(++row > ROWS-1)
				{
					row = 0;
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
					ESOS_TASK_WAIT_ON_SEND_UINT82(DEBUG_CLRSCR3);
					ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
				}
			}
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
			ESOS_TASK_WAIT_ON_SEND_UINT82(CHAR_AT_CMD);
			ESOS_TASK_WAIT_ON_SEND_UINT82(row);
			ESOS_TASK_WAIT_ON_SEND_UINT82(col);
			ESOS_TASK_WAIT_ON_SEND_UINT82(data2);
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
			if(++col > COLUMN-1)
			{
				col = 0;
				if(++row > ROWS-1)
				{
					row = 0;
				}
			}
		}
    } // endof while()
    ESOS_TASK_END();
}
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


		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    ESOS_TASK_WAIT_ON_SEND_STRING("reset");
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** delay_comm1 ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(delay_comm1)
{
	static UCHAR test;
	ESOS_TASK_BEGIN();
	test = 0;
	while(TRUE)
	{
		ESOS_TASK_WAIT_TICKS(6);
		ESOS_SIGNAL_SEMAPHORE(comm1_sem,1);
		test++;
	}
	ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** send_comm1  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(send_comm1)
{
	static ULONG send1;
	static ULONG send2;
	static UCHAR temp1, temp2, temp3, temp4;
	
    ESOS_TASK_BEGIN();
	while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
		{
//			ESOS_TASK_WAIT_SEMAPHORE(comm1_sem,1);
			
			temp1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
/*
			send2 = send1;
			temp1 = (UCHAR)send1;
			send2 >>= 24;
			temp2 = (UCHAR)send2;
			send2 = send1;
			send2 >>= 16;
			temp3 = (UCHAR)send2;
			send2 = send1;
			send2 >>= 8;
			temp4 = (UCHAR)send2;
*/			
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(temp1);
/*
			ESOS_TASK_WAIT_ON_SEND_UINT8(temp2);
			ESOS_TASK_WAIT_ON_SEND_UINT8(temp3);
			ESOS_TASK_WAIT_ON_SEND_UINT8(temp4);
			temp4 = 0xff;
			ESOS_TASK_WAIT_ON_SEND_UINT8(temp4);
*/
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
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
	static UCHAR data1 = 0x21;
	static UCHAR data2;
	static char test_str[10];
//    static ESOS_TASK_HANDLE handle1;
//    static ESOS_TASK_HANDLE handle2;

//	handle1 = esos_GetTaskHandle(send_char);
//	handle2 = esos_GetTaskHandle(send_string);

    ESOS_TASK_BEGIN();
//	CONFIG_R0_DIG_OUTPUT();
	// RA0 - p17 - 5th up - outside right				//	col 0

	while(1)
	{
/*
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_TICKS(1);
*/
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

//		ESOS_TASK_WAIT_TICKS(1);
/*
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
*/
		ESOS_TASK_WAIT_TICKS(1);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//		data1 = data2;
/*
		data1++;
		if(data1 > 0x7e)
			data1 = 0x21;
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

//	ESOS_INIT_SEMAPHORE(key_sem,0);
//	ESOS_INIT_SEMAPHORE(comm1_sem,1);
//	ESOS_INIT_SEMAPHORE(comm2_sem,0);
//	esos_RegisterTask(keypad);
//	esos_RegisterTask(poll_keypad);
/*
	esos_RegisterTask(send_char);
	esos_RegisterTask(send_string);
	esos_RegisterTask(set_cursor);
	esos_RegisterTask(get_comm1);
	esos_RegisterTask(send_comm1);
//	esos_RegisterTask(delay_comm1);
*/
	esos_RegisterTask(test1);
//	esos_RegisterTask(convADC);
} // end user_init()



