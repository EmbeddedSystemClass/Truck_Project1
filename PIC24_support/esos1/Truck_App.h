#if 1
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
//
// "I am not so much concerned with the return on capital as I am with the return of capital." - Will Rogers
//

#include "../esos/include/esos.h"
#include "../esos/include/pic24/esos_pic24.h"
#include "../esos/include/pic24/esos_pic24_rs232.h"
#include "../esos/include/pic24/esos_pic24_spi.h"
#include "../lib/include/pic24_all.h"
#include "../lib/include/pic24_timer.h"
#include "../lib/include/pic24_util.h"
#include "../lib/include/pic24_adc.h"
#include "../lib/include/pic24_ports_config.h"
#include "../lib/include/pic24_ports_mapping.h"
#include "pic_t6963.h"
//#include "main.h"
//#include "../../AVR_t6963/key_defs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// DEFINEs go here
//#define   CONFIG_LED1()   printf("called CONFIG_LED1()\n");
uint8_t     LED1 = TRUE;      // LED1 is initially "on"
#define MY_ESOS1
#define TIME_DELAY		20
#define TEMP_DELAY3		2
#define TEMP_DELAY2		1000
#define TDELAY 2
#define TDELAY2 1
#define VREF 3.3  //assume Vref = 3.3 volts
#define FLAG1        ESOS_USER_FLAG_1
#define RT_OFFSET 0x70
//#define AUX_STRING_LEN 1024	// this should be the same as AUX_STRING_LEN in AVR_t6963/main.h
#define AUX_STRING_LEN 300

#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e4
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e8
#define NO_MENUS_EEPROM_LOCATION 0x3ea
#define RT_PARAMS_OFFSET_EEPROM_LOCATION 0x0350
#define MAX_LABEL_LEN 10
#define RT_OFFSET 0x70
#define NUM_ENTRY_SIZE 7
//#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/2)
#define NUM_ENTRY_BEGIN_COL 3
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
#define NUM_ENTRY_ROW 8
#define NUM_CHECKBOXES 10
#define TEMP_EEPROM_OFFSET 400
#define NUM_CBLABELS 50
#define CBLABEL_SIZE 600
#define TIME_DELAY1 1
#define NUM_FPTS 6
#define NUM_LABELS 40
#define NUM_MENU_CHOICES 6
#define NUM_RT_PARAMS 11
#define NUM_RT_LABELS NUM_RT_PARAMS

#define DISP_OFFSET 1
#define LAST_ROW DISP_OFFSET + 58
#define NUM_EXECCHOICES 10
#define SCALE_DISP_ALL 0
#define SCALE_DISP_SOME 1
#define SCALE_DISP_NONE 2
#define RT_OFFSET 0x70
#define SIM_EEPROM_SIZE 0x800
#define CBLABEL_SIZE 600

char cblabels[CBLABEL_SIZE];

#endif

#if 1
volatile int avr_ptr;
volatile UCHAR random_data;
volatile int checkbox_offsets[NUM_CBLABELS];

int get_label(int index, char *str);
UCHAR get_keypress(UCHAR key1);
volatile int goffset;

int pack(UCHAR low_byte, UCHAR high_byte);
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte);
int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index);
UCHAR menu_change(UCHAR ch);
UCHAR do_exec(UCHAR ch);
UCHAR do_chkbox(UCHAR ch);
UCHAR non_func(UCHAR ch);
UCHAR do_numentry(UCHAR ch);
UCHAR do_init(UCHAR ch);

int prev_list(void);
volatile int current_fptr;		// pointer into the menu_list[]
volatile int prev_fptr;
volatile int list_size;
volatile int dirty_flag;
int curr_checkbox;
int curr_execchoice;
int last_checkbox;
volatile int first_menu;
volatile char cur_global_number[NUM_ENTRY_SIZE];
volatile char new_global_number[NUM_ENTRY_SIZE];
volatile int sample_numbers[5];
volatile int _size;
volatile int start_addr;
volatile UCHAR cur_col;
volatile UCHAR cur_row;
volatile int sync;
UCHAR curr_menus[6];
UCHAR enter(UCHAR ch);
void init_numentry(int menu_index);
void cursor_forward_stuff(char x);
void stuff_num(char num);
void cursor_forward(char ch);
static void init_checkboxes(int index);
static void blank_choices(void);
static void blank_display(void);
static void blank_menu(void);
#define LIST_SIZE 50
static int curr_chkbox_index;
volatile int menu_list[LIST_SIZE];	// works like a stack for the how deep into the menus we go
UCHAR set_list(int fptr);
UCHAR get_fptr(void);
void get_fptr_label(char *str);
int get_curr_menu_index(void);
int get_curr_menu(void);
UCHAR backspace(UCHAR ch);
void _eclear(void);
void cursor_backward(void);
static int scrollup_checkboxes(int index);
static int scrolldown_checkboxes(int index);
static int scrollup_execchoice(int index);
static int scrolldown_execchoice(int index);
static int toggle_checkboxes(int index);
static void clean_disp_num(void);
static void display_edit_value(void);
static void display_menus(void);
void adv_menu_label(int index, UCHAR *row, UCHAR *col);
static void init_execchoices(int menu_index);
static UCHAR checkboxes_reset(int index);
#define NUM_ENTRY_SIZE 7
//#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/2)
#define NUM_ENTRY_BEGIN_COL 3
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
// '!' - '9' (33 - 58) + 'A' - 'Z' (26) + 'a' - 'z' (26) = 77
//#define NUM_ALNUM 77
//#define NUM_ALNUM 52		// without the '!' - '9'
#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_START_ROW 14
#define MENU_START_COL 3
#define MENU_BLANK "          "
#define _dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define _dispStringAt(_row,_col,_char) GDispStringAt((UINT)_row,(UINT)_col,(char *)_char)
#define _dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)
#define _dispSetMode(_mode) GDispSetMode(_mode)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
#ifdef TEST_WRITE_DATA
void set_win(WINDOW *win);
#endif
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_str_len(void);
int update_mlabels(int i, char *str);
int update_cblabels(int i, char *str);
void get_cblabel(int index, char *str);
UCHAR get_row(int index);
UCHAR get_col(int index);
int get_mlabel_offsets(void);
int get_cblabel_offsets(void);
void get_mlabel(int index, char *str);
void init_list(void);
int mlabel_offsets[NUM_LABELS+NUM_RT_LABELS];
int cblabel_offsets[NUM_CBLABELS];

#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e4
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e8
#define NO_MENUS_EEPROM_LOCATION 0x3ea
#define RT_PARAMS_OFFSET_EEPROM_LOCATION 0x0350
#if 0
#define RTPARAMS_OFFSET_EEPROM_LOCATION_LSB 0x03e8	// points to just after all the labels (prompt_info)
#define RTPARAMS_OFFSET_EEPROM_LOCATION_MSB 0x03ea
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB 0x03ec	// points to just after all the labels (prompt_info)
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB 0x03ee
#endif

int rt_params_offset;
int no_cblabels;
int global_fd;
void set_state_defaults(void);
int curr_execchoice;
int last_execchoice;
int scale_type;
int prev_scale_type;

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
} CHECKBOXES;

CHECKBOXES check_boxes[NUM_CBLABELS];
CHECKBOXES prev_check_boxes[NUM_CBLABELS];

enum data_types
{
	RT_LOW,				// uint8_t without high bit set
	RT_HIGH0,			// uint8_t with bit 7 set
	RT_HIGH1,			// UINT with neither 7 or 15 set
	RT_HIGH2,			// bit 7 of UINT set
	RT_HIGH3			// bit 15 of UINT set
} DATA_TYPES;

// warning: these are also defined in the AVR directory
// got mult defined errors if enums have the same names
// but got undefined errors if left out ???

typedef struct rt_param
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// SHOWN_SENT = shown & sent; NOSHOWN_SENT = sent but not shown; NOSHOWN_NOSENT
	UCHAR dtype;		// 0 = UCHAR; 1 = UINT; 2 = dword?
	UCHAR type;			// rt_types
} RT_PARAM;

enum shown_types
{
	SHOWN_SENT,
	NOSHOWN_SENT,
	NOSHOWN_NOSENT
} SHOWN_TYPES;

enum key_types
{
	TEST1 = 0xC9,
	TEST2,			//  - CA
	TEST3,			//  - CB
	TEST4,			//  - CC
	TEST5,			//	- CD
	TEST6,			//	- CE
	TEST7,			//	- CF
	TEST8,			//	- D0
	TEST9,			//	- D1
	TEST10,			//	- D2
	TEST11,			//	- D3
	TEST12,			//	- D4
	TEST13,			//	- D5
	TEST14,			//	- D6
	TEST15,			//	- D7
	TEST16,			//	- D8
	TEST17,			//  - D9
	LOAD_RAM,		//  - DA
	INIT, 			//	- DB
	SPACE,			//	- DC
	BURN_PART,		//  - DD
	READ_EEPROM1,	//	- DE
	READ_EEPROM2,	//	- DF

	KP_POUND,	// '#'	- E0
	KP_AST, // '*'		- E1
	KP_0, // '0'		- E2
	KP_1, // '1'		- E3
	KP_2, // '2'		- E4
	KP_3, // '3'		- E5
	KP_4, // '4'		- E6
	KP_5, // '5'		- E7
	KP_6, // '6'		- E8
	KP_7, // '7'		- E9
	KP_8, // '8'		- EA
	KP_9, // '9'		- EB
	KP_A, // 'A'		- EC
	KP_B, // 'B'		- ED
	KP_C, // 'C'		- EE
	KP_D, // 'D'		- EF
} KEY_TYPES;

enum non_func_type
{
	NF_1 = 0xB0,
	NF_2,
	NF_3,
	NF_4,
	NF_5,
	NF_6,
	NF_7,
	NF_8,
	NF_9,
	NF_10
} NON_FUNC_TYPES;

enum rt_types
{
	RT_RPM = RT_OFFSET,
	RT_ENGT,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_MPH,
	RT_OILP,
	RT_MAP,
	RT_OILT,
	RT_O2,
} RT_TYPES;

enum menu_types
{
	MAIN,					// 0
	MENU1A,		// 1
	MENU1B,		// 2
	MENU1C,		// 3
	MENU1D,		// 4
	MENU1E,		// 5
	MENU2A,		// 6
	MENU2B,		// 7
	MENU2C,		// 8
	MENU2D,		// 9
	MENU2E,		// 10
	MENU3A,		// 11
	MENU3B,		// 12

	ckenter,	// 13
	ckup,		// 14
	ckdown,		// 15
	cktoggle,	// 16
	ckesc,		// 17
	cclear,		// 18

	entr,		// 19
	forward,	// 20
	back,		// 21
	eclear,		// 22
	esc,		// 23

	caps,		// 24
	small,		// 25
	spec,		// 26
	next,		// 27
	blank,		// 28

	rpm,
	engt,
	trip,
	time,
	airt,
	mph,
	oilp,
	map,
	oilt,
	o2,
	test,
} MENU_TYPES;

enum fptr_types
{
	_menu_change,
	_do_chkbox,
	_exec_choice,
	_non_func,
	_do_numentry,
	_do_init
} FPTR_TYPES;

#define NUM_MENUS 13

typedef struct menu_func
{
//	int _index;
	UCHAR fptr;								// which function to call (menu_types)
	int menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR index;
} MENU_FUNC_STRUCT;

MENU_FUNC_STRUCT menu_structs[NUM_MENUS];

#define NUM_FPTS 6

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_chkbox, do_exec, non_func, do_numentry, do_init };

/* volatile uint8_t row;
volatile uint8_t cmd;
volatile uint8_t data;
 */volatile UINT32 U32_lastCapture; // UINT32 declared in all_generic.h

volatile uint8_t col;

#define NUM_RT_LABELS NUM_RT_PARAMS

volatile int label_offsets[NUM_LABELS+NUM_RT_LABELS];

volatile uint16_t total_offset;
volatile uint16_t no_menu_labels;
volatile uint16_t no_rtparams;
volatile uint16_t no_rt_labels;

volatile UCHAR aux_string[AUX_STRING_LEN];
//volatile char state[30];

volatile char labels[NUM_LABELS][MAX_LABEL_LEN] =
//#if 0
{"home\0","MENU1a\0","MENU1b\0","MENU1c\0","MENU1d\0","MENU1e\0","MENU2a\0","MENU2B\0","MENU2c\0","MENU2d\0","MENU2e\0","MENU3a\0","MENU3b\0",\
\
"enter\0","up\0","down\0","toggle\0","esc\0","reset\0","enter\0","forward\0","back\0","clear\0","escape\0",\
\
"caps\0","small\0","spec\0","next\0",\
\
"\0","RPM\0","ENG TEMP\0","TRIP\0","TIME\0","AIR TEMP\0","MPH\0","OIL PRES\0","MAP\0","OIL TEMP\0","O2\0","test\0"};
//#endif

RT_PARAM rt_params[NUM_RT_PARAMS] = {
 {1, 0, SHOWN_SENT, 1, RT_RPM},
 {2, 0, SHOWN_SENT, 0, RT_ENGT},
 {3, 0, SHOWN_SENT, 0, RT_TRIP},
 {4, 0, SHOWN_SENT, 0, RT_TIME},
 {5, 0, SHOWN_SENT, 0, RT_AIRT},
 {1, 15, SHOWN_SENT, 0, RT_MPH},
 {2, 15, SHOWN_SENT, 0, RT_OILP},
 {3, 15, SHOWN_SENT, 0, RT_MAP},
 {4, 15, SHOWN_SENT, 0, RT_OILT},
 {5, 15, SHOWN_SENT, 0, RT_O2} };


UCHAR get_keypress(UCHAR key1)
{
	UCHAR wkey;

	switch(key1)
	{
		case '0':
			wkey = KP_0;
			break;
		case '1':
			wkey = KP_1;
			break;
		case '2':
			wkey = KP_2;
			break;
		case '3':
			wkey = KP_3;
			break;
		case '4':
			wkey = KP_4;
			break;
		case '5':
			wkey = KP_5;
			break;
		case '6':
			wkey = KP_6;
			break;
		case '7':
			wkey = KP_7;
			break;
		case '8':
			wkey = KP_8;
			break;
		case '9':
			wkey = KP_9;
			break;
		case '*':
			wkey = KP_AST;
			break;
		case '#':
			wkey = KP_POUND;
			break;
		case 'A':
		case 'a':
			wkey = KP_A;
			break;
		case 'B':
		case 'b':
			wkey = KP_B;
			break;
		case 'C':
		case 'c':
			wkey = KP_C;
			break;
		case 'D':
		case 'd':
			wkey = KP_D;
			break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
		case 'Y':
		case 'y':
			wkey = KP_POUND;
			break;
		case 'Z':
		case 'z':
			wkey = KP_AST;
			break;

		case 'g':
			wkey = TEST1;		// set size = 300, start_addr = 0
			break;
		case 'h':
			wkey = TEST2;		// set size = 300, start_addr = 300
			break;
		case 'j':
			wkey = TEST3;		// set size = 300, start_addr = 600
			break;
		case 'k':
			wkey = TEST4;		// set size = 129, start_addr = 900
			break;
		case 'l':
			wkey = TEST5;		// fill aux_string with random data
			break;
		case 'm':
			wkey = TEST6;		// draw border
			break;
		case 'n':
			wkey = TEST7;		// print cblabels
			break;
		case 'o':
			wkey = TEST8;		// restore eeprom.bin
			break;
		case 'p':
			wkey = TEST9;		// copy eeprom2.bin to eeprom.bin in sim_write
			break;
		case 'x':
			wkey = TEST10;		// read aux_string
			break;
		case 's':
			wkey = TEST11;		// tell AVR to print pattern
			break;
		case 'r':
			wkey = LOAD_RAM;	// load all stuff in ram
			break;
		case 'i':
			wkey = INIT;
			break;
		case ' ':
			wkey = SPACE;
			break;
		case 'v':
			wkey = BURN_PART;
			break;
		case 'e':
			wkey = READ_EEPROM1;
			break;
		case 'f':
			wkey = READ_EEPROM2;
			break;
#if 0
		case 'A':
			wkey = TEST12;
			break;
		case 'B':
			wkey = TEST13;
			break;
		case 'C':
			wkey = TEST14;
			break;
		case 'D':
			wkey = TEST15;
			break;
#endif
		case 'E':
			wkey = TEST16;
			break;
		case 'F':
			wkey = TEST17;
			break;
		default:
			wkey = 0xff;
			break;
	}
	return wkey;
}

#endif 
//#if 0
//******************************************************************************************//
//************************************ adv_menu_label **************************************//
//******************************************************************************************//
void adv_menu_label(int index, UCHAR *row, UCHAR *col)
{
//	int menu_index = index * 6;
	char temp[MAX_LABEL_LEN];
	char temp2[4];

	switch (index % 6)
	{
		case 0: strncpy(temp2," A:\0",4);break;
		case 1: strncpy(temp2," B:\0",4);break;
		case 2: strncpy(temp2," C:\0",4);break;
		case 3: strncpy(temp2," D:\0",4);break;
		case 4: strncpy(temp2," #:\0",4);break;
		case 5: strncpy(temp2," 0:\0",4);break;
		default:strncpy(temp2,"\0",4);break;
	}

	get_mlabel(curr_menus[index],temp);

	if(temp[0] != 0)
	{
		_dispStringAt(*row,*col,temp2);
		_dispStringAt(*row,*col+3,temp);
	}

	// if MAX_LABEL_LEN > 10 then menus will be too wide
	if(*col > MAX_LABEL_LEN*2-1)
	{
		*col = 0;
		(*row)++;
	}
	else
		*col += MAX_LABEL_LEN;
}
//******************************************************************************************//
//************************************* display_menus **************************************//
//******************************************************************************************//
static void display_menus(void)
{
	UCHAR row,col;
	row = MENU_START_ROW;
	col = MENU_START_COL;
	col = 0;

	blank_menu();
	adv_menu_label(0,&row,&col);
	adv_menu_label(1,&row,&col);
	adv_menu_label(2,&row,&col);
	adv_menu_label(3,&row,&col);
	adv_menu_label(4,&row,&col);
	adv_menu_label(5,&row,&col);
}

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;
	int menu_index = 0;
	int curr_menu;

	curr_menu = get_curr_menu();
//	strcpy((char *)state,"menu_change       \0");
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		menu_index++;
		break;
		case KP_C:
		menu_index += 2;
		break;
		case KP_D:
		menu_index += 3;
		break;
		case KP_POUND:
		menu_index += 4;
		break;
		case KP_0:
		menu_index += 5;
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
	if(ch != KP_AST)
		ret_char = set_list(menu_structs[curr_menu].menus[menu_index]);

	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR do_init(UCHAR ch)
{
	init_list();
	return ch;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void init_list(void)
{
//	int i = 0;
//	UCHAR k;
#if 0
	alnum_array[i++] = 33;		// first one is a '!'
	for(k = 34;k < 48;k++)		// '"' - '/'	14
		alnum_array[i++] = k;
	alnum_array[i++] = 32;		// 'space'		1
	for(k = 58;k < 65;k++)		// ':' - '@'	7
		alnum_array[i++] = k;
	for(k = 91;k < 97;k++)		// '[' - '`'	6
		alnum_array[i++] = k;
	for(k = 123;k < 127;k++)	// '{' - '~'	4
		alnum_array[i++] = k;
	for(k = 65;k < 91;k++)		// 'A' - 'Z'	26
		alnum_array[i++] = k;
	for(k = 97;k < 123;k++)		// 'a' - 'z'	26	total: 85 (NUM_ALNUM)
		alnum_array[i++] = k;
	choose_alnum = 0x30;
	scroll_ptr = 25;		// start showing at 'a' (skip '!' - '9')
	cur_alnum_col = NUM_ENTRY_BEGIN_COL;
#endif
	list_size = LIST_SIZE;
	current_fptr = first_menu;
	prev_fptr = first_menu;
	memset((void *)menu_list,0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	dirty_flag = 0;
	curr_checkbox = 0;
	last_checkbox = NUM_CHECKBOXES-1;
//	last_execchoice = NUM_EXECCHOICES-1;
//	scale_type = 0;
//	prev_scale_type = 1;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	display_menus();
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int prev_list(void)
{
	int ret;

	if(current_fptr == 0)
		ret = -1;
	else
	{
		menu_list[current_fptr] = 0;
		current_fptr--;
		dirty_flag = 1;
		ret = 0;
	}
	return ret;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR set_list(int fptr)
{
	int k;
	UCHAR ret_char = 0;
//	char tlabel[MAX_LABEL_LEN];

	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;

		ret_char = get_curr_menu_index();
		if(menu_structs[get_curr_menu()].fptr != _menu_change)
		{
			k = get_curr_menu_index();
			switch(ret_char)
			{
				case MENU1C:	// 1st do_chkbox
				case MENU1D:	// 2nd do_chkbox
					init_checkboxes(k);
				break;
				case MENU1E:
				case MENU2A:
				case MENU2B:
					init_checkboxes(k);
				break;
				case MENU2C:	// do_numentry
				case MENU2D:
				case MENU2E:
				case MENU3A:
				case MENU3B:
					init_numentry(ret_char);
				break;
				default:
				break;
			}
		}
	}
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR do_numentry(UCHAR ch)
{
	UCHAR ret_char = ch;

	
//	strcpy((char *)state,"do_numentry     \0");

	switch(ch)
	{
		case KP_0:
		cursor_forward_stuff(0);
		break;
		case KP_1:
		cursor_forward_stuff(1);
		break;
		case KP_2:
		cursor_forward_stuff(2);
		break;
		case KP_3:
		cursor_forward_stuff(3);
		break;
		case KP_4:
		cursor_forward_stuff(4);
		break;
		case KP_5:
		cursor_forward_stuff(5);
		break;
		case KP_6:
		cursor_forward_stuff(6);
		break;
		case KP_7:
		cursor_forward_stuff(7);
		break;
		case KP_8:
		cursor_forward_stuff(8);
		break;
		case KP_9:
		cursor_forward_stuff(9);
		break;
		case KP_A:
		cursor_forward(ch);
		break;
		case KP_B:
		backspace(ch);
		break;
		case KP_C:
		_eclear();
		break;
		case KP_D:
		enter(ch);
		prev_list();
		break;
		case KP_POUND:
		prev_list();
		break;
		case KP_AST:
			prev_list();
		break;
		default:
		break;
	}
	return ret_char;
}
//******************************************************************************************//
//************************************ clean_disp_num **************************************//
//******************************************************************************************//
static void clean_disp_num(void)
{
	int i;
	for(i = 0;i < NUM_ENTRY_SIZE+1;i++)
	{
		_dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL,0x20);
//		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL+1,0x20);
	}
}
//******************************************************************************************//
//******************************************* escape ***************************************//
//******************************************************************************************//
UCHAR escape(UCHAR ch)
{
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	cur_col = NUM_ENTRY_BEGIN_COL;
	clean_disp_num();
	return ch;
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
UCHAR enter(UCHAR ch)
{
	int limit;
	int index;
	if(1)
//	if(data_entry_mode)
	{
		limit = atoi((const char *)cur_global_number);
		if(limit > 32767)
			strcpy((char *)cur_global_number,"32766\0");
		memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
		cur_col = NUM_ENTRY_BEGIN_COL;

		index = get_curr_menu_index() - MENU2C;
		sample_numbers[index] = atoi((const char *)new_global_number);
	}
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	return ch;
}
//******************************************************************************************//
//********************************* display_edit_value *************************************//
//******************************************************************************************//
static void display_edit_value(void)
{
	_dispStringAt(NUM_ENTRY_ROW,NUM_ENTRY_BEGIN_COL,(char *)cur_global_number);
	cur_col = strlen((const char *)cur_global_number)+NUM_ENTRY_BEGIN_COL;
//	dispCharAt(NUM_ENTRY_ROW,cur_col,'x');
    _dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
	_dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
}
//******************************************************************************************//
//*************************************** init_numentry ************************************//
//******************************************************************************************//
void init_numentry(int menu_index)
{
	int temp_int;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
//	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);

	temp_int = sample_numbers[menu_index-MENU2C];
	sprintf((char *)cur_global_number,"%4d",temp_int);
	aux_string[AUX_STRING_LEN-1] = (UCHAR)temp_int;
	temp_int >>= 8;
	aux_string[AUX_STRING_LEN-2] = (UCHAR)temp_int;
	cur_col = strlen((const char *)cur_global_number)+NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
void cursor_forward_stuff(char x)
{
//	if(data_entry_mode)
	if(1)
	{
		stuff_num(x);
		cursor_forward(x);
	}
}
//******************************************************************************************//
//*************************************** stuff_num ****************************************//
//******************************************************************************************//
void stuff_num(char num)
{
	num += 0x30;
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
void cursor_forward(char ch)
{
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//***************************************** backspace **************************************//
//******************************************************************************************//
UCHAR backspace(UCHAR ch)
{
//	if(data_entry_mode)
	if(1)
	{
		cursor_backward();
		cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
	}
	return ch;
}
//******************************************************************************************//
//**************************************** _eclear *******************************************//
//******************************************************************************************//
void _eclear(void)
{
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
void cursor_backward(void)
{
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR do_exec(UCHAR ch)
{
	UCHAR ret_char = ch;
	int j,k;

	k = get_curr_menu_index() * NUM_CHECKBOXES;

	switch(ch)
	{
		case KP_A:
			j = scrollup_execchoice(k);
		break;
		case KP_B:
			j = scrolldown_execchoice(k);
		break;
		case KP_C:
		break;
		case KP_D:
			prev_list();
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
			prev_list();
			blank_choices();
		break;
		default:
		break;
	}

	return ret_char;
}

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR do_chkbox(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i, j, k;
	k = get_curr_menu_index() * NUM_CHECKBOXES;

	switch(ch)
	{
		case KP_A:
			j = scrollup_checkboxes(k);
		break;
		case KP_B:
			j = scrolldown_checkboxes(k);
		break;
		case KP_C:
			j = toggle_checkboxes(k);
		break;
		case KP_D:		// enter
			for(i = 0;i < NUM_CHECKBOXES;i++)
				check_boxes[i].checked = prev_check_boxes[i].checked;
			prev_list();
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
				prev_check_boxes[i].checked = check_boxes[i].checked;
			prev_list();
		break;
		case KP_0:
			j = checkboxes_reset(k);
		break;
		case KP_AST:
			prev_list();
		break;
		default:
		break;
	}

	return ret_char;
}
//******************************************************************************************//
//************************************* init_checkboxes ************************************//
//******************************************************************************************//
static void init_checkboxes(int index )
{
	int i,j;
	UCHAR row, col;
	char tlabel[MAX_LABEL_LEN];
//	scale_disp(SCALE_DISP_SOME);
	row = 0;
	col = 3;
	curr_checkbox = 0;

	blank_display();

	j = curr_chkbox_index * NUM_CHECKBOXES;


	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		get_cblabel(j+i,tlabel);
		_dispStringAt(row,col,tlabel);

		if(check_boxes[j+i].checked > 0)
		{
			_dispCharAt(row,0,0x78);
			check_boxes[j+i].checked = 1;
		}
		else
		{
			_dispCharAt(row,0,0x5F);	// underscore
			check_boxes[j+i].checked = 0;
		}
		row++;
	}
}
//******************************************************************************************//
//*********************************** init_execchoices *************************************//
//******************************************************************************************//
static void init_execchoices(int menu_index)
{
	int i,j;
	UCHAR row, col;
	char tlabel[MAX_LABEL_LEN];
//	scale_disp(SCALE_DISP_SOME);
	row = 0;
	col = 3;
	curr_checkbox = 0;

	blank_display();

	j = curr_chkbox_index * NUM_CHECKBOXES;

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		get_cblabel(j+i,tlabel);
		_dispStringAt(row,col,tlabel);
		row++;
	}
}
//******************************************************************************************//
//*********************************** checkboxes_reset *************************************//
//******************************************************************************************//
static UCHAR checkboxes_reset(int index)
{
//	int k = index+curr_checkbox;
	int i,k;
	curr_checkbox = 0;
	k = curr_chkbox_index * NUM_CHECKBOXES;

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[k+i].checked = 0;
		curr_checkbox++;
		k++;
		_dispCharAt(1+curr_checkbox,20,0x5F);
	}
	curr_checkbox = 0;
	return k;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static int scrollup_checkboxes(int index)
{
	int k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(curr_checkbox,20,0x5F);
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,3,tlabel);
	_dispSetMode(ATTR_REVERSE);

	if(--curr_checkbox < 0)
		curr_checkbox = 9;
	// move cursor to 3 + check_boxes[curr_checkbox]
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;

	get_cblabel(k,tlabel);
	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,4,tlabel);
	_dispSetMode(ATTR_REVERSE);

//	dispCharAt(1+check_boxes[k].index,20,0x21);
	return k;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static int scrolldown_checkboxes(int index)
{
	int k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(1+curr_checkbox,20,0x20);

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,3,tlabel);
	_dispSetMode(ATTR_REVERSE);

	if(++curr_checkbox > 9)
		curr_checkbox = 0;

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,4,tlabel);
	_dispSetMode(ATTR_REVERSE);

	return k;
}
//******************************************************************************************//
//********************************** toggle_checkboxes *************************************//
//******************************************************************************************//
static int toggle_checkboxes(int index)
{
	int k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	_dispCharAt(1+curr_checkbox,20,0x20);

	if(check_boxes[k].checked == 1)
	{
		check_boxes[k].checked = 0;
		_dispCharAt(curr_checkbox,0,0x5f);	// display '_'
	}
	else
	{
		check_boxes[k].checked = 1;
		_dispCharAt(curr_checkbox,0,120);	// display 'x'
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2,"toggle: ind %d curr %d ckbox %d  k: %d   ",
						curr_chkbox_index, curr_checkbox,check_boxes[k].index,k);
	wrefresh(win);
#endif
	return k;
}
//******************************************************************************************//
//********************************** scrollup_execchoice ***********************************//
//******************************************************************************************//
static int scrollup_execchoice(int ch)
{
	int k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(curr_checkbox,20,0x5F);
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
//	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,3,tlabel);
//	_dispSetMode(ATTR_REVERSE);

	if(--curr_checkbox < 0)
		curr_checkbox = 9;
	// move cursor to 3 + check_boxes[curr_checkbox]
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;

	get_cblabel(k,tlabel);
//	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,4,tlabel);
//	_dispSetMode(ATTR_REVERSE);

//	dispCharAt(1+check_boxes[k].index,20,0x21);
	return k;

/*
	UCHAR temp = curr_execchoice;
	if(--curr_execchoice < 0)
		curr_execchoice = last_execchoice;

//	mvwprintw(win, DISP_OFFSET+22,2,"%d %d   ",temp,curr_execchoice);
//	wrefresh(win);

	dispCharAt(1+check_boxes[temp].index,0,0x20);
	dispCharAt(1+check_boxes[curr_execchoice].index,0,120);
	return ch;
*/
}
//******************************************************************************************//
//********************************* scrolldown_execchoice **********************************//
//******************************************************************************************//
static int scrolldown_execchoice(int ch)
{
	int k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(1+curr_checkbox,20,0x20);

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
//	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,3,tlabel);
//	_dispSetMode(ATTR_REVERSE);

	if(++curr_checkbox > 9)
		curr_checkbox = 0;

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
//	_dispSetMode(ATTR_REVERSE);
	_dispStringAt(curr_checkbox,3,blank);
	_dispStringAt(curr_checkbox,4,tlabel);
//	_dispSetMode(ATTR_REVERSE);

	return k;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR non_func(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		ret_char = NF_1;
		break;
		case KP_B:
		ret_char = NF_2;
		break;
		case KP_C:
		ret_char = NF_3;
		break;
		case KP_D:
		ret_char = NF_4;
		break;
		case KP_POUND:
		ret_char = NF_5;
		break;
		case KP_0:
		ret_char = NF_6;
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR get_fptr(void)
{
	return menu_structs[get_curr_menu()].fptr;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void get_fptr_label(char *str)
{
//	char tlabel[MAX_LABEL_LEN];
//	return menu_labels[menu_structs[get_curr_menu()].fptr+total_no_menu_labels];
//	get_label(menu_structs[get_curr_menu()].fptr+total_no_menu_labels,tlabel);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_curr_menu(void)
{
	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_curr_menu_index(void)
{
	return menu_structs[get_curr_menu()].index;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int curr_fptr_changed(void)
{
	return 0;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_choices(void)
{
	int row,col,i;
	row = 0;
	col = 0;
	char blank[] = "                        ";
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		_dispStringAt(row,col,blank);
		row++;
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_display(void)
{
	int row,col;
	for(row = 0;row < 13;row++)
		for(col = 0;col < COLUMN;col++)
			_dispCharAt(row,col,0x20);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_menu(void)
{
	int row,col;
	for(row = MENU_START_ROW;row < MENU_START_ROW+2;row++)
		for(col = 1;col < COLUMN;col++)
			_dispCharAt(row,col,0x20);
}

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#if 1
ESOS_USER_TASK(keypad);
ESOS_USER_TASK(poll_keypad);
ESOS_SEMAPHORE(key_sem);
ESOS_SEMAPHORE(comm2_sem);
ESOS_USER_TASK(menu_task);
ESOS_USER_TASK(send_comm1);
//ESOS_USER_TASK(test1);
ESOS_USER_TASK(send_comm2);
ESOS_USER_TASK(get_comm2);
ESOS_USER_TASK(poll_comm1);
ESOS_USER_TASK(get_sync);
//ESOS_USER_TASK(fast_echo_spi_task);

/*
RE0 - p93 - 5th from right inside top			//	row 0
RE1 - p94 - 4th from right outside top			//	row 1
RE2 - p98 - 2nd from right outside top			//	row 2
RE3 - p99 - 2nd from right inside top			//	row 3

RA0 - p17 - 5th up - outside right				//	col 0
RA1 - p38 - 7th from right - outside bottom		//	col 1
RA2 - p58 - 4th up from bottom - inside left	//	col 2
RA3 - p59 - 5th up from bottom - outside left	//	col 3
*/

#define C0 _RA0
#define C1 _RA1
#define C2 _RA2
#define C3 _RA3

// test

static inline void CONFIG_COLUMN()
{
	CONFIG_RA0_AS_DIG_INPUT();
	ENABLE_RA0_PULLUP();
	CONFIG_RA1_AS_DIG_INPUT();
	ENABLE_RA1_PULLUP();
	CONFIG_RA2_AS_DIG_INPUT();
	ENABLE_RA2_PULLUP();
	CONFIG_RA3_AS_DIG_INPUT();
	ENABLE_RA3_PULLUP();
}

#define R0 _LATE0
#define R1 _LATE1
#define R2 _LATE2
#define R3 _LATE3

#define CONFIG_R0_DIG_OUTPUT() CONFIG_RE0_AS_DIG_OUTPUT()
#define CONFIG_R1_DIG_OUTPUT() CONFIG_RE1_AS_DIG_OUTPUT()
#define CONFIG_R2_DIG_OUTPUT() CONFIG_RE2_AS_DIG_OUTPUT()
#define CONFIG_R3_DIG_OUTPUT() CONFIG_RE3_AS_DIG_OUTPUT()

void CONFIG_ROW()
{
	CONFIG_R0_DIG_OUTPUT();
	CONFIG_R1_DIG_OUTPUT();
	CONFIG_R2_DIG_OUTPUT();
	CONFIG_R3_DIG_OUTPUT();
}

static inline void DRIVE_ROW_LOW()
{
	R0 = 0;
	R1 = 0;
	R2 = 0;
	R3 = 0;
}

static inline void DRIVE_ROW_HIGH()
{
	R0 = 1;
	R1 = 1;
	R2 = 1;
	R3 = 1;
}

typedef enum
{
	STATE_WAIT_FOR_PRESS = 0x30,
	STATE_WAIT_FOR_PRESS2,
	STATE_WAIT_FOR_RELEASE,
} ISRSTATE;

volatile ISRSTATE e_isrState;
volatile uint8_t u8_newKey = 0;

void configKeypad(void)
{
	CONFIG_ROW();
	DRIVE_ROW_LOW();
	CONFIG_COLUMN();
	DELAY_US(1);								  //wait for pullups to stabilize inputs
	e_isrState = STATE_WAIT_FOR_PRESS;
}

//drive one row low
void setOneRowLow(uint8_t u8_x)
{
	switch (u8_x)
	{
		case 0:
			R0 = 0;
			R1 = 1;
			R2 = 1;
			R3 = 1;
			break;
		case 1:
			R0 = 1;
			R1 = 0;
			R2 = 1;
			R3 = 1;
			break;
		case 2:
			R0 = 1;
			R1 = 1;
			R2 = 0;
			R3 = 1;
			break;
		default:
			R0 = 1;
			R1 = 1;
			R2 = 1;
			R3 = 0;
			break;
	}
}
#define NUM_ROWS 4
#define NUM_COLS 4
const uint8_t au8_keyTable[NUM_ROWS][NUM_COLS] =
{
	{KP_1, KP_2, KP_3, KP_A },
	{KP_4, KP_5, KP_6, KP_B },
	{KP_7, KP_8, KP_9, KP_C },
	{KP_AST, KP_0, KP_POUND, KP_D}
};
//	{'1', '2', '3', 'A'},
//	{'4', '5', '6', 'B'},
//	{'7', '8', '9', 'C'},
//	{'*', '0', '#', 'D'}

#define KEY_PRESSED() (!C0 || !C1 || !C2 || !C3)  //any low
#define KEY_RELEASED() (C0 && C1 && C2 && C3)	  //all high

uint8_t doKeyScan(void)
{
	uint8_t u8_row, u8_col;
//determine column
	if (!C0) u8_col = 0;
	else if (!C1) u8_col = 1;
	else if (!C2) u8_col = 2;
	else if (!C3) u8_col = 3;
	else return('E');							  //error
//determine row
	for (u8_row = 0; u8_row < NUM_ROWS; u8_row++)
	{
		setOneRowLow(u8_row);					  //enable one row low
		if (KEY_PRESSED())
		{
			DRIVE_ROW_LOW();					  //return rows to driving low
			return(au8_keyTable[u8_row][u8_col]);
		}
	}
	DRIVE_ROW_LOW();							  //return rows to driving low
	return('E');								  //error
}

//******************************************************************************************//
//*************************************** keypad *******************************************//
//******************************************************************************************//
ESOS_USER_TASK(keypad)
{

    ESOS_TASK_BEGIN();
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("keypad task");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
	while(TRUE)
	{
		ESOS_TASK_WAIT_TICKS(1);
#if 0
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(e_isrState);
		ESOS_TASK_WAIT_ON_SEND_UINT8(e_isrState);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		switch (e_isrState)
		{
			case STATE_WAIT_FOR_PRESS:
				if (KEY_PRESSED() && (u8_newKey == 0))
				{
	//ensure that key is sampled low for two consecutive interrupt periods
#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("wait for press");
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
					e_isrState = STATE_WAIT_FOR_PRESS2;
				}
				break;
			case STATE_WAIT_FOR_PRESS2:
				if (KEY_PRESSED())
				{
	// a key is ready
#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("key pressed");
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
					u8_newKey = doKeyScan();
					e_isrState = STATE_WAIT_FOR_RELEASE;
				} else e_isrState = STATE_WAIT_FOR_PRESS;
				break;

			case STATE_WAIT_FOR_RELEASE:
	//keypad released
				if (KEY_RELEASED())
				{
					ESOS_SIGNAL_SEMAPHORE(key_sem,1);
#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("key released");
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
					e_isrState = STATE_WAIT_FOR_PRESS;
				}
				break;
			default:
				e_isrState = STATE_WAIT_FOR_PRESS;
				break;
		}
	}
	ESOS_TASK_END();
}

//******************************************************************************************//
//************************************** poll_keypad ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(poll_keypad)
{
    static ESOS_TASK_HANDLE cmd_param_task;

	
    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(menu_task);

	configKeypad();
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("poll keypad task");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/

	while (TRUE)
	{
		ESOS_TASK_WAIT_SEMAPHORE(key_sem,1);
		if (u8_newKey)
		{
			__esos_CB_WriteUINT8(cmd_param_task->pst_Mailbox->pst_CBuffer,u8_newKey);
//#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_newKey-0xE2);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
//#endif
			u8_newKey = 0;
		}
	}
    ESOS_TASK_END();
}
#endif
//******************************************************************************************//
//************************************** poll_comm1  ***************************************//
//******************************************************************************************//

ESOS_USER_TASK(poll_comm1)
{
    static ESOS_TASK_HANDLE menu_task_handle;
    static ESOS_TASK_HANDLE send_comm_handle;
//    static ESOS_TASK_HANDLE get_sync_handle;
    static ESOS_TASK_HANDLE get_comm_handle;
    static uint8_t key;
    static uint8_t wkey;
    static int i,j,k;
    static uint8_t low_byte, high_byte;

	
    ESOS_TASK_BEGIN();
    menu_task_handle = esos_GetTaskHandle(menu_task);
    send_comm_handle = esos_GetTaskHandle(send_comm2);
    get_comm_handle = esos_GetTaskHandle(get_comm2);
//	get_sync_handle = esos_GetTaskHandle(get_sync);
	_size = -1;

	j = k = 0;
//	memset(aux_string,0,AUX_STRING_LEN);
	for(i = 0;i < NUM_LABELS;i++)
	{
		j = strlen((const char *)labels[i]);
		memcpy((void *)aux_string+k, (void *)&labels[i],j);
		k++;
		*(aux_string+j+k) = 0;
		k += j;
	}
	for(i = 240;i < AUX_STRING_LEN-240;i++)
		aux_string[i] = i;

	while (TRUE)
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(key);
		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8(key);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		wkey = get_keypress(key);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(wkey);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


		if(wkey == TEST1)
		{
			low_byte = TEST1;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
		}
		else if(wkey == TEST2)
		{
			low_byte = TEST2;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');

			for(i = TEMP_EEPROM_OFFSET; i < AUX_STRING_LEN;i++)
			{
				if(aux_string[i] > 0x1f && aux_string[i] < 0x7e)
					ESOS_TASK_WAIT_ON_SEND_UINT8(aux_string[i]);
				else if(aux_string[i] == 0)
					ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
				else if( aux_string[i] == 0xff)
					ESOS_TASK_WAIT_ON_SEND_UINT8('_');
				else
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(aux_string[i]);
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		}
		else if(wkey == TEST3)
		{
			low_byte = TEST3;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			for(i = 0;i < 400;i++)
			{
				if(aux_string[i] > 0x1f && aux_string[i] < 0x7e)
					ESOS_TASK_WAIT_ON_SEND_UINT8(aux_string[i]);
				else if(aux_string[i] == 0)
					ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);
				else if( aux_string[i] == 0xff)
					ESOS_TASK_WAIT_ON_SEND_UINT8('_');
				else
					ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(aux_string[i]);
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		}
		else if(wkey == TEST4)
		{
			low_byte = TEST4;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
		}
		else if(wkey == TEST5)
		{
			low_byte = TEST5;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);

			for(i = 0;i < AUX_STRING_LEN;i++)
				aux_string[i] = ~i - random_data;
			random_data++;

/* 			for(i = 0;i < AUX_STRING_LEN;i++)
			{
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,aux_string[i]);
			}
 */		}
		else if(wkey == TEST6)
		{
			low_byte = TEST6;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
		}

		else if(wkey == TEST7)
		{
			low_byte = TEST7;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
		}
		else if(wkey == TEST8)
		{
		}
		else if(wkey == TEST9)
		{
		}
		else if(wkey == TEST10)
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_STRING("read aux_string from target");
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
		else if(wkey == TEST11)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}
		else if(wkey == TEST12)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}
		else if(wkey == TEST13)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}
		else if(wkey == TEST14)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}
		else if(wkey == TEST15)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}
		else if(wkey == TEST16)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}
		else if(wkey == TEST17)
		{
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,wkey);
		}

		else if(wkey == LOAD_RAM)
		{
			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,cblabels[i]);
			}
			for(i = 0;i < NUM_CBLABELS;i++)
			{
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,check_boxes[i].checked);
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,check_boxes[i].index);
				memcpy(&prev_check_boxes[i],&check_boxes[i],sizeof(CHECKBOXES));
			}
			for(i = 0;i < NUM_RT_PARAMS;i++)
			{
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,rt_params[i].row);
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,rt_params[i].col);
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,rt_params[i].shown);
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,rt_params[i].dtype);
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,rt_params[i].type);
			}
			for(i = 0;i < 5;i++)
			{
				j = sample_numbers[i];
				unpack(j,&low_byte,&high_byte);				
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,high_byte);
			}
		}
		if(wkey == BURN_PART)
		{
#if 0
			if(_size == -1)
			{
				_size = 234;
				start_addr = 0;
			}

			low_byte = BURN_PART;

			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);

			unpack(_size,&low_byte,&high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);

			unpack(start_addr,&low_byte,&high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(_size>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)_size);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			for(i = 0;i < _size;i++)
			{
				__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,aux_string[i]);
				ESOS_TASK_WAIT_TICKS(20);
			}
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)i);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		}

		else if(wkey == READ_EEPROM1)
		{
#if 0
			avr_ptr = 0;
//			ESOS_KILL_TASK(get_comm_handle);
/*
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(_size>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)_size);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)(start_addr>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((UCHAR)start_addr);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
			low_byte = READ_EEPROM;
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);

			unpack(_size,&low_byte,&high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);

			unpack(start_addr,&low_byte,&high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,high_byte);
			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
/*
			for(i = 0;i < _size;i++)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
				ESOS_TASK_WAIT_ON_GET_UINT82(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(low_byte);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}
			ESOS_RESTART_TASK(get_comm_handle);
*/

#endif
		}
		else if(wkey == INIT)
		{
			_size = 234;
			start_addr = 0;
			ESOS_RESTART_TASK(send_comm_handle);
			ESOS_RESTART_TASK(get_comm_handle);
			ESOS_RESTART_TASK(menu_task_handle);
			ESOS_TASK_RESTART();
		}
		else if(wkey == SPACE)
		{
			low_byte = SPACE;
//			__esos_CB_WriteUINT8(send_comm_handle->pst_Mailbox->pst_CBuffer,low_byte);
		}
		else
			__esos_CB_WriteUINT8(menu_task_handle->pst_Mailbox->pst_CBuffer,wkey);

	}
    ESOS_TASK_END();
}
//#if 0
//******************************************************************************************//
//********************************** CONFIG_SPI_SLAVE **************************************//
//******************************************************************************************//
static void CONFIG_SPI_SLAVE(void)
{
  SPI1CON1 = SEC_PRESCAL_8_1		|		//8:1 secondary prescale
             PRI_PRESCAL_64_1		|		//64:1 primary prescale
             CLK_POL_ACTIVE_HIGH	|		//clock active high (CKP = 0)
             SPI_CKE_ON				|		//out changes active to idle (CKE=1)
             SPI_MODE8_ON			|		//8-bit mode
//             SPI_SMP_ON				|		// input data sampled at end of data output time
             SLAVE_ENABLE_ON;				//master mode

	CONFIG_SDI1_TO_RP(RD14_RP);
	CONFIG_RD14_AS_DIG_INPUT();
	CONFIG_SDO1_TO_RP(RD15_RP);
	CONFIG_RD15_AS_DIG_OUTPUT();
	CONFIG_SCK1IN_TO_RP(RF4_RP);
	CONFIG_RF4_AS_DIG_INPUT();
	CONFIG_SS1IN_TO_RP(RF5_RP);
	CONFIG_RF5_AS_DIG_INPUT();
	SPI1STATbits.SPIEN = 1;  //enable SPI mode
}
volatile uint8_t  cmd_array[24] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//******************************************************************************************//
//**************************************** echo_spi ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(echo_spi_task)
{
	static uint8_t data1;
//	static uint8_t data2;
	static uint8_t cmd;
	static uint8_t i;

//	static char rpm_disp[5];
//	static char mph_disp[5];
//	static uint16_t res;
	static uint8_t res2;
//	static uint16_t res3, res4;
//	static char res_str[10];
//	static char res2_str[10];

	static uint8_t  res_array[24];

    ESOS_TASK_BEGIN();

//#if 0
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("echo_spi_task on comm1");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	ESOS_TASK_SIGNAL_AVAILABLE_SPI();
//#endif


	data1 = 0x21;
    while(TRUE)
	{

//		ESOS_TASK_WAIT_TICKS(TDELAY2);
//#if 0
		for(i = 0;i < 24;i++)
		{
			cmd = cmd_array[i];
			ESOS_TASK_WAIT_ON_AVAILABLE_SPI();
			ESOS_TASK_WAIT_ON_READ1SPI1(data1);
			ESOS_TASK_WAIT_ON_WRITE1SPI1(cmd);
			ESOS_TASK_SIGNAL_AVAILABLE_SPI();

			if(res_array[i] != data1)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(i);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}
			res_array[i] = data1;

			if(i == 0)
				ESOS_TASK_WAIT_TICKS(TDELAY2);
			res2++;
			if((res2 % 300) == 0)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(res2>>8));
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}

#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			if(i == 2)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
	//        ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
		    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			switch(i)
			{
				case 2:
					rpm_disp[0] = data1 + 0x30;
					break;
				case 3:
					rpm_disp[1] = data1 + 0x30;
					break;
				case 4:
					rpm_disp[2] = data1 + 0x30;
					break;
				case 5:
					rpm_disp[3] = data1 + 0x30;
					break;
				case 6:
					mph_disp[0] = data1 + 0x30;
					break;
				case 7:
					mph_disp[1] = data1 + 0x30;
					break;
				case 8:
					mph_disp[2] = data1 + 0x30;
					break;
				case 9:
					mph_disp[3] = data1 + 0x30;
					res = res2 = 0;
					res3 = res4 = 0;
					break;

				case 10:
					res = (uint16_t)data1;
	//					printf("%2x\t",res);
					break;
				case 11:
					res2 = (uint16_t)data1;
	//					printf("%2x\t",res2);
					res2 <<= 8;
					res2 &= 0xff00;
					res |= res2;
					break;
				case 12:
					res3 = data1;
					break;
				case 13:
					res4 = data1;
					res4 <<= 8;
					res4 &= 0xff00;
					res3 |= res4;
					break;
				case 15:
					ESOS_TASK_WAIT_ON_SEND_STRING(rpm_disp);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					ESOS_TASK_WAIT_ON_SEND_STRING(mph_disp);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					sprintf(res_str,"%4d",res);
					sprintf(res2_str,"%4d",res2);
					ESOS_TASK_WAIT_ON_SEND_STRING(res);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					ESOS_TASK_WAIT_ON_SEND_STRING(res2);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					break;
			}
#endif
		}
    }
    ESOS_TASK_END();
}

#define NUM_CHANNELS 2
volatile uint16_t au16_buffer[NUM_CHANNELS];
volatile uint8_t  u8_waiting;

//******************************************************************************************//
//**************************************** convADC  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(convADC)
{
    static  uint8_t data1, u8_wdtState;
	static uint16_t u16_adcVal1, u16_adcVal2;
	static float f_adcVal;
	static uint8_t   u8_i;
	static uint16_t  u16_pot;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_STRING("starting ADC task\r\n");
	ESOS_TASK_WAIT_ON_SEND_STRING("                                 \r");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	CONFIG_RB0_AS_ANALOG();
	CONFIG_RB1_AS_ANALOG();

	configADC1_AutoScanIrqCH0( ADC_SCAN_AN0 | ADC_SCAN_AN1, 31, 0);
	while ( !AD1CON1bits.DONE)
		ESOS_TASK_WAIT_TICKS(1);

	while (TRUE)
	{
		while (u8_waiting)	// wait for valid data in ISR
			ESOS_TASK_WAIT_TICKS(1);
		u8_waiting = 0;
		for ( u8_i=0; u8_i<NUM_CHANNELS; u8_i++)
		{
			u16_pot = au16_buffer[u8_i];

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			if(u8_i == 0)
			{
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(u16_pot>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)u16_pot);
			ESOS_TASK_WAIT_TICKS(50);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
	}
	ESOS_TASK_END();
}
//******************************************************************************************//
//************************************ ACD1Interrupt  **************************************//
//******************************************************************************************//
void _ISR _ADC1Interrupt (void)
{
	uint8_t			u8_i;
	uint16_t*		au16_adcHWBuff = (uint16_t*) &ADC1BUF0;

	for ( u8_i=0; u8_i<NUM_CHANNELS; u8_i++)
	{
		au16_buffer[u8_i] = au16_adcHWBuff[u8_i];
	} //end for()
	u8_waiting = 0;  // signal main() that data is ready
	_AD1IF = 0;   //clear the interrupt flag
}

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int pack(UCHAR low_byte, UCHAR high_byte)
{
	int temp;
	int myint;
	low_byte = ~low_byte;
	myint = (int)low_byte;
	temp = (int)high_byte;
	temp <<= 8;
	myint |= temp;
	return myint;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte)
{
	*low_byte = (UCHAR)myint;
	myint >>= 8;
	*high_byte = (UCHAR)myint;
	*low_byte = ~(*low_byte);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index)
{
	int len;
	len = sizeof(MENU_FUNC_STRUCT);
	menu_structs[i].menus[0] = menu0;
	menu_structs[i].menus[1] = menu1;
	menu_structs[i].menus[2] = menu2;
	menu_structs[i].menus[3] = menu3;
	menu_structs[i].menus[4] = menu4;
	menu_structs[i].menus[5] = menu5;
	menu_structs[i].fptr = fptr;
	menu_structs[i].index = index;

//	total_offset += len;
	i++;
	return i;
}
//******************************************************************************************//
//*********************************** get_label_offsets ************************************//
//******************************************************************************************//
void get_label_offsets(void)
{
	int i, j;
	char *ch;
	char temp_label[MAX_LABEL_LEN];

	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		label_offsets[i] = goffset;
		j = 0;
		memcpy((void*)&temp_label[0],(void*)aux_string+goffset,MAX_LABEL_LEN);
//		eeprom_read_block(temp_label, eepromString+goffset, MAX_LABEL_LEN);
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
int get_label(int index, char *str)
{
	int i = 0;
		// void *dest, const void *src, size_t n
	memcpy((void*)str,(void*)aux_string+label_offsets[index],MAX_LABEL_LEN);
	while(str[i] != 0)
		i++;
//	eeprom_read_block((void *)str,eepromString+label_offsets[index],MAX_LABEL_LEN);
	return i;
}
//******************************************************************************************//
//*********************************** get_mlabel_offsets ***********************************//
//******************************************************************************************//
// warning this needs to be written for the AVR main
int get_mlabel_offsets(void)
{
#if 0
	int i, j,k;
	UCHAR *ch;
	UCHAR *pch;
	UCHAR temp_label[MAX_LABEL_LEN];
	int done = 0;
	size_t size;
	UCHAR tch;

	goffset = 0;


//	ch = pch = eeprom_sim;

	size = AUX_STRING_LEN;
	eeprom_read_block((void*)&aux_string[0],(const void *)eepromString,(size_t)size);
	ch = pch = aux_string;
	j = k = 0;
	for(i = 0;i < AUX_STRING_LEN;i++)
	{
		tch = aux_string[i];
		if(tch == 0)
			_dispCharAt(j,k,'_');
		else
			_dispCharAt(j,k,tch);
		if(k++ > COLUMN-1)
		{
			k = 0;
			j++;
		}
	}

	no_menu_labels = 0;
	no_rt_labels = 0;
	rt_params_offset = 0;
	i = 0;


	pch--;
// the eeprom image must have an extra '0' at the end of
// the menu labels and another extra '0' at the end
// of the rt labels
	do
	{
		if(*ch == 0)
			no_menu_labels++;
		i++;
		ch++;
		pch++;
	}while(*ch != 0 || *pch != 0);

	no_menu_labels++;

	if(i > EEPROM_SIZE-10)
		return -1;

	i = 0;
	do
	{
		if(*ch == 0)
			no_rt_labels++;
		i++;
		ch++;
		pch++;
	}while(*ch != 0 || *pch != 0);

	if(i > EEPROM_SIZE-10)
		return -1;

	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		mlabel_offsets[i] = goffset;
		j = 0;

		memcpy(temp_label,eeprom_sim+goffset,MAX_LABEL_LEN);
#if 0
		eeprom_read_block(temp_label, eepromString+goffset, MAX_LABEL_LEN);
#endif
		ch = &temp_label[0];
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
	rt_params_offset = goffset;
	return no_menu_labels;
#endif
	return 0;
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
void get_mlabel(int index, char *str)
{

		// void *dest, const void *src, size_t n

//	memcpy(str,eeprom_sim+mlabel_offsets[index],MAX_LABEL_LEN);
#if 0
	eeprom_read_block((void *)str,eepromString+mlabel_offsets[index],MAX_LABEL_LEN);
#endif
}
//******************************************************************************************//
//********************************** get_cblabel_offsets ***********************************//
//******************************************************************************************//
int get_cblabel_offsets(void)
{
	int i, j;
	char *ch;
	char temp_label[MAX_LABEL_LEN];

	goffset = 0;

	no_cblabels = 0;
	for(i = 0;i < CBLABEL_SIZE;i++)
	{
		if(cblabels[i] == 0 && cblabels[i-1] != 0)
			no_cblabels++;
	}

	for(i = 0;i < no_cblabels;i++)
	{
		cblabel_offsets[i] = goffset;
		j = 0;
		memcpy(temp_label,cblabels+goffset,MAX_LABEL_LEN);
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
	return no_cblabels;
}
//******************************************************************************************//
//************************************* get_cblabel ****************************************//
//******************************************************************************************//
void get_cblabel(int index, char *str)
{
		// void *dest, const void *src, size_t n
	memcpy(str,cblabels+cblabel_offsets[index],MAX_LABEL_LEN);
}
//******************************************************************************************//
//************************************* update_cbabels *************************************//
//******************************************************************************************//
int update_cblabels(int index, char *str)
{
	int len;
	len = strlen(str);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
	memcpy(cblabels+total_offset,str, len);
//	strncpy(menu_labels[index],ramstr,len);
	total_offset += len;
	index++;
	return index;
}

#ifdef DEBUG
void PIC_DispCharAt(UCHAR row, UCHAR col, char *c)
{
    static ESOS_TASK_HANDLE comm1_handle;
	static UCHAR low_byte = 0xaa;
    comm1_handle = esos_GetTaskHandle(send_comm1);
	
	__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,low_byte);
}

void PIC_DispStringAt(UCHAR row, UCHAR col, char *c)
{
}

void PIC_SetCursor(UCHAR mode, UINT row, UINT col, UCHAR type)
{
}
#endif
