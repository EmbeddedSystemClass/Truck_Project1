// main.h - has what's the same for both AVR_menu.c and PIC_menu.c

#define TIME_DELAY1 1
#define NUM_FPTS 6
#define MAX_LABEL_LEN 10
#define NUM_LABELS 30
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
#define EEPROM_SIZE 0x400
#define NUM_CBLABELS 40
#define CBLABEL_SIZE 400		// AVR data is 97% full

void get_mlabel(int index, char *str);
void get_cblabel(int index, char *str);

int mlabel_offsets[NUM_LABELS+NUM_RT_LABELS];
int cblabel_offsets[NUM_CBLABELS];
int goffset;

#define NUM_CHECKBOXES 10

int checkbox_offsets[NUM_CBLABELS];

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
} CHECKBOXES;

CHECKBOXES check_boxes[NUM_CBLABELS];
CHECKBOXES prev_check_boxes[NUM_CBLABELS];

#define AUX_STRING_LEN 300

#ifdef TEST_WRITE_DATA
UCHAR eeprom_sim[EEPROM_SIZE];
#endif

#ifdef SHOW_EEPROM
int burn_eeprom(void);
#endif

char cblabels[CBLABEL_SIZE];

#if 0
typedef struct menu_func
{
	UCHAR fptr;								// which function to call (menu_types)
//	int menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR index;
} MENU_FUNC_STRUCT;


#define NUM_MENUS 13

MENU_FUNC_STRUCT menu_structs[NUM_MENUS];
#endif
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
	SHOWN_SENT = 05,
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
#define NUM_ENTRY_ROW 10
// '!' - '9' (33 - 58) + 'A' - 'Z' (26) + 'a' - 'z' (26) = 77
//#define NUM_ALNUM 77
//#define NUM_ALNUM 52		// without the '!' - '9'
#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_START_ROW 14
#define MENU_START_COL 3
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
int update_mlabels(int i, char *ramstr);
int update_cblabels(int i, char *ramstr);
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
UCHAR aux_string[AUX_STRING_LEN];
int sample_numbers[5];

