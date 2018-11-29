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

#include "Truck_App.h"
/*
can't call ESOS_TASK_WAIT_TICKS from within mail recipient?

note: don't do any assignment statements before ESOS_TASK_BEGIN()
it causes strange side-effects
*/
//******************************************************************************************//
//************************************ display_rtvalues ************************************//
//******************************************************************************************//
ESOS_USER_TASK(display_rtvalues)
{
//	static ESOS_TASK_HANDLE comm1_handle;
    static int i;
    static UCHAR data1;

    ESOS_TASK_BEGIN();

	data1 = 0x21;
//	comm1_handle = esos_GetTaskHandle(send_comm1);

	while(1)
	{
		if(key_mode == NORMAL)
		{
			// engine temp doesn't come from ADC anymore
			// 
			for(i = ENG_TEMP + 1;i < O2+1;i++)
			{
				avr_buffer[0] = SEND_BYTE_RT_VALUES;
				avr_buffer[1] = rtlabel_str[i].row;
				avr_buffer[2] = rtlabel_str[i].data_col;
				avr_buffer[3] = u8_pot[i];
				AVR_CALL();
			}

//	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
// 2 knobs on front of mbox
/*	send to serial port for testing
			for(i = 0;i < 2;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT8(u8_pot[i]);

			for(i = 8;i < 11;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT8(u8_pot[i]);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
*/
		}
		ESOS_TASK_WAIT_TICKS(2000);
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** send_fpga ****************************************//
//******************************************************************************************//
// send data to the FPGA and use handshaking lines DataReady & CmdParam
// 1) set CmdParam
// 2) set DataReady
// 3) send the cmd
// 4) set both lines low
// 5) set DataReady high
// 6) send the param
// 7) set DataReady low

ESOS_USER_TASK(send_fpga)
{
    static  UCHAR param, cmd;
    static UINT data2;
	
    ESOS_TASK_BEGIN();

	// setup the handshaking lines

	CONFIG_RG12_AS_DIG_OUTPUT();
	CONFIG_RG14_AS_DIG_OUTPUT();
	CONFIG_RG13_AS_DIG_INPUT();
	ENABLE_RG13_PULLUP();
/*
	DataReady _RG14		// output
	CmdParam _RG12		// output
	DataSent _LATG13	// input
*/

/*
	data2 = 0x21;
	param = 1;

	while(1)
	{
		// DataSent line is one of the 3 handshaking lines
		// which is input from the FPGA where the other 2 are outputs to it
		// not using DataSent for now but here's a sample
		if(DataSent == 1 && param == 1)
		{
			param = 0;
			if(++data2 > 0x7e)
				data2 = 0x21;
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
		if(DataSent == 0 && param == 0)
		{
			param = 1;
		}
		ESOS_TASK_WAIT_TICKS(1);
    } // endof while()
    ESOS_TASK_END();
*/

    while (1)
    {
       ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data2 = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);
			param = (UCHAR)data2;
			data2 >>= 8;
			cmd = (UCHAR)data2;
			CmdParam = 1;
			ESOS_TASK_WAIT_TICKS(1);
			DataReady = 1;

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
			ESOS_TASK_WAIT_ON_SEND_UINT83(cmd);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();
			ESOS_TASK_WAIT_TICKS(1);

			DataReady = 0;
			ESOS_TASK_WAIT_TICKS(1);
			CmdParam = 0;
			ESOS_TASK_WAIT_TICKS(1);
			DataReady = 1;

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
			ESOS_TASK_WAIT_ON_SEND_UINT83(param);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();
			ESOS_TASK_WAIT_TICKS(1);
			DataReady = 0;
/*
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFF);
			ESOS_TASK_WAIT_ON_SEND_UINT8(cmd);
			ESOS_TASK_WAIT_ON_SEND_UINT8(param);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		} 	// end of got mail
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** recv_fpga ****************************************//
//******************************************************************************************//
// recv data from FPGA
// this waits for serial input from FPGA which happens at a freq determinted by setting
// the update rate - for now just sends the current rpm/mph preceded by '0xFF'
// FPGA sends
// 0 - 0xFF
// 1 - low byte of temperature data1
// 2 - high byte of temperature data1
// 3 - low byte of temperature data2
// 4 - high byte of temperature data2
// 5 - low byte of temperature data3
// 6 - high byte of temperature data3
// 7 - low byte of temperature data4
// 8 - high byte of temperature data4
// 9 - low byte of rpm
// 10 - high byte of mph
// 11 - low byte of mph
// 12 - high byte of mph
// 13 - should be a '5'	(not used)
// 14 - should be a '6'		"
// 15 - should be a '7'		"
ESOS_USER_TASK(recv_fpga)
{
    static UCHAR data2;
	static int i,j;
	static UCHAR temp[20];
	static UINT ttemp;
	static UCHAR ttemp2;
	static char str[20];
	
    ESOS_TASK_BEGIN();

	data2 = 0x21;
	gl_rpm = gl_mph = 0;
	gl_engine_temp = gl_indoor_temp = gl_outdoor_temp = gl_temp4 = 0;
	i = 30;
	j = 0;
    while (1)
    {
		if(key_mode == NORMAL)
		{
			// this blocks on input of the comm port
			// so don't really have to call the wait_ticks below
			// otherwise not calling a wait_ticks in a task loop
			// will cause the whole program to freeze
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM3();
			ESOS_TASK_WAIT_ON_GET_UINT83(data2);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM3();

			if(data2 == 0xFF)
			{
				i = 0;
//		 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//				ESOS_TASK_WAIT_ON_SEND_UINT8(0xFF);
//				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
				memset(temp,0,sizeof(temp));
			}
			else
			{
				
//		 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//				ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
//				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
				temp[i++] = data2;
				if(i == 15)
				{

					gl_engine_temp = (UINT)temp[0];
					gl_engine_temp <<= 8;
					gl_engine_temp |= (UINT)temp[1];

					gl_indoor_temp = (UINT)temp[2];
					gl_indoor_temp <<= 8;
					gl_indoor_temp |= (UINT)temp[3];
/*
					gl_engine_temp++;
					if(gl_engine_temp > 0x4a)
						gl_engine_temp = 0;

					gl_indoor_temp++;
					if(gl_indoor_temp > 0x4a)
						gl_indoor_temp = 0;
*/
					gl_outdoor_temp = (UINT)temp[4];
					gl_outdoor_temp <<= 8;
					gl_outdoor_temp |= (UINT)temp[5];

					gl_temp4 = (UINT)temp[6];
					gl_temp4 <<= 8;
					gl_temp4 |= (UINT)temp[7];

					strcpy(str,temp_lookup(gl_engine_temp));

					avr_buffer[0] = DISPLAY_STR;
					avr_buffer[1] = 10;
					avr_buffer[2] = START_MENU_VALUE_COL + 10;
					strncpy((char *)&avr_buffer[3],str,9);
					AVR_CALL();

					strcpy(str,temp_lookup(gl_indoor_temp));
					avr_buffer[0] = DISPLAY_STR;
					avr_buffer[1] = 12;
					avr_buffer[2] = START_MENU_VALUE_COL + 31;
					strncpy((char *)&avr_buffer[3],str,9);
					AVR_CALL();

					strcpy(str,temp_lookup(gl_outdoor_temp));
					avr_buffer[0] = DISPLAY_STR;
					avr_buffer[1] = 11;
					avr_buffer[2] = START_MENU_VALUE_COL + 31;
					strncpy((char *)&avr_buffer[3],str,9);
					AVR_CALL();
/*
					avr_buffer[0] = SEND_INT_RT_VALUES;
					avr_buffer[1] = 0;
					avr_buffer[2] = 0;
					avr_buffer[3] = temp[0];
					avr_buffer[4] = temp[1];
					AVR_CALL();

					avr_buffer[0] = SEND_INT_RT_VALUES;
					avr_buffer[1] = 0;
					avr_buffer[2] = 8;
					avr_buffer[3] = temp[2];
					avr_buffer[4] = temp[3];
					AVR_CALL();

					avr_buffer[0] = SEND_INT_RT_VALUES;
					avr_buffer[1] = 0;
					avr_buffer[2] = 16;
					avr_buffer[3] = temp[4];
					avr_buffer[4] = temp[5];
					AVR_CALL();
					
					avr_buffer[0] = DISPLAY_FLOAT;
					avr_buffer[1] = 1;
					avr_buffer[2] = 0;
					avr_buffer[3] = temp[0];
					avr_buffer[4] = temp[1];
					AVR_CALL();

					avr_buffer[0] = DISPLAY_FLOAT;
					avr_buffer[1] = 1;
					avr_buffer[2] = 8;
					avr_buffer[3] = temp[2];
					avr_buffer[4] = temp[3];
					AVR_CALL();
*/
					i = 0;
#if 0
					break;
					case 3:
						gl_indoor_temp = (UINT)temp[i];
					break;
					case 4:
						ttemp = (UINT)temp[i];
						ttemp <<= 8;
						gl_indoor_temp |= ttemp;
					break;
					case 5:
						gl_outdoor_temp = (UINT)temp[i];
					break;
					case 6:
						ttemp = (UINT)temp[i];
						ttemp <<= 8;
						gl_outdoor_temp |= ttemp;
					break;
					case 7:
						gl_temp4 = (UINT)temp[i];
						ttemp2 = temp[i];
					break;
					case 8:
						ttemp = (UINT)temp[i];
	/*
				 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
						ESOS_TASK_WAIT_ON_SEND_UINT8(ttemp2);
						ESOS_TASK_WAIT_ON_SEND_UINT8(ttemp);
						ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	*/
						ttemp <<= 8;
						gl_temp4 |= ttemp;

						avr_buffer[0] = SEND_BYTE_RT_VALUES;
						avr_buffer[1] = 1;
						avr_buffer[2] = 0;
						avr_buffer[3] = ttemp2;
						AVR_CALL();

						avr_buffer[0] = SEND_BYTE_RT_VALUES;
						avr_buffer[1] = 1;
						avr_buffer[2] = 4;
						avr_buffer[3] = ttemp;
						AVR_CALL();

	/*
						avr_buffer[0] = SEND_INT_RT_VALUES;
						avr_buffer[1] = 0;
						avr_buffer[2] = 10;
						avr_buffer[3] = ttemp2;
						avr_buffer[4] = temp[i];
						AVR_CALL();
	*/
					break;
					case 9:
						gl_rpm = (UINT)temp[i];
						ttemp2 = temp[i];
					break;
					case 10:
						avr_buffer[0] = SEND_INT_RT_VALUES;
						avr_buffer[1] = rtlabel_str[RPM].row;
						avr_buffer[2] = rtlabel_str[RPM].data_col;
						avr_buffer[3] = ttemp2;
						avr_buffer[4] = temp[i];
						AVR_CALL();
						ttemp = (UINT)temp[i];
						ttemp <<= 8;
						gl_rpm |= ttemp;

					break;
					case 11:
						gl_mph = (UINT)temp[i];
						ttemp2 = temp[i];
					break;
					case 12:
						avr_buffer[0] = SEND_INT_RT_VALUES;
						avr_buffer[1] = rtlabel_str[MPH].row;
						avr_buffer[2] = rtlabel_str[MPH].data_col;
						avr_buffer[3] = ttemp2;
						avr_buffer[4] = temp[i];
						AVR_CALL();
						ttemp = (UINT)temp[i];
						ttemp <<= 8;
						gl_mph |= ttemp;
					break;
					case 13:
					break;
					case 14:
					break;
					case 15:
					break;
					default:
					break;
				
#endif
				}
			}
		}
		ESOS_TASK_WAIT_TICKS(1);
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
			avr_buffer[0] = EEPROM_STR;

			for(i = 0;i < NUM_MENU_LABELS;i++)
			{
				avr_buffer[1] = menu_str[i].row;
				avr_buffer[2] = menu_str[i].col;
				avr_buffer[3] = menu_str[i].str + data2;	// data2 is the offset into the 
															// menu labels
				avr_buffer[4] = 1;

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
			// data2 doesn't do anything
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			avr_buffer[0] = DISPLAY_RTLABELS;
			AVR_CALL();
/*
			avr_buffer[0] = EEPROM_STR;

			for(i = 0;i < NUM_RT_LABELS;i++)
			{
				avr_buffer[1] = rtlabel_str[i].row;
				avr_buffer[2] = rtlabel_str[i].col;
				avr_buffer[3] = rtlabel_str[i].str;
				avr_buffer[4] = 1;

				AVR_CALL();
			}
*/
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
    static UCHAR test;
    static int i;
	
    ESOS_TASK_BEGIN();
    i = 0;
    data1 = 0;
    test = 0;
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();

/*
			if(data1 == RT_DATA)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8(RT_DATA);

				for(i = 0;i < NUM_ADC_CHANNELS;i++)
				{
					ESOS_TASK_WAIT_ON_SEND_UINT8(u8_pot[i]);
				}
			}
			else ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
*/
			ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
//			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
/*
			avr_buffer[0] = SEND_BYTE_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 0;
			avr_buffer[3] = data1;
			AVR_CALL();
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
//	static UCHAR data1, data2, data3, data4, data5, data6;
	static UCHAR buffer[20];
	static int i;
	static UCHAR temp;
	static ESOS_TASK_HANDLE menu_handle;
	static ESOS_TASK_HANDLE rt_handle;
	static ESOS_TASK_HANDLE fpga_handle;
	static int len;
//	static int row, col, str;

	ESOS_TASK_BEGIN();

	menu_handle = esos_GetTaskHandle(display_menu);
	rt_handle = esos_GetTaskHandle(display_rtlabels);
	fpga_handle = esos_GetTaskHandle(send_fpga);

    i = 0;

    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
//		ESOS_TASK_WAIT_ON_GET_UINT8(data1);
		ESOS_TASK_WAIT_ON_GET_U8BUFFER(&buffer[0],21);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

//		len  = GET_ESOS_COMM_IN_DATA_LEN();
		if(buffer[0] >= START_DS_MSG && buffer[0] < END_DS_MSG)
//if(1)
		{
/*
			avr_buffer[0] = DISPLAY_STR;
			avr_buffer[1] = 0;
			avr_buffer[2] = 0;
			strncpy((char *)&avr_buffer[3],(const char*)&buffer[1],10);
			AVR_CALL();

			avr_buffer[0] = SEND_INT_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 0;
			avr_buffer[3] = (UCHAR)(len >> 8);
			avr_buffer[4] = (UCHAR)len;
			AVR_CALL();

			avr_buffer[0] = SEND_BYTE_RT_VALUES;

			avr_buffer[1] = 0;
			avr_buffer[2] = 0;
			avr_buffer[3] = buffer[1];
			AVR_CALL();
			avr_buffer[1] = 0;
			avr_buffer[2] = 4;
			avr_buffer[3] = buffer[2];
			AVR_CALL();
			avr_buffer[1] = 0;
			avr_buffer[2] = 8;
			avr_buffer[3] = buffer[3];
			AVR_CALL();

			avr_buffer[1] = 1;
			avr_buffer[2] = 0;
			avr_buffer[3] = buffer[4];
			AVR_CALL();
			avr_buffer[1] = 1;
			avr_buffer[2] = 4;
			avr_buffer[3] = buffer[5];
			AVR_CALL();
			avr_buffer[1] = 1;
			avr_buffer[2] = 8;
			avr_buffer[3] = buffer[6];
			AVR_CALL();

			avr_buffer[1] = 2;
			avr_buffer[2] = 0;
			avr_buffer[3] = buffer[7];
			AVR_CALL();
			avr_buffer[1] = 2;
			avr_buffer[2] = 4;
			avr_buffer[3] = buffer[8];
			AVR_CALL();
			avr_buffer[1] = 2;
			avr_buffer[2] = 8;
			avr_buffer[3] = buffer[9];
			AVR_CALL();

			avr_buffer[0] = SEND_BYTE_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 0;
			avr_buffer[3] = buffer[18];
			AVR_CALL();
			avr_buffer[1] = 0;
			avr_buffer[2] = 4;
			avr_buffer[3] = buffer[19];
			AVR_CALL();
			avr_buffer[1] = 0;
			avr_buffer[2] = 8;
			avr_buffer[3] = buffer[20];
			AVR_CALL();
*/
		}
		if(buffer[1] == CLEAR_SCREEN1)
		{
	
/*
			col = START_RT_VALUE_COL;
			for(str = 0,row = START_RT_VALUE_ROW;str < NUM_RT_LABELS+1;str++,row++)
			{
				rtlabel_str[str].str = str + RT_VALUES_OFFSET;
				rtlabel_str[str].row = row;
				rtlabel_str[str].col = col;
				rtlabel_str[str].data_col = col+10;
				rtlabel_str[str].onoff = 0;

				if(row == 15)
				{
					row = START_RT_VALUE_ROW-1;
					col += 17;
				}
			}

			// use menu_values_offsets to index menu_str array
			for(str = 0,row = START_MENU_VALUE_ROW,col = START_MENU_VALUE_COL;
						str < NUM_MENU_LABELS;str++,row++)
			{
				menu_str[str].str = str + MENU_VALUES_OFFSET;
				menu_str[str].row = row;
				menu_str[str].col = col;
				menu_str[str].data_col = col+10;
				menu_str[str].onoff = 0;

				if(row == 7)
				{
					row = START_MENU_VALUE_ROW-1;
					col += 19;
				}
			}
*/
			avr_buffer[0] = LCD_CLRSCR;
			avr_buffer[1] = 0;
			AVR_CALL();
	
//			__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,data1);

//			__esos_CB_WriteUINT8(rt_handle->pst_Mailbox->pst_CBuffer,data1);
#if 0
		}else if(data1 == NEW_PASSWORD2)
		{
			memset(correct_password,0,PASSWORD_SIZE);
			memset(password,0,PASSWORD_SIZE);
			correct_password[0] = data2;
			correct_password[1] = data3;
			correct_password[2] = data4;
			correct_password[3] = data5;

		}else if(data1 == NEW_PASSWORD3)
		{
			correct_password[4] = data2;
			correct_password[5] = data3;
			correct_password[6] = data4;
			correct_password[7] = data5;
		}else if(data1 == NEW_PASSWORD4)
		{
			correct_password[8] = data2;
			correct_password[9] = data3;
			correct_password[10] = data4;
			correct_password[11] = data5;

			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 14;
			avr_buffer[2] = 27;
			AVR_CALL();
			for(i = 0;i < PASSWORD_SIZE;i++)
			{
				avr_buffer[0] = CHAR_CMD;
				if(password[i] == 0)
					avr_buffer[1] = 'x';
				else	
					avr_buffer[1] = password[i];
				AVR_CALL();
			}
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 15;
			avr_buffer[2] = 27;
			AVR_CALL();
			for(i = 0;i < PASSWORD_SIZE;i++)
			{
				avr_buffer[0] = CHAR_CMD;
				if(correct_password[i] == 0)
					avr_buffer[1] = 'x';
				else	
					avr_buffer[1] = correct_password[i];
				AVR_CALL();
			}
#if 0
		}else if(data1 == RE_ENTER_PASSWORD1)
		{
			key_mode = PASSWORD;
			password_valid = 0;
 			memset(password,0,PASSWORD_SIZE);
			password_ptr = 0;
//			password_retries = 0;

			avr_buffer[0] = PASSWORD_MODE;
			avr_buffer[1] = (UCHAR)strlen(correct_password);
			AVR_CALL();
#endif
		}else if(data1 == OUTPUT_MSG && key_mode == NORMAL)
		{
			avr_buffer[0] = EEPROM_STR;
			avr_buffer[1] = 1;
			avr_buffer[2] = 20;
			avr_buffer[3] = data2 + RT_VALUES_OFFSET + 10;	// offset in eeprom starting at "STARTER"
			avr_buffer[4] = 15;
			AVR_CALL();

			avr_buffer[0] = EEPROM_STR;
			avr_buffer[1] = 1;
			avr_buffer[2] = 37;

			if(data3 == 1)
				temp = VARIOUS_MSG_OFFSET + 4;		// offset of ON str in eeprom
			else if(data3 == 0) 	
				temp = VARIOUS_MSG_OFFSET + 5;		// offset of OFF str in eeprom

			avr_buffer[3] = temp;
			avr_buffer[4] = 1;
			AVR_CALL();
			// if the port was the E-Stop switch then we must go back to password mode
			if(data1 == ESTOP_SIGNAL)
			{
				password_valid = 0;
	 			memset(password,0,PASSWORD_SIZE);
				password_ptr = 0;
				avr_buffer[0] = PASSWORD_MODE;
				avr_buffer[1] = (UCHAR)strlen(correct_password);
				AVR_CALL();
				key_mode = PASSWORD;
				engine_on = 0;
			}
/*
			avr_buffer[0] = SEND_BYTE_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 0;
			avr_buffer[3] = data2;
			AVR_CALL();
*/
 		}else if(data1 == TIME_DATA1 && key_mode == NORMAL)
		{
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 0;
			avr_buffer[2] = 23;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data2;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data3;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = '-';
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data4;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data5;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = '-';
			AVR_CALL();
		}else if(data1 == TIME_DATA2 && key_mode == NORMAL)
		{
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 0;
			avr_buffer[2] = 29;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data2;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data3;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = ' ';
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data4;
			AVR_CALL();
 
			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data5;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = ':';
			AVR_CALL();
		}else if(data1 == TIME_DATA3 && key_mode == NORMAL)
		{
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 0;
			avr_buffer[2] = 35;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data2;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data3;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = ':';
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data4;
			AVR_CALL();

			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = data5;
			AVR_CALL();
#endif
		}else if(buffer[1] == LIGHTS_OFF)
		{
			lights_on = -1;
		}else if(buffer[1] == LIGHTS_ON)
		{
			lights_on = 10;
		}else if(buffer[1] == ENGINE_ON)
		{
			engine_on = 1;
		}else if(buffer[1] == ENGINE_OFF)
		{
			engine_on = 0;
		}else if(buffer[1] == STOP_SERIAL_RECV)
			ESOS_TASK_SLEEP();

		memset(buffer,0,sizeof(buffer));

		FLUSH_ESOS_COMM_IN_DATA();	
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

		if(data2 == 0xFD)
		{
			ESOS_SIGNAL_SEMAPHORE(lcd_sem,1);
		}
/*
		else
		{
	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
*/
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** AVR_cmd ******************************************//
//******************************************************************************************//
ESOS_USER_TASK(AVR_cmd)
{

    static UCHAR buff[AVR_BUF_LEN+5];
    static UCHAR data1;
    static int i;

//	data1 = 0;	can't do this or else data1 stays at 0
    ESOS_TASK_BEGIN();

    data1 = 0;

    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			__esos_CB_ReadUINT8Buffer(__pstSelf->pst_Mailbox->pst_CBuffer,buff,AVR_BUF_LEN);

//	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
//			for(i = 0;i < 5;i++)
//				ESOS_TASK_WAIT_ON_SEND_UINT8(buff[i]);

//			ESOS_TASK_WAIT_ON_SEND_UINT8(0xFF);
//			ESOS_TASK_WAIT_ON_SEND_UINT8(0x0A);
//			ESOS_TASK_WAIT_ON_SEND_UINT8(0x0D);
//			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			ESOS_TASK_WAIT_SEMAPHORE(lcd_sem,1);

	 		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();

			for(i = 0;i < AVR_BUF_LEN;i++)
			{
				if(buff[i] == 0xFE)
					buff[i]--;
				ESOS_TASK_WAIT_ON_SEND_UINT82(buff[i]);
			}
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xFE);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		}

    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** main_proc ****************************************//
//******************************************************************************************//
// simulate the key presses
ESOS_USER_TASK(main_proc)
{
	static UCHAR data2 = 0x55;
	static UINT data3;
	static UCHAR data1;
//	static ESOS_TASK_HANDLE rt_handle;

	static int i,j,k;
//	static int row, col, str;
//	static int retry_counter;

	ESOS_TASK_BEGIN();

	engine_on = 0;
	// these are global
	avr_handle = esos_GetTaskHandle(AVR_cmd);
	fpga_handle = esos_GetTaskHandle(send_fpga);
	recv_handle = esos_GetTaskHandle(recv_comm1);
	// this is local
//	rt_handle = esos_GetTaskHandle(display_rtlabels);

	i = 0;
	j = 0;
	k = 0x1f;

	data3 = LCD_PWM;
	data3 <<= 8;
	data3 &= 0xFF00;
//	data3 |= PWM_80DC_PARAM;
	data3 |= PWM_ON_PARAM;
//	data3 |= PWM_12DC_PARAM;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

	data3 = SET_FPGA_SEND_UPDATE_RATE;
	data3 <<= 8;
	data3 &= 0xFFFF;
	data3 |= 0xFF;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
//	ESOS_TASK_WAIT_TICKS(100);

	avr_buffer[0] = GOTO_CMD;
	avr_buffer[1] = 1;
	avr_buffer[2] = 1;
	AVR_CALL();

	avr_buffer[0] = GOTO_CMD;
	avr_buffer[1] = 1;
	avr_buffer[2] = 1;
	AVR_CALL();

	avr_buffer[0] = LCD_CLRSCR;
	avr_buffer[1] = 0;
	AVR_CALL();
/*
	avr_buffer[0] = EEPROM_STR;
	avr_buffer[1] = 0;	// row 0
	avr_buffer[2] = 0;	// col 0
	avr_buffer[3] = VARIOUS_MSG_OFFSET+3;
	avr_buffer[4] = 2;
	AVR_CALL();

	avr_buffer[0] = EEPROM_STR;
	avr_buffer[1] = 1;	// row 0
	avr_buffer[2] = 0;	// col 0
	avr_buffer[3] = VARIOUS_MSG_OFFSET+2;
	avr_buffer[4] = 2;
	AVR_CALL();
*/
	menu_ptr = 0;
    password_valid = 0;
    password_ptr = 0;
	password_retries = 0;

	key_mode = PASSWORD;
//	key_mode = NORMAL;

	memset(correct_password,0,sizeof(correct_password));
	strcpy(correct_password,"2354795\0");
	memset(password,0,PASSWORD_SIZE);

	avr_buffer[0] = PASSWORD_MODE;
//	avr_buffer[1] = (UCHAR)strlen(correct_password);
	avr_buffer[1] = 0;
	AVR_CALL();

	i = 0;
	j = 0;
	k = 0;

	data2 = 0x21;
	ESOS_TASK_WAIT_TICKS(100);
	data2 = 0x30;

//	__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,data2);
//	__esos_CB_WriteUINT8(rt_handle->pst_Mailbox->pst_CBuffer,data2);

	data3 = DTMF_TONE_ON;
	data3 <<= 8;
	data3 &= 0xFF00;
	data3 |= 12;

	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

	ESOS_TASK_WAIT_TICKS(2);

	data3 = DTMF_TONE_OFF;
	data3 <<= 8;
	data3 &= 0xFF00;
	__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
/*
	avr_buffer[0] = EEPROM_STR;
	avr_buffer[1] = 6;	// row 6
	avr_buffer[2] = 0;	// col 0
	avr_buffer[3] = VARIOUS_MSG_OFFSET;	//  ("enter password")
	avr_buffer[4] = 10;
	AVR_CALL();
*/
	init_rt_labels();
//	init_menu_labels();
//	ignore_comm1 = 0;
//	__esos_CB_WriteUINT8(rt_handle->pst_Mailbox->pst_CBuffer,data1);

	while(TRUE)
	{
		ESOS_TASK_WAIT_TICKS(500);
/*
		data3 = TEST_COMM;	// used for testing serial connection to FPGA - changes the 4 leds on the FPGA
		data3 <<= 8;
		data3 &= 0xFF00;
		data3 |= data2;
		__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);

		if(++data2 > 0x7e)
			data2 = 0x21;
*/
/*
		if(password_valid == 0)
		{
			key_mode = PASSWORD;
		}
*/
#if 0
		if(password_retries > PASSWORD_RETRIES_NUM && ESOS_IS_TASK_SLEEPING(password_task_handle))
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
#endif
	}
    ESOS_TASK_END();
}



// use menu_values_offsets to index menu_str array
static void init_menu_labels(void)
{
	static int col, row,str;
	for(str = 0,row = START_MENU_VALUE_ROW,col = START_MENU_VALUE_COL;
				str < NUM_MENU_LABELS*NO_MENUS;str++,row++)
	{
		menu_str[str].str = str + MENU_VALUES_OFFSET;
		menu_str[str].row = row;
		menu_str[str].col = col;
		menu_str[str].data_col = col+10;
		menu_str[str].onoff = 0;

		if(row == 7)
		{
			row = START_MENU_VALUE_ROW-1;
			col += 19;
		}
	}
}
// use enum rt_values_offsets to index rtlabels_str array
static void init_rt_labels(void)
{
	static int col, row,str;
	
	col = START_RT_VALUE_COL;
 	for(str = 0,row = START_RT_VALUE_ROW;str < NUM_RT_LABELS+1;str++,row++)
	{
		rtlabel_str[str].str = str + RT_VALUES_OFFSET;
		rtlabel_str[str].row = row;
		rtlabel_str[str].col = col;
		rtlabel_str[str].data_col = col+10;
		rtlabel_str[str].onoff = 0;

		if(row == 15)
		{
			row = START_RT_VALUE_ROW-1;
			col += 17;
		}
	}
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
	esos_RegisterTask(AVR_cmd);
	esos_RegisterTask(send_fpga);

	esos_RegisterTask(recv_fpga);
	esos_RegisterTask(recv_lcd);
	esos_RegisterTask(convADC);
	esos_RegisterTask(main_proc);
	esos_RegisterTask(send_comm1);
	esos_RegisterTask(recv_comm1);
	esos_RegisterTask(menu_task);
	esos_RegisterTask(password_task);
	esos_RegisterTask(numentry_task);
	esos_RegisterTask(display_menu);
	esos_RegisterTask(display_rtlabels);
	esos_RegisterTask(key_timer_task);
	esos_RegisterTask(temp_monitor_task);
	esos_RegisterTask(display_rtvalues);
} // end user_init()

