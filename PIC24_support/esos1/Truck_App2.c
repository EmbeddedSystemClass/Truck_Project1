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
#include "Truck_App.h"

//******************************************************************************************//
//*************************************** sendFPGA *****************************************//
//******************************************************************************************//
ESOS_USER_TASK(sendFPGA)
{
    static  uint8_t param1, param2, param3, param4, cmd1;
	static  uint16_t cmd_param;
    static ESOS_TASK_HANDLE cmd_param_task;

    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(send_cmd_param);

// blue - mph
// red - rpm
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0x7F;
   	cmd1 = 7;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0x7F;
   	cmd1 = 8;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0x7F;
   	cmd1 = 12;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0x7F;
   	cmd1 = 13;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

#if 0
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0xFF;
   	cmd1 = 7;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0xFF;
   	cmd1 = 8;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0xFF;
   	cmd1 = 12;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);
	param2 = 0xFF;
   	cmd1 = 13;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param2;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY2*4);
#endif

    param1 = 0;
    param2 = 0;
    param3 = 0xaa;
    param4 = 0x55;

    while (1)
    {
//	    ESOS_TASK_WAIT_SEMAPHORE(send_sem,1);

	   	cmd1 = 1;
		cmd_param = (uint16_t)(cmd1 << 8);
		cmd_param |= (uint16_t)param1;
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

		ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

		cmd1 = 2;
		cmd_param = (uint16_t)(cmd1 << 8);
		cmd_param |= (uint16_t)param2;
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

		ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

/*
	   	cmd1 = 13;
		cmd_param = (uint16_t)(cmd1 << 8);
		cmd_param |= (uint16_t)param3;
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

		ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

		cmd1 = 8;
		cmd_param = (uint16_t)(cmd1 << 8);
		cmd_param |= (uint16_t)param4;
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
*/
		ESOS_TASK_WAIT_TICKS(TEMP_DELAY2);

		param1 += 10;
		param2 += 10;
		param3 += 1;
		param4 += 1;
    } // endof while()
    ESOS_TASK_END();
}

//******************************************************************************************//
//*************************************** sendFPGA2 *****************************************//
//******************************************************************************************//
ESOS_USER_TASK(sendFPGA2)
{
    static  uint8_t param, cmd1;
	static  uint8_t cmd[7];
	static  uint8_t i;
	static  uint16_t cmd_param;
    static ESOS_TASK_HANDLE cmd_param_task;

    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(send_cmd_param);

    param = 0;
    i = 0;
    cmd[0] = 1;
    cmd[1] = 2;
    cmd[2] = 7;
    cmd[3] = 8;
    cmd[4] = 9;
    cmd[5] = 12;
    cmd[6] = 13;

   	cmd1 = 1;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY2);

   	cmd1 = 2;
	cmd_param = (uint16_t)(cmd1 << 8);
	cmd_param |= (uint16_t)param;
	__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);
	ESOS_TASK_WAIT_TICKS(TEMP_DELAY);

	param = 0xFF;
    while (1)
    {
    	cmd1 = cmd[i];
//    	cmd_param = (uint16_t)(cmd1 << 8) | (uint16_t)param;
		cmd_param = (uint16_t)(cmd1 << 8);
		cmd_param |= (uint16_t)param;
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,cmd_param);

		ESOS_TASK_WAIT_TICKS(TEMP_DELAY2);
	    ESOS_SIGNAL_SEMAPHORE(send_sem,1);

		if(++i > 2)
			i = 0;

		param -= 50;
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** recvFPGA  ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(recvFPGA)
{
    static  uint8_t data1;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_WAIT_ON_SEND_STRING("starting...");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    while (1)
    {
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();	// get byte from FPGA
        ESOS_TASK_WAIT_ON_GET_UINT82(data1);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		if(data1 == 0xFF)
		{
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
//			count = 0;
		}
//		if((data1 & 0x03) > 4)
		if(data1 == 0xFE || data1 == 0xFD || data1 == 0xFC)
			ESOS_TASK_WAIT_ON_SEND_UINT8(' ');

	    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
	    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************* data_from_AVR **************************************//
//******************************************************************************************//
// test the LCD for receiving data (later this will be valid telemetry data from the FPGA)
ESOS_USER_TASK(data_from_AVR)
{
    static  uint8_t data = 2;
    static  uint8_t data1 = 0;
    static  uint8_t skip = 1;
	static  uint16_t data2 = 0;
    static  uint8_t code = RT_TRIP;
//    static  uint8_t code = RT_RPM;

    ESOS_TASK_BEGIN();
//    h_Task3 = esos_GetTaskHandle(task3);
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(0x41);
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    while (1)
    {
		while(TRUE)
        {
			code++;
            ESOS_TASK_WAIT_TICKS(100);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
            ESOS_TASK_WAIT_ON_SEND_UINT83(code);
            ESOS_TASK_WAIT_TICKS(2);
			if(code == RT_RPM)
			{
				if(data2 & 0x8000)
				{
				    ESOS_TASK_WAIT_ON_SEND_UINT83(RT_HIGH3);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2);
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2>>8);
					data1 &= 0x7f;
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);
				}
				else if(data2 & 0x0080)
				{
				    ESOS_TASK_WAIT_ON_SEND_UINT83(RT_HIGH2);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)data2;
					data1 &= 0x7f;
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2>>8);
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);
				}
				else
				{
				    ESOS_TASK_WAIT_ON_SEND_UINT83(RT_HIGH1);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2);
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2>>8);
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);

				}
/*
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		        ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
		        ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(~code);
		        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
				if(data2 > 6000)
					data2 = 0;
				data2++;
				code = RT_TRIP-1;
//				code = RT_OILP;
			}
			else
			{
				if(data > 0x7f)
				{
				    ESOS_TASK_WAIT_ON_SEND_UINT83(RT_HIGH0);
				    ESOS_TASK_WAIT_TICKS(2);
					data1 = data & 0x7f;
				    ESOS_TASK_WAIT_ON_SEND_UINT83(data1);
				}
				else
				{
				    ESOS_TASK_WAIT_ON_SEND_UINT83(RT_LOW);
				    ESOS_TASK_WAIT_TICKS(2);
			        ESOS_TASK_WAIT_ON_SEND_UINT83(data);
				}
				data++;
			}
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
//            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(~code);
            ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
			if(code == RT_RPM)
			{
	            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)data2);
	            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(data2>>8));
	            ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
			}
			else
	            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif

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
	CONFIG_SPI_SLAVE();
//	CONFIG_SPI2();
//	esos_RegisterTask(echo_spi_task);
	esos_RegisterTask(echo_spi_task);
/*
    CONFIG_KEYPAD();
	ESOS_INIT_SEMAPHORE(key_sem,0);
	ESOS_INIT_SEMAPHORE(send_sem,0);

	esos_RegisterTask(keyscan_col);
	esos_RegisterTask(keyscan_row);
	esos_RegisterTask(getkey_task);
	esos_RegisterTask(recvFPGA);
	esos_RegisterTask(sendFPGA);
	esos_RegisterTask(sendFPGA2);
	esos_RegisterTask(send_cmd_param);
*/
//	esos_RegisterTask(comm3_task);
//	esos_RegisterTask(comm1_task);
//	esos_RegisterTask(convADC);
} // end user_init()


