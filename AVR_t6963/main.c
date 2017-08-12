#if 1
//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// main program that drives the t6963 LCD (32 col x 15 row) display - text only
// see t6963_notes.txt for more details
#include <avr/io.h>
#include "avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include <string.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

char eepromString[STRING_LEN] EEMEM;

//PROMPT_STRUCT prompts[26];	// this must be large enough for no_prompts
#endif
int main(void)
{
	int i;
	uint16_t temp2;
	UCHAR ret_char, temp;
	int done;
	UCHAR xbyte;
	UINT xword;
	int do_states = 1;
//    size_t str_size = sizeof(PROMPT_STRUCT);

	initUSART();

	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	GDispStringAt(1,1,"LCD is on!");

//******************************************************************************************//
//******************* read all the data from eeprom into memory  ***************************//
//******************************************************************************************//
	no_rt_labels  = eeprom_read_byte((UCHAR*)NO_RT_LABELS_EEPROM_LOCATION);

	if(no_rt_labels != 0xff)
	{
#ifdef TTY_DISPLAY
#warning "TTY_DISPLAY defined"
		printString("reading prompt data into prompt structs\r\n");
#endif
		no_rt_labels  = eeprom_read_byte((UCHAR*)NO_RT_LABELS_EEPROM_LOCATION);
		no_menu_labels  = eeprom_read_byte((UCHAR*)NO_MENU_LABELS_EEPROM_LOCATION);
		no_rtparams  = eeprom_read_byte((UCHAR*)NO_RTPARAMS_EEPROM_LOCATION);
		no_menu_structs  = eeprom_read_byte((UCHAR*)NO_MENUS_EEPROM_LOCATION);
#ifdef TTY_DISPLAY
		printString("no_labels: ");
		printHexByte(no_labels);
		printString("\r\n");
		printString("no_rtparams: ");
		printHexByte(no_rtparams);
		printString("\r\n");
		printString("no_menu_structs: ");
		printHexByte(no_menu_structs);
		printString("\r\n");
#endif
		rt_params_offset = (UINT)eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (UINT)temp;
		rt_params_offset |= (temp2 << 8);		  // shift msb over and bit-or with lsb

		menu_struct_offset = (UCHAR)eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (UINT)temp;
		menu_struct_offset |= (temp2 << 8);		  // shift msb over and bit-or with lsb

#ifdef TTY_DISPLAY
/*
		printString("label_info_offset: ");
		printHexByte((UCHAR)label_info_offset);
		printHexByte((UCHAR)(label_info_offset>>8));
		printString("\r\n");
*/
		printString("rt_params_offset: ");
		printHexByte((UCHAR)rt_params_offset);
		printHexByte((UCHAR)(rt_params_offset>>8));
		printString("\r\n");
		printString("menu_struct_offset: ");
		printHexByte((UCHAR)menu_struct_offset);
		printHexByte((UCHAR)(menu_struct_offset>>8));
		printString("\r\n");
#endif
#if 0
		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
		}

#ifdef TTY_DISPLAY
		for(i = 0;i < no_prompts;i++)
		{
			printHexByte((UCHAR)prompts[i].pnum);
			transmitByte(0x20);
			printHexByte((UCHAR)prompts[i].row);
			transmitByte(0x20);
			printHexByte((UCHAR)prompts[i].col);
			transmitByte(0x20);
//			printHexByte((UCHAR)(prompts[i].offset>>8));
//			printHexByte((UCHAR)(prompts[i].offset));
			transmitByte(0x20);
//			printHexByte((UCHAR)prompts[i].len);
			transmitByte(0x20);
			printHexByte((UCHAR)prompts[i].type);
			printString("\r\n");
		}
		printString("\r\ndone reading eeprom\r\n");
#endif
#endif
	}
	else
	{
		printString("prompts not set - run eeprom_burn.c to burn eeprom\r\n");
	}

//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	GDispClrTxt();
	set_defaults();
	_delay_us(10);
	init_list();
	display_labels();
	done = 0;
	char param_string[10];
	UCHAR avr_data[AUX_DATA_SIZE];
	UCHAR avr_data2[AUX_DATA_SIZE];
	UCHAR auxcmd, auxparam;
	UINT tempint;
	UINT tempint2 = 0;
	UINT tempint3 = 0;

	while (1)
	{
		ret_char = receiveByte();
		ret_char = get_key(ret_char);
		done = parse_P24(ret_char, param_string, &xbyte, &xword);

// try this, just to be a little more cryptic:
//		done = parse_P24(get_key(receiveByte()), param_string, &xbyte, &xword);

		if(done)
		{
			if(current_param == RT_RPM)
			{
#ifdef TTY_DISPLAY
				printHexByte((UCHAR)(xword>>8));
				printHexByte((UCHAR)xword);
//			GDispStringAt(2,2,param_string);
#else
				transmitByte((UCHAR)(xword>>8));
				transmitByte((UCHAR)xword);
//			GDispStringAt(2,2,param_string);
#endif
			}
			else if(current_param != RT_AUX1 && current_param != RT_AUX2)
			{
#ifdef TTY_DISPLAY
				printHexByte(xbyte);
//			GDispStringAt(2,4,param_string);
#else
				transmitByte(xbyte);
//			GDispStringAt(2,4,param_string);
#endif
			}
			else if(current_param == RT_AUX1)
			{
				tempint = atol(param_string);
				auxparam = (UCHAR)tempint;
				tempint >>= 8;
				auxcmd = (UCHAR)tempint;
/*
				mvwprintw(win, DISP_OFFSET+27, 2,"cmd: %x  param: %x  ",auxcmd,auxparam);
				mvwprintw(win, DISP_OFFSET+35, 2,"mod_data_ready: %d  ",mod_data_ready);
				mvwprintw(win, DISP_OFFSET+36, 2,"new_data_ready: %d  ",new_data_ready);
*/
				switch(aaux_state)
				{
					// menu choice sets aux_index to something other than 0
					// when it wants certain data from PIC24
					case IDLE_AUX:
						mod_data_ready = 0;
						if(aux_index != 0)
						{
							avr_data[0] = CMD_GET_DATA;
							avr_data[1] = aux_index-1;
							aaux_state = DATA_REQ;
						}
						else aaux_state = IDLE_AUX;
						break;
					// wait for PIC24 to get data
					case DATA_REQ:
						aux_index = 0;
						if(auxcmd == CMD_DATA_READY)	// new data should be in tempint2
						{
							aaux_state = VALID_DATA;
							sprintf(cur_global_number,"%-6u",tempint2);
							i = 0;
							do
							{
								i++;
							}while(cur_global_number[i] != 0x20);
							cur_global_number[i] = 0;
/*
							strncpy(tempnum,cur_global_number+i,NUM_ENTRY_SIZE-i);
							mvwprintw(win, DISP_OFFSET+24,2,"tempnum: %s   %d",tempnum,i);
							memset(cur_global_number,0,NUM_ENTRY_SIZE);
							strcpy(cur_global_number,tempnum);
							cur_global_number[NUM_ENTRY_SIZE-i] = 0;
							mvwprintw(win, DISP_OFFSET+25,2,"cur:     %sx     ",cur_global_number);
							mvwprintw(win, DISP_OFFSET+26,2,"%sx   %d   ",cur_global_number,tempint2);
*/
							new_data_ready = 1;
//								loop = 0;
						}
						else
						{
							aaux_state = DATA_REQ;
//								loop = break_out_loop(loop,aaux_state);
//								mvwprintw(win, DISP_OFFSET+28,2,"loop: %d ",loop);
						}
						break;
					// read data into buffer and waits for user to modify it (or cancel)
					case VALID_DATA:
//							mvwprintw(win, DISP_OFFSET+28,2,"             ");
//							mvwprintw(win, DISP_OFFSET+35, 2,"mod_data_ready: %d  ",mod_data_ready);
						if(mod_data_ready == 1)
						{
							aaux_state = DATA_READY;
//								loop = 0;
						}
						else
						{
//								loop = break_out_loop(loop,aaux_state);
//								mvwprintw(win, DISP_OFFSET+28,2,"loop: %d ",loop);
							aaux_state = VALID_DATA;
						}
						break;
					// data has been modified by AVR and is ready to send back to PIC24
					case DATA_READY:
						tempint2 = atol(new_global_number);
						avr_data[0] = CMD_NEW_DATA;
						aaux_state = DATA_READY;
						avr_data2[0] = (UCHAR)(tempint2 >> 8);
						avr_data2[1] = (UCHAR)tempint2;
//						limit16++;
//							avr_data2[2] = (UCHAR)(limit16a >> 8);
//							avr_data2[3] = (UCHAR)limit16a;
// this is a work-around for strange bug - for some reason the high bit of the 2nd byte is getting unset
						avr_data2[2] = avr_data2[0];
						avr_data2[3] = avr_data2[1] >> 1;
//						limit16a += 2;
//						mvwprintw(win, DISP_OFFSET+29, 2,"%d   %d   ",limit16,limit16a);
//							if(++taux_index > no_menu_labels)
//								taux_index = 1;
//						mvwprintw(win, DISP_OFFSET+32, 2,"                                                    ");
						aaux_state = IDLE_AUX;
						break;
					default:
						aaux_state = IDLE_AUX;
						break;
				}
/*
				mvwprintw(win, DISP_OFFSET+30, 2,"aux_index: %x  ",aux_index);
				disp_astate(aaux_state,tempx);
				mvwprintw(win, DISP_OFFSET+31, 2,"%s     ",tempx);
				disp_auxcmd(avr_data[0],tempx);
				mvwprintw(win, DISP_OFFSET+31, 20,"%s     ",tempx);

				for(i = 0;i < AUX_DATA_SIZE;i++)
					mvwprintw(win, display_offset+32, 2+(i * 3),"%x  ",avr_data[i]);
*/
//					avr_data[2] = (UCHAR)(tempint3 >> 8);
//					avr_data[3] = (UCHAR)tempint3;
				avr_data[2] = avr_data[3] = 0;
				tempint3++;
// write avr_data
//				write(fd,avr_data,AUX_DATA_SIZE);
				for(i = 0;i < AUX_DATA_SIZE;i++)
					transmitByte(avr_data[i]);

			} else if (current_param == RT_AUX2)
			{
				if(aaux_state == DATA_REQ)
				{
					tempint2 = atol(param_string);
					strcpy(param_string,cur_global_number);
//					mvwprintw(win, DISP_OFFSET+34, 2,"tempint2: %d  ",tempint2);
				}
				else tempint2 = 0;
//					for(i = 0;i < AUX_DATA_SIZE;i++)
//						avr_data2[i]++;
//				mvwprintw(win, display_offset+33, 2,"avr_data2:");
//				for(i = 0;i < AUX_DATA_SIZE;i++)
//					mvwprintw(win, display_offset+33, 12+(i * 3),"%x  ",avr_data2[i]);
// write avr_data2
//				write(fd,avr_data2,AUX_DATA_SIZE);
				for(i = 0;i < AUX_DATA_SIZE;i++)
					transmitByte(avr_data2[i]);
			}
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == current_param)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+9,"      ");
				}

				if(rt_params[i].type == current_param && rt_params[i].shown == SHOWN_SENT)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+10,param_string);
				}
			}
			set_defaults();
		}

	}
	return (0);									  // this should never happen
}


//******************************************************************************************//
//******************************************* dispRC ***************************************//
//******************************************************************************************//
// show on monitor where the current row, col is
void dispRC(int row, int col)
{
#if TTY_DISPLAY
	printString("col=");
	printByte(col);
	printString("\r\n");
	printString("row=");
	printByte(row);
	printString("\r\n");
	printString("\r\n");
#endif
}


//******************************************************************************************//
//****************************************** CheckRC ***************************************//
//******************************************************************************************//
// check row col to see if it is 'off the screen' so it can re-appear on the opposite border
void CheckRC(int *row, int *col, UCHAR *k)
{
	if(*row >= ROWS)
		*row = 0;

	if(*col >= COLUMN)
		*col = 0;

	if(*row < 0)
		*row = ROWS-1;

	if(*col < 0)
		*col = COLUMN-1;

	if(*k > 0x50)
		*k = 0x41;
}


void set_defaults(void)
{
	temp_UINT = 0;
	parse_state = IDLE;
}
