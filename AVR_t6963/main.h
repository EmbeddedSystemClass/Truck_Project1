// main.h - has what's the same for both AVR_menu.c and PIC_menu.c

#define TIME_DELAY1 1
#define NUM_FPTS 6
#define MAX_LABEL_LEN 10
#define NUM_LABELS 60
#define NUM_MENU_CHOICES 6
#define NUM_RT_PARAMS 12
#define NUM_RT_LABELS NUM_RT_PARAMS
#define DISP_OFFSET 4
#define NUM_EXECCHOICES 10
#define SCALE_DISP_ALL 0
#define SCALE_DISP_SOME 1
#define SCALE_DISP_NONE 2
#define RT_OFFSET 0x70
#define EEPROM_SIZE 0x400

char *get_label(int index, char *str);

int label_offsets[NUM_LABELS+NUM_RT_LABELS];
int goffset;

#define NUM_CHECKBOXES 10
#define TOTAL_NUM_CHECKBOXES NUM_CHECKBOXES*4
#define CHECKBOX_STRING_LEN 20

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
	char string[CHECKBOX_STRING_LEN];
} CHECKBOXES;

#define AUX_STRING_LEN 1024

#ifdef TEST_WRITE_DATA
UCHAR eeprom_sim[EEPROM_SIZE];
#endif

#define LAST_ROW DISP_OFFSET+54
#define LAST_ROW_DISP LAST_ROW-12
#define LAST_COL 63
int burn_eeprom(void);
int burn_eeprom2(void);

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
	SHOWN_SENT,
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

	entr,		// 18
	forward,	// 19
	back,		// 20
	eclear,		// 21
	esc,		// 22

	caps,		// 23
	small,		// 24
	spec,		// 25
	next,		// 26

	blank,		// 27
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
	test
} MENU_TYPES;

// total of 20 menus

enum fptr_types
{
	_menu_change,
	_exec_choice,
	_do_chkbox,
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
	KP_POUND = 0xE0, // '#'
	KP_AST, // '*'	- E1
	KP_0, // '0'	- E2
	KP_1, // '1'	- E3
	KP_2, // '2'	- E4
	KP_3, // '3'	- E5
	KP_4, // '4'	- E6
	KP_5, // '5'	- E7
	KP_6, // '6'	- E8
	KP_7, // '7'	- E9
	KP_8, // '8'	- EA
	KP_9, // '9'	- EB
	KP_A, // 'A'	- EC
	KP_B, // 'B'	- ED
	KP_C, // 'C'	- EE
	KP_D, // 'D'	- EF
	INIT, //		- F0
	READ_EEPROM,//	- F1
	SPACE,	//		- F2
	SHOW_EEPROM, //	- F3
	SHOW_MENU_STRUCT, //- F4
	LOAD_MENU_STRUCT, // - F5
	BURN_PART,		// - F6
	BURN_PART1,		// - F7
	BURN_PART2,		// - F8
	BURN_PART3		// - F9
} KEY_TYPES;

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
void get_label_offsets(void);
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type);
int update_labels(int i, char *ramstr);
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
int menu_offset;
int rt_params_offset;
int global_fd;
void set_state_defaults(void);
int curr_checkbox;
int last_checkbox;
int curr_execchoice;
int last_execchoice;
int scale_type;
int prev_scale_type;
UCHAR aux_index;
UCHAR new_data_ready;
UCHAR mod_data_ready;
UCHAR data_entry_mode;
UCHAR aux_string[AUX_STRING_LEN];

