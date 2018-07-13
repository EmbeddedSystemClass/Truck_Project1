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

 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
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
//*************************************** send_comm1 ***************************************//
//******************************************************************************************//
// recv data from FPGA
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
		}
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_TICKS(1000);

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
	
    ESOS_TASK_BEGIN();
    i = 0;
    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
		ESOS_TASK_WAIT_ON_GET_UINT82(data2);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();
#if 0
 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
//		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
		if(++i > 20)
		{
			i = 0;
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
		}
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
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
//************************************** set_cursor2 ***************************************//
//******************************************************************************************//
// send only the row/col as a UINT
ESOS_USER_TASK(set_cursor2)
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
			ESOS_TASK_WAIT_ON_SEND_UINT82(LCD_CLRSCR3);
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
	static UCHAR data2 = 0x55;
	static UCHAR cmd, param;
	static UINT data3;
    static ESOS_TASK_HANDLE send_handle;
    static ESOS_TASK_HANDLE goto_handle;
    static ESOS_TASK_HANDLE clrscr_handle;
    static ESOS_TASK_HANDLE fpga_handle;
    static ESOS_TASK_HANDLE mode_handle;
    static int i,j,k;
    static UCHAR row, col;
    static UCHAR mode, type;

	send_handle = esos_GetTaskHandle(send_char);
	goto_handle = esos_GetTaskHandle(goto1);
	clrscr_handle = esos_GetTaskHandle(clr_screen);
	fpga_handle = esos_GetTaskHandle(send_fpga);
	mode_handle = esos_GetTaskHandle(set_cursor2);
	UCHAR buffer[5];

    ESOS_TASK_BEGIN();

	row = col = 0;
	i = 0;
	j = 0;
	k = 0x1f;

	ESOS_TASK_WAIT_TICKS(1000);
	data3 = FPGA_LCD_PWM;
	data3 <<= 8;
	data3 &= 0xFF00;
	data3 |= PWM_ON_PARAM;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

	__esos_CB_WriteUINT8(clrscr_handle->pst_Mailbox->pst_CBuffer,data2);

	data3 = (UINT)row;
	data3 <<= 8;
	data3 |= (UINT)col;
	__esos_CB_WriteUINT16(goto_handle->pst_Mailbox->pst_CBuffer,data3);
/*
	mode = TEXT_ON | CURSOR_BLINK_ON;
	buffer[0] = mode;
	buffer[1] = 2; 		// row
	buffer[2] = 3; 		// col
	buffer[3] = LINE_8_CURSOR;
	__esos_CB_WriteUINT8Buffer(mode_handle->pst_Mailbox->pst_CBuffer, &buffer, 4 );
*/
	ESOS_TASK_WAIT_TICKS(1000);

	while(1)
	{
		data2++;

		if(data2 > 0x7e)
			data2 = 0x21;

		ESOS_TASK_WAIT_TICKS(2);

//		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
//		ESOS_TASK_WAIT_ON_GET_UINT8(data2);
//		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

//		__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data2);
		__esos_CB_WriteUINT8(send_handle->pst_Mailbox->pst_CBuffer,data2);

/*
		if(++j > 1200)
		{
			j = 0;
			data3 = FPGA_LCD_PWM;
			data3 <<= 8;
			data3 |= 0x1f;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}
*/
		if(++i > 639)
		{
			i = 0;
/*
			mode = TEXT_ON | CURSOR_BLINK_ON;
//			mode = TEXT_ON | CURSOR_ON_BLINK_OFF;
//			mode = ATTR_BLINK_REVERSE;
//			mode = ATTR_REVERSE;
			buffer[0] = mode;
			buffer[1] = row; 		// row
			buffer[2] = col; 		// col
			buffer[3] = LINE_4_CURSOR;
			__esos_CB_WriteUINT8Buffer(mode_handle->pst_Mailbox->pst_CBuffer, &buffer, 4 );
*/
			// with clrscr it doesn't matter what the param is
			__esos_CB_WriteUINT8(clrscr_handle->pst_Mailbox->pst_CBuffer,data2);
			data3 = (UINT)row;
			data3 <<= 8;
			data3 |= (UINT)col;
			// send the row/col with the goto cmd
			__esos_CB_WriteUINT16(goto_handle->pst_Mailbox->pst_CBuffer,data3);
			__esos_CB_WriteUINT16(mode_handle->pst_Mailbox->pst_CBuffer,data3);
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
	esos_RegisterTask(set_cursor2);
	esos_RegisterTask(goto1);
	esos_RegisterTask(clr_screen);
	esos_RegisterTask(send_fpga);
	esos_RegisterTask(recv_fpga);
//	esos_RegisterTask(recv_lcd);
	esos_RegisterTask(test1);
	esos_RegisterTask(convADC);
//	esos_RegisterTask(send_comm1);
//	esos_RegisterTask(menu_task);
	
} // end user_init()

