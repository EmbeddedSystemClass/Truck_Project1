// main.h - has what's the same for both AVR_menu.c and PIC_menu.c

#define TIME_DELAY1 1
#define STRING_LEN 100
#define NUM_FPTS 6
#define MAX_LABEL_LEN 10
#define NUM_LABELS 100
#define NUM_MENU_CHOICES 6
#define NUM_RT_PARAMS 12
#define NUM_RT_LABELS NUM_RT_PARAMS
#define DISP_OFFSET 4
#define NUM_CHECKBOXES 10
#define NUM_EXECCHOICES 10
#define SCALE_DISP_ALL 0
#define SCALE_DISP_SOME 1
#define SCALE_DISP_NONE 2
#define RT_OFFSET 0x70
#define EEPROM_SIZE 0x400

#define NUM_MENUS 12
#define NUM_MENU_STRUCTS NUM_MENUS*NUM_MENU_CHOICES

char *get_label(int index, char *str);

#ifndef MAIN_C

typedef struct menu_func
{
//	int _index;
	UCHAR fptr;								// which function to call (menu_types)
	int menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR index;
} MENU_FUNC_STRUCT;

//char menu_labels[NUM_LABELS][MAX_LABEL_LEN];
//char rt_labels[NUM_RT_LABELS][MAX_LABEL_LEN];
MENU_FUNC_STRUCT menu_structs[NUM_MENU_STRUCTS];
UCHAR eeprom_sim[1023];
#else
#endif

int label_offsets[NUM_LABELS+NUM_RT_LABELS];
int goffset;

//#define AUX_STRING_LEN 300	// main AVR data is 70% full
//#define AUX_STRING_LEN 400	// main AVR data is 75% full
#define AUX_STRING_LEN 520		// main AVR data is 81% full
//#define AUX_STRING_LEN 600	// main AVR data is 85% full
//#define AUX_STRING_LEN 800	// main AVR data is 94% full
//#define AUX_STRING_LEN 900	// main AVR data is 99% full
#define LAST_ROW DISP_OFFSET+54
#define LAST_COL 63
int burn_eeprom(void);

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
	MAIN,
	MENU1A,
	MENU1B,
	MENU1C,
	MENU1D,
	MENU1E,
	MENU2A,
	MENU2B,
	MENU2C,
	MENU2D,

	ckup,
	ckdown,
	cktoggle,
	ckenter,
	ckesc,

	entr,
	caps,
	small,
	spec,
	next,
	forward,
	back,
	esc,

	choice0,
	choice1,
	choice2,
	choice3,
	choice4,
	choice5,
	choice6,
	choice7,
	choice8,
	choice9,

	exec0,
	exec1,
	exec2,
	exec3,
	exec4,
	exec5,
	exec6,
	exec7,
	exec8,
	exec9,

	blank
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
	SPACE,//		- F0
	SET_DATA1,//	- F1
	SET_DATA2,//	- F2
	SET_DATA3,//	- F3
	SET_DATA4,//	- F4
	SET_DATA5,//	- F5
	PUSH_DATA, //	- F6
	INIT, //		- F7
	TEST_RTPARAMS,//- F8
	READ_EEPROM	//	- F9
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

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
	char string[MAX_LABEL_LEN];
} CHECKBOXES;

typedef struct execchoices
{
	UCHAR index;
	char string[MAX_LABEL_LEN];
} EXEC_CHOICES;

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
void init_list(void);
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_str_len(void);
int burn_eeprom(void);
int read_eeprom(void);
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
int choice_aux_offset;
int exec_aux_offset;
int no_menu_labels;
int no_func_labels;
int no_data_index;
//UINT label_info_offset;
UINT rt_params_offset;
UINT menu_struct_offset;
//char labels[1][MAX_LABEL_LEN];
// just have 1 copy in ram and reload from eeprom every time we change menus
UCHAR get_row(int index);
UCHAR get_col(int index);
int pack(UCHAR low_byte, UCHAR high_byte);
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte);
RT_PARAM rt_params[NUM_RT_PARAMS];
#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e2
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e4
#define NO_MENUS_EEPROM_LOCATION 0x3e6
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
//int label_offsets[NUM_MENU_LABELS+NUM_RT_LABELS];
//#endif
//#endif
int total_offset;
int global_fd;
void set_state_defaults(void);
EXEC_CHOICES exec_choices[NUM_EXECCHOICES];
CHECKBOXES check_boxes[NUM_CHECKBOXES];
UCHAR prev_check_boxes[NUM_CHECKBOXES];
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

