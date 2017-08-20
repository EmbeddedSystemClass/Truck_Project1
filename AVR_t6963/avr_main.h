// avr_main.h

#define TIME_DELAY1 1
#define STRING_LEN 100
#define NUM_FPTS 4
#define MAX_LABEL_LEN 9
#define NUM_MENU_LABELS 34
#define NUM_RT_PARAMS 12
#define NUM_RT_LABELS NUM_RT_PARAMS
#define NUM_MENU_CHOICES 6
#define NUM_MENUS 9
#define NUM_MENU_STRUCTS NUM_MENUS*NUM_MENU_CHOICES+1 // plus 1 for ENDMENU
#define DISP_OFFSET 5
#define NUM_CHECKBOXES 10
#define SCALE_DISP_ALL 0
#define SCALE_DISP_SOME 1
#define SCALE_DISP_NONE 2
#define RT_OFFSET 0x70
#define LIST_SIZE 50
#define NUM_UCHAR_PARAMS 10

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

typedef struct menu_func
{
//	int _index;
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// which menu to goto if _menu_change is the fptr
	UCHAR label;		// which label to display in legend (labels)
	UCHAR index;		// if > 0 then this is index into sample_data
} MENU_FUNC_STRUCT;

enum menu_types
{
	MAIN,
	MENU1A,
	MENU1B,
	MENU1C,
	MENU1D,
	MENU2A,
	MENU2B,
	MENU2C,
	MENU2D,
	MENU3A,
	MENU3B,
	MENU3C,
	MENU3D,
	MENU4A,
	MENU4B,
	MENU4C,
	MENU4D,
	testnum0,
	testnum1,
	testnum2,
	testnum3,
	testnum4,
	testnum5,
	testnum6,
	testnum7,
	testnum8,
	ENDMENU
} MENU_TYPES;

// total of 20 menus

enum fptr_types
{
	_menu_change,
	_exec_choice,
	_do_chkbox,
	_non_func,
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
} FPTR_TYPES;

enum key_types
{
	KP_POUND = 0xE0, // '#'
	KP_AST, // '*'
	KP_0, // '0'
	KP_1, // '1'
	KP_2, // '2'
	KP_3, // '3'
	KP_4, // '4'
	KP_5, // '5'
	KP_6, // '6'
	KP_7, // '7'
	KP_8, // '8'
	KP_9, // '9'
	KP_A, // 'A'
	KP_B, // 'B'
	KP_C, // 'C'
	KP_D, // 'D'	(0xEF)
	KP_SIM_DATA		// simulates sending data 
} KEY_TYPES;

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
	RT_AUX1,
	RT_AUX2
} RT_TYPES;

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
	char string[20];
} CHECKBOXES;

UINT send_data;
UINT recv_data;
int global_fd;
#define NUM_ENTRY_SIZE 7
#define AUX_DATA_SIZE 4
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
#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e2
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e4
#define NO_MENUS_EEPROM_LOCATION 0x3e6
#define RTPARAMS_OFFSET_EEPROM_LOCATION_LSB 0x03e8	// points to just after all the labels (prompt_info)
#define RTPARAMS_OFFSET_EEPROM_LOCATION_MSB 0x03ea
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB 0x03ec	// points to just after all the labels (prompt_info)
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB 0x03ee
#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
void set_win(WINDOW *win);
void init_list(void);
UCHAR get_key(UCHAR ch, UCHAR *str);
UCHAR read_get_key(UCHAR *str);
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_str_len(void);
int burn_eeprom(void);
int read_eeprom(void);
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type);
int update_menu_structs(int i, UCHAR enabled, UCHAR fptr, UCHAR menu, UCHAR label, UCHAR index);
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type);
int update_menu_labels(int i, char *ramstr);
int update_rt_labels(int index, char *ramstr);
UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
UCHAR cursor_row, cursor_col;
int no_rt_labels;
int no_rtparams;
int no_menu_structs;
int no_menu_labels;
int no_data_index;
//UINT label_info_offset;
UINT rt_params_offset;
UINT menu_struct_offset;
char menu_labels[NUM_MENU_LABELS][MAX_LABEL_LEN];
char rt_labels[NUM_RT_LABELS][MAX_LABEL_LEN];
//char labels[1][MAX_LABEL_LEN];
// just have 1 copy in ram and reload from eeprom every time we change menus
MENU_FUNC_STRUCT menu_structs[NUM_MENU_STRUCTS];
char *get_label(int index);
UCHAR get_row(int index);
UCHAR get_col(int index);
RT_PARAM rt_params[NUM_RT_PARAMS];
// define a separate rt_params for the write part of test_write_data.c_str
// because we want to handle this as if a separate array is running on the PIC24
//#ifdef NOAVR
// we could read the labels into ram when in AVR mode but its just as easy to read them from
// eeprom directly - doesn't take that much more time, plus it saves ram space in AVR
// not worried about it when compiling in NOAVR mode on a linux box in 32/64 bit mode.
// label_offsets is an array that get set to the length of each label by searching
// for the first 0
#ifdef EEPROM_BURN
int label_offsets[NUM_MENU_LABELS+NUM_RT_LABELS];
#endif
int total_offset;
//#endif
char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
//int sample_data[10];
UINT send_data;
UINT recv_data;
int global_fd;
void set_state_defaults(void);
CHECKBOXES check_boxes[NUM_CHECKBOXES];
int curr_checkbox;
int last_checkbox;
int scale_type;
int prev_scale_type;
UCHAR aux_index;
UCHAR new_data_ready;
UCHAR mod_data_ready;
UCHAR data_entry_mode;

UCHAR cur_param_string[NUM_UCHAR_PARAMS];

