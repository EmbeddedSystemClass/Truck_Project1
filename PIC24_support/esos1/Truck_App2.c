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
//************************************** data_to_AVR ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(data_to_AVR)
{
    static  uint8_t data = 2;
    static  uint8_t data1 = 0;
    static  uint8_t skip = 1;
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

	code = RT_RPM;
	code2 = code - RT_RPM;
	auxcmd = 0;
	auxparam = 0;
	temp_int2 = 0;
	memset((void*)pic_data,0,AUX_DATA_SIZE);
	memset((void*)pic_data2,0,AUX_DATA_SIZE);

	while(TRUE)
    {
		code++;
        ESOS_TASK_WAIT_TICKS(100);
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
        ESOS_TASK_WAIT_ON_SEND_UINT82(code);
        ESOS_TASK_WAIT_TICKS(2);
		if(P24_rt_params[code2].shown == NOSHOWN_SENT || P24_rt_params[code2].shown == SHOWN_SENT)
		{
			if(P24_rt_params[code2].dtype > 0)
			{
				data2 = rtdata[code2];
				if(data2 & 0x8000)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT82(RT_HIGH3);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2);
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2>>8);
					data1 &= 0x7f;
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
				}
				else if(data2 & 0x0080)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT82(RT_HIGH2);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)data2;
					data1 &= 0x7f;
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2>>8);
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
				}
				else
				{
					ESOS_TASK_WAIT_ON_SEND_UINT82(RT_HIGH1);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2);
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = (UCHAR)(data2>>8);
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);

				}

				if(code2 != RT_AUX1-RT_OFFSET && code2 != RT_AUX2-RT_OFFSET)
				{
					data2 += 10;	// rpm get inc'd by 10
				}
				rtdata[code2] = data2;
			}
			else if(P24_rt_params[code2].dtype == 0)
			{
				data = (UCHAR)rtdata[code2];
				if(data > 0x7f)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT82(RT_HIGH0);
					ESOS_TASK_WAIT_TICKS(2);
					data1 = data & 0x7f;
					ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
				}
				else
				{
					ESOS_TASK_WAIT_ON_SEND_UINT82(RT_LOW);
					ESOS_TASK_WAIT_TICKS(2);
				    ESOS_TASK_WAIT_ON_SEND_UINT82(data);
				}
				data++;
				rtdata[code2] = (uint16_t)data;
//				sprintf(param_string,"%4u",data);
			}	
		}
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		if(code == RT_AUX1)
		{
			
//			read(fd,&pic_data,AUX_DATA_SIZE);
//				mvwprintw(menu_win, display_offset+24, 2,"res: %d  ",res);
//				for(i = 0;i < AUX_DATA_SIZE;i++)
//					mvwprintw(menu_win, display_offset+25, 2+(i * 3),"%x  ",pic_data[i]);
//				mvwprintw(menu_win, display_offset+26, 2,"%x %x  ",pic_data[0],pic_data[1]);
//#if 0
			switch(paux_state)
			{
				case IDLE_AUX:
					auxcmd = auxparam = 0;
					if(pic_data[0] == CMD_GET_DATA)
					{
						paux_state = DATA_REQ;
						aux_index = pic_data[1];
					}
					else
						paux_state = IDLE_AUX;
					break;
										// write requested data to AVR to be modified
				case DATA_REQ:
					rtdata[code2+1] = sample_data[aux_index];		// this ends up in tempint2 of do_read()
//						mvwprintw(menu_win, display_offset+29, 2,"before: %d    ",rtdata[code2+1]);
					auxcmd = CMD_DATA_READY;
					auxparam = 0;
					paux_state = VALID_DATA;
					break;
				case VALID_DATA:
					auxcmd = auxparam = 1;
					if(pic_data[0] == CMD_NEW_DATA)
						paux_state = DATA_READY;
					else
					{
						paux_state = VALID_DATA;
//							loop = break_out_loop(loop, paux_state);
//							mvwprintw(menu_win, display_offset+24, 2,"loop: %d  ",loop);
					}
					break;
				case DATA_READY:
//						mvwprintw(menu_win, display_offset+24, 2,"             ");
					auxcmd = auxparam = 2;
//						for(i = 0;i < AUX_DATA_SIZE;i++)
//							mvwprintw(menu_win, display_offset+26, 2+(i * 3),"%x  ",pic_data[i]);
					paux_state = EXTRA;
					break;
				case EXTRA:
				default:
					paux_state = IDLE_AUX;
					break;
			}
//#endif
			disp_pstate(paux_state,tempx);
//				auxcmd = pic_data[0];
//				mvwprintw(menu_win, display_offset+25, 2,"aux_index: %x  ",aux_index);
//				mvwprintw(menu_win, display_offset+26, 2,"%s        ",tempx);
			disp_auxcmd(pic_data[0], tempx);
//				mvwprintw(menu_win, display_offset+26, 20,"%s        ",tempx);
//				mvwprintw(menu_win, display_offset+27, 2,"cmd: %x  param: %x  ",auxcmd,auxparam);
			temp_int = (uint16_t)auxcmd;
//				mvwprintw(menu_win, display_offset+34,2,"%x ",temp_int);
			temp_int <<= 8;
//				temp_int &= 0xff00;
//				mvwprintw(menu_win, display_offset+35,2,"%x ",temp_int);
			rtdata[code2] = temp_int;
			temp_int = (uint16_t)auxparam;
			rtdata[code2] |= temp_int;
//				mvwprintw(menu_win, display_offset+28,2,"temp_int: %x   ",temp_int);
			temp_int2 = (uint16_t)pic_data[2];
			temp_int2 <<= 8;
			temp_int2 |= (uint16_t)pic_data[3];
//				mvwprintw(menu_win, display_offset+33, 2,"temp_int2: %d  ",temp_int2);
		}
		else if(code == RT_AUX2)
		{
//			read(fd,&pic_data2,AUX_DATA_SIZE);
//				mvwprintw(menu_win, display_offset+31, 2,"res: %d  ",res);
//				for(i = 0;i < AUX_DATA_SIZE;i++)
//					mvwprintw(menu_win, display_offset+29, 2+(i * 3),"%x  ",pic_data2[i]);
//				temp1 = (uint16_t)pic_data2[0];
//				temp1 <<= 8;
//				temp1 |= (uint16_t)pic_data2[1];
			UCHAR test1x, test2x;
			uint16_t test1y;
			if(paux_state == DATA_READY)
			{
				test1x = pic_data2[2];
//					mvwprintw(menu_win, display_offset+31,2,"%x  ",test1x);
				test2x= pic_data2[3] << 1;
				test2x &= 0x80;
//					mvwprintw(menu_win, display_offset+31,6,"%x  ",test2x);
				test1y = (uint16_t)test1x;
				test1y <<= 8;
//					mvwprintw(menu_win, display_offset+31,10,"%x  ",test1y);
				test1y |= (uint16_t)(test2x);
//					mvwprintw(menu_win, display_offset+31,16,"%x  ",test1y);
				sample_data[aux_index] = (uint16_t)pic_data2[0];
				// this is a work-around for strange bug - for some reason the high bit of
				// the 2nd byte is getting unset
				sample_data[aux_index] <<= 8;
				pic_data2[1] |= test2x;
				sample_data[aux_index] |= (uint16_t)pic_data2[1];
//					mvwprintw(menu_win, display_offset+30, 2,"after:%d    %x %x %x %x %d   ",sample_data[aux_index],pic_data2[0],pic_data2[1],pic_data2[2],pic_data2[3],test1y);
			}

//				temp2 = (uint16_t)pic_data2[2];

//				temp2 <<= 8;
//				temp2 |= (uint16_t)pic_data2[3];
		}

//			for(i = 0;i < no_data_index;i++)
//				mvwprintw(menu_win, display_offset+34+i, 2,"%d  ",sample_data[i]);

		if(++code > RT_AUX2)
			code = RT_RPM;
		code2 = code - RT_RPM;	// use code2 as index - code starts at RT_OFFSET

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
/*
    CONFIG_KEYPAD();
	ESOS_INIT_SEMAPHORE(key_sem,0);
	ESOS_INIT_SEMAPHORE(send_sem,0);

	esos_RegisterTask(recvFPGA);
	esos_RegisterTask(sendFPGA);
	esos_RegisterTask(sendFPGA2);
	esos_RegisterTask(send_cmd_param);
*/
//	esos_RegisterTask(comm3_task);
	esos_RegisterTask(comm1_task);

//	esos_RegisterTask(keypad);
//	esos_RegisterTask(poll_keypad);
//	esos_RegisterTask(convADC);
} // end user_init()


