// main.h - has what's the same for both AVR_menu.c and PIC_menu.c

#define TIME_DELAY1 1
#define NUM_FPTS 6
#define MAX_LABEL_LEN 10
#define NUM_LABELS 60
#define NUM_MENU_CHOICES 6
#define NUM_RT_PARAMS 10
#define NUM_RT_LABELS NUM_RT_PARAMS

#define DISP_OFFSET 1
#define LAST_ROW DISP_OFFSET + 58
#define NUM_EXECCHOICES 10
#define SCALE_DISP_ALL 0
#define SCALE_DISP_SOME 1
#define SCALE_DISP_NONE 2
#define RT_OFFSET 0x70
#define EEPROM_SIZE 0x400
#define TOTAL_NUM_CBLABELS 80
#define CBLABEL_SIZE 500

void get_mlabel(int index, char *str);
void get_cblabel(int index, char *str);

int mlabel_offsets[NUM_LABELS+NUM_RT_LABELS];
int cblabel_offsets[TOTAL_NUM_CBLABELS];
int goffset;

#define NUM_CHECKBOXES 10

int checkbox_offsets[TOTAL_NUM_CBLABELS];

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
} CHECKBOXES;

#define AUX_STRING_LEN EEPROM_SIZE/8	// 128

#ifdef TEST_WRITE_DATA
UCHAR eeprom_sim[EEPROM_SIZE];
#endif

#ifdef SHOW_EEPROM
int burn_eeprom(void);
#endif
void update_ram(void);

char cblabels[CBLABEL_SIZE];

typedef struct rt_params
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// SHOWN_SENT = shown & sent; NOSHOWN_SENT = sent but not shown; NOSHOWN_NOSENT
	UCHAR dtype;		// 0 = UCHAR; 1 = UINT; 2 = dword?
	UCHAR type;			// rt_types
} RT_PARAM;

enum shown_types
{
	SHOWN_SENT = 0x14,
	NOSHOWN_SENT,
	NOSHOWN_NOSENT
} SHOWN_TYPES;

enum menu_types
{
	MAIN,		// 0
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

// total of 20 menus

enum fptr_types
{
	_menu_change,
	_do_chkbox,
	_exec_choice,
	_non_func,
	_do_numentry
/*
	entr,
	back,
	esc,
	caps,
	small,
	spec,
	next,
	cur_for,
	alnum_ent,
	ckup,
	ckdown,
	cktoggle,
	ckenter,
	ckesc
*/
} FPTR_TYPES;

// warning: these are also defined in the esos1 directory
enum key_types
{
	SYNC = 0xCF,
	TEST1,			//	- D0
	TEST2,			//  - D1
	TEST3,			//  - D2
	TEST4,			//  - D3
	TEST5,			//	- D4
	TEST6,			//	- D5
	TEST7,			//	- D6
	LOAD_RAM,		//  - D7
	INIT, 			//	- D8
	SPACE,			//	- D9
	BURN_PART,		//  - DA
	BURN_PART1,		//  - DB
	BURN_PART2,		//  - DC
	BURN_PART3,		//  - DD
	BURN_PART4,		//  - DE
	READ_EEPROM,	//	- DF

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
	TEST9,	//			- F0
	TEST10,	//			- F1
	TEST11,	//			- F2
	TEST12,	//			- F3
	TEST13,	//			- F4
	TEST14,	//			- F5
	TEST15,	//			- F6
	TEST16,	//			- F7
	TEST17	//			- F8
} KEY_TYPES;


#if 0

	KP_POUND = 0xE0,
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
	INIT, //			- F0
	SPACE,	//			- F1
	SHOW_EEPROM, //		- F2
	SHOW_MENU_STRUCT,// - F3
	LOAD_MENU_STRUCT,// - F4
	READ_EEPROM,//		- F5
	BURN_PART,		//  - F6
	BURN_PART1,		//  - F7
	BURN_PART2,		//  - F8
	BURN_PART3,		//  - F9
	BURN_PART4,		//  - FA
	TEST1,			//  - FB
	TEST2,			//  - FC
	TEST3,			//  - FD
	TEST4			//  - FE
#endif

enum non_func_types
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

// we already don't need the string elem in the PIC so when we find a way to pass the strings
// in via the aux_string we can do away with string elem

int global_fd;
#define NUM_ENTRY_SIZE 7
//#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/2)
#define NUM_ENTRY_BEGIN_COL 3
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
#define NUM_ENTRY_ROW 8
// '!' - '9' (33 - 58) + 'A' - 'Z' (26) + 'a' - 'z' (26) = 77
//#define NUM_ALNUM 77
//#define NUM_ALNUM 52		// without the '!' - '9'
#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_START_ROW 12
#define MENU_START_COL 15
#define MENU_BLANK "          "
#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
#ifdef TEST_WRITE_DATA
void set_win(WINDOW *win);
#endif
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_str_len(void);
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type);
int update_mlabels(int i, char *ramstr);
int update_cblabels(int i, char *ramstr);
UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
//UCHAR cursor_row, cursor_col;
int no_rt_labels;
int no_rtparams;
int total_no_menu_labels;
int no_func_labels;
int no_menu_structs;
int no_menu_labels;

//char labels[1][MAX_LABEL_LEN];
// just have 1 copy in ram and reload from eeprom every time we change menus
UCHAR get_row(int index);
UCHAR get_col(int index);
int pack(UCHAR low_byte, UCHAR high_byte);
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte);
RT_PARAM rt_params[NUM_RT_PARAMS];
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
// define a separate rt_params for the write part of test_write_data.c_str
// because we want to handle this as if a separate array is running on the PIC24
//#ifdef NOAVR
// we could read the labels into ram when in AVR mode but its just as easy to read them from
// eeprom directly - doesn't take that much more time, plus it saves ram space in AVR
// not worried about it when compiling in NOAVR mode on a linux box in 32/64 bit mode.
// label_offsets is an array that get set to the length of each label by searching
// for the first 0
//#ifdef EEPROM_BURN
//#endif
//#endif
int total_offset;
int rt_params_offset;
int no_cblabels;
int global_fd;
void set_state_defaults(void);
int curr_execchoice;
int last_execchoice;
int scale_type;
int prev_scale_type;
UCHAR aux_index;
UCHAR new_data_ready;
UCHAR mod_data_ready;
UCHAR data_entry_mode;
UCHAR aux_string[AUX_STRING_LEN];
int sample_numbers[5];

