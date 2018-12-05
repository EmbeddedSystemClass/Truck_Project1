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
    static int i;
    static UCHAR data1;

    ESOS_TASK_BEGIN();

	data1 = 0x21;

	while(1)
	{
		if(key_mode == NORMAL)
		{
			// engine temp doesn't come from ADC anymore
			// 
			for(i = OIL_PRES ;i <= O2+1;i++)
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

					gl_rpm = (UINT)temp[8];			// not displaying these yet
					gl_rpm <<= 8;
					gl_rpm |= (UINT)temp[9];

					gl_mph = (UINT)temp[10];
					gl_mph <<= 8;
					gl_mph |= (UINT)temp[11];

					strcpy(str,temp_lookup(gl_engine_temp));

					avr_buffer[0] = DISPLAY_STR;
					avr_buffer[1] = rtlabel_str[ENG_TEMP].row;
					avr_buffer[2] = rtlabel_str[ENG_TEMP].data_col;
//					str[5] = 0;
					strncpy((char *)&avr_buffer[3],str,strlen(str));
//					avr_buffer[4] = 6;
					AVR_CALL();

					strcpy(str,temp_lookup(gl_indoor_temp));

					avr_buffer[0] = DISPLAY_STR;
					avr_buffer[1] = rtlabel_str[INDOOR_TEMP].row;
					avr_buffer[2] = rtlabel_str[INDOOR_TEMP].data_col;
//					str[5] = 0;
					strncpy((char *)&avr_buffer[3],str,strlen(str));
//					avr_buffer[4] = 6;
					AVR_CALL();

					strcpy(str,temp_lookup(gl_outdoor_temp));

					avr_buffer[0] = DISPLAY_STR;
					avr_buffer[1] = rtlabel_str[OUTDOOR_TEMP].row;
					avr_buffer[2] = rtlabel_str[OUTDOOR_TEMP].data_col;
//					str[5] = 0;
					strncpy((char *)&avr_buffer[3],str,strlen(str));
//					avr_buffer[4] = 6;
					AVR_CALL();
					i = 0;
				}
			}
		}
		ESOS_TASK_WAIT_TICKS(1);
    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//********************************** display_rtlabels **************************************//
//******************************************************************************************//
ESOS_USER_TASK(display_rtlabels)
{
	static UCHAR data2, i, onoff, code;
/*
	static int off_status[7] = {
			SHUTDOWN,
			BLOWER_OFF,
			OFF_FAN,
			OFF_LIGHTS,
			OFF_RUNNING_LIGHTS,
			OFF_BRIGHTS,
			OFF_BRAKES };
*/
	ESOS_TASK_BEGIN();
/*
	onoff = 1;
	for(i = 0;i < 7;i++)		// start off with all the status labels at 'OFF'
	{
		code = off_status[i];
		avr_buffer[0] = EEPROM_STR2;
		avr_buffer[1] = code;
		avr_buffer[2] = onoff;
		AVR_CALL();
	}
*/
	init_rtlabels();
	while (1)
	{
        ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			// data2 doesn't do anything
			data2 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			avr_buffer[0] = DISPLAY_RTLABELS;
			// starting row
			avr_buffer[1] = START_RT_VALUE_ROW;
			// starting col
			avr_buffer[2] = START_RT_VALUE_COL;
			// ending row
			avr_buffer[3] = ENDING_RT_VALUE_ROW;
			// col width
			avr_buffer[4] = RT_VALUE_COL_WIDTH;
			// num lables
			avr_buffer[5] = NUM_RT_LABELS;
			AVR_CALL();

			avr_buffer[0] = DISPLAY_STATUSLABELS;
			AVR_CALL();
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
    static char str[15];
	
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
			ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
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
	static UCHAR cmd, code;
	static UINT data1, data2;
	static int i;
	static UCHAR buffer[33];
	static ESOS_TASK_HANDLE process_comm1_handle;
	static ESOS_TASK_HANDLE process_comm1_buffer_handle;

	ESOS_TASK_BEGIN();

	process_comm1_handle = esos_GetTaskHandle(process_comm1);
	process_comm1_buffer_handle = esos_GetTaskHandle(process_comm1_buffer);

    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT16(data1);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

		cmd = (UCHAR)data1;
		data1 >>= 8;
		code = (UCHAR)data1;

		// if the cmd != GENERIC_CMD then it's one of the others
		// in which case code is actually the length of the data to send
		// otherwise its only 2 bytes
		if(cmd > START_DS_MSG && cmd < END_DS_MSG)
		{
			if(cmd != GENERIC_CMD)
			{
				buffer[0] = cmd;
				gl_comm1_buf_len = code;
/*
					avr_buffer[0] = SEND_BYTE_RT_VALUES;
					avr_buffer[1] = 0;
					avr_buffer[2] = 10;
					avr_buffer[3] = cmd;
					AVR_CALL();

					avr_buffer[0] = SEND_BYTE_RT_VALUES;
					avr_buffer[1] = 0;
					avr_buffer[2] = 20;
					avr_buffer[3] = code;
					AVR_CALL();
*/
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				for(i = 2;i < code;i++)
				{
					ESOS_TASK_WAIT_ON_GET_UINT8(buffer[i]);
	/*
					avr_buffer[0] = SEND_BYTE_RT_VALUES;
					avr_buffer[1] = 15;
					avr_buffer[2] = (i-3)*3;
					avr_buffer[3] = buffer[i];
					AVR_CALL();
	*/
				}
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
				__esos_CB_WriteUINT8Buffer(process_comm1_buffer_handle->pst_Mailbox->pst_CBuffer, 
						&buffer[0], gl_comm1_buf_len);
			}else
				__esos_CB_WriteUINT8(process_comm1_handle->pst_Mailbox->pst_CBuffer,code);
		}
		memset(buffer,0,sizeof(buffer));
		FLUSH_ESOS_COMM_IN_DATA();
	} // endof while()
    ESOS_TASK_END();
}

//******************************************************************************************//
//********************************* process_comm1_buffer ***********************************//
//******************************************************************************************//
ESOS_USER_TASK(process_comm1_buffer)
{
	static UCHAR buffer[33];
	static UCHAR data1;
	static UCHAR cmd, temp;
	static int i;

	ESOS_TASK_BEGIN();

    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			__esos_CB_ReadUINT8Buffer(__pstSelf->pst_Mailbox->pst_CBuffer, &buffer[0], gl_comm1_buf_len);

/*
			for(i = 1;i < 10;i++)
			{
				avr_buffer[0] = SEND_BYTE_RT_VALUES;
				avr_buffer[1] = 14;
				avr_buffer[2] = (i-1)*3;
				avr_buffer[3] = buffer[i];
				AVR_CALL();
			}

			for(i = 25;i < 30;i++)
			{
				avr_buffer[0] = SEND_BYTE_RT_VALUES;
				avr_buffer[1] = 15;
				avr_buffer[2] = (i-25)*3;
				avr_buffer[3] = buffer[i];
				AVR_CALL();
			}
*/
			cmd = buffer[0];
			
			switch(cmd)
			{
				case SEND_PARAMS:
					temp = buffer[1];
					temp <<= 8;
					gl_fan_on = (UINT)buffer[2];
					gl_fan_on |= (UINT)temp;

					temp = buffer[3];
					temp <<= 8;
					gl_fan_off = (UINT)buffer[4];
					gl_fan_off |= (UINT)temp;

					temp = buffer[5];
					temp <<= 8;
					gl_blower_en = (UINT)buffer[6];
					gl_blower_en |= (UINT)temp;

					temp = buffer[7];
					temp <<= 8;				
					gl_blower3_on = (UINT)buffer[8];
					gl_blower3_on |= (UINT)temp;

					temp = buffer[9];
					temp <<= 8;
					gl_blower2_on = (UINT)buffer[10];
					gl_blower2_on |= (UINT)temp;

					temp = buffer[11];
					temp <<= 8;
					gl_blower1_on = (UINT)buffer[12];
					gl_blower1_on |= (UINT)temp;
					break;

				case STOP_SERIAL_RECV:
					ESOS_TASK_SLEEP();
					break;

				case ESTOP_SIGNAL:
					key_mode = PASSWORD;

					memset(correct_password,0,sizeof(correct_password));
					strcpy(correct_password,"2354795\0");
					memset(password,0,PASSWORD_SIZE);

					avr_buffer[0] = PASSWORD_MODE;
				//	avr_buffer[1] = (UCHAR)strlen(correct_password);
					avr_buffer[1] = 0;
					AVR_CALL();
					break;
			}
		}
	} // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************ process_comm1 ***************************************//
//******************************************************************************************//
// recv data from TS-7200
ESOS_USER_TASK(process_comm1)
{
	static UCHAR code;
	static UCHAR onoff;

	ESOS_TASK_BEGIN();

    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();

        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			code = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
/*
			avr_buffer[0] = SEND_INT_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 10;
			avr_buffer[3] = (UCHAR)(data1 >> 8);
			avr_buffer[4] = (UCHAR)data1;
			AVR_CALL();

			avr_buffer[0] = SEND_BYTE_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 25;
			avr_buffer[3] = cmd;
			AVR_CALL();

			avr_buffer[0] = SEND_BYTE_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 35;
			avr_buffer[3] = code;
			AVR_CALL();
*/
			switch(code)
			{
				case START_SEQ:
					onoff = 0;
					engine_on = 1;
					break;
				case SHUTDOWN:
					onoff = 1;
					engine_on = 0;
					break;
				case BLOWER_OFF:
					onoff = 1;
					break;
				case BLOWER1:
					onoff = 2;
					break;
				case BLOWER2:
					onoff = 3;
					break;
				case BLOWER3:
					onoff = 4;
					break;
				case ON_FAN:
					onoff = 0;
					break;
				case OFF_FAN:
					onoff = 1;
					break;
				case ON_LIGHTS:
					onoff = 0;
					lights_on = 10;
					break;
				case OFF_LIGHTS:
					onoff = 1;
					lights_on = -1;
					break;
				case ON_RUNNING_LIGHTS:
					onoff = 0;
					break;
				case OFF_RUNNING_LIGHTS:
					onoff = 1;
					break;
				case ON_BRIGHTS:
					onoff = 0;
					break;
				case OFF_BRIGHTS:
					onoff = 1;
					break;
				case ON_BRAKES:
					onoff = 0;
					break;
				case OFF_BRAKES:
					onoff = 1;
					break;
			}	// end of switch(cmd)
			avr_buffer[0] = EEPROM_STR2;
			avr_buffer[1] = code;
			avr_buffer[2] = onoff;
			AVR_CALL();
		} // end of got mail	
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

	static int i,j,k;

	ESOS_TASK_BEGIN();

	engine_on = 0;
	// these are global
	avr_handle = esos_GetTaskHandle(AVR_cmd);
	fpga_handle = esos_GetTaskHandle(send_fpga);
	recv_handle = esos_GetTaskHandle(recv_comm1);

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

	while(TRUE)
	{
		ESOS_TASK_WAIT_TICKS(500);
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
//******************************************************************************************//
//*********************************** init_rtlabels  **************************************//
//******************************************************************************************//
static void init_rtlabels(void)
{
	static UCHAR col, data_col, row, str;
	int i;
	
	col = START_RT_VALUE_COL;
	data_col = col + 9;
	
	// rt labels at bottom of screen
 	for(str = 0,row = START_RT_VALUE_ROW;str < NUM_RT_LABELS+1;str++,row++)
	{
		rtlabel_str[str].str = str + RT_VALUES_OFFSET;
		rtlabel_str[str].row = row;
		rtlabel_str[str].col = col;
		rtlabel_str[str].data_col = data_col;

		if(row == ENDING_RT_VALUE_ROW)
		{
			row = START_RT_VALUE_ROW;
			col += RT_VALUE_COL_WIDTH;
			data_col = col + 15;
		}
	}
	row = col = 0;
	data_col = 10;

	// init labels for status above rt labels
 	for(str = 0;str < NUM_STATUS_LABELS+1;str++,row++)
	{
		status_label_str[str].str = str + STATUS_VALUES_OFFSET;
		status_label_str[str].row = row;
		status_label_str[str].col = col;
		status_label_str[str].data_col = data_col;
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
	esos_RegisterTask(process_comm1);
	esos_RegisterTask(process_comm1_buffer);
	esos_RegisterTask(menu_task);
	esos_RegisterTask(password_task);
	esos_RegisterTask(numentry_task);
	esos_RegisterTask(display_rtlabels);
	esos_RegisterTask(key_timer_task);
	esos_RegisterTask(temp_monitor_task);
	esos_RegisterTask(display_rtvalues);
} // end user_init()


