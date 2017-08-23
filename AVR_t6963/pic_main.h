// pic_main.h
// include this before pic_main.h in the files for the PIC
// and before avr_main.h in the files for the AVR

#define LIST_SIZE 50
#define NUM_MENU_CHOICES 6
#define NUM_MENUS 12
#define NUM_MENU_STRUCTS NUM_MENUS*NUM_MENU_CHOICES

typedef struct menu_func
{
//	int _index;
	UCHAR enabled;							// if alt function will replace generic function
	UCHAR fptr;								// which function to call (menu_types)
	int menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR index;							// if > 0 then this is index into sample_data
} MENU_FUNC_STRUCT;

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
int update_menu_structs(int i, UCHAR enabled, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index);
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type);
int update_menu_labels(int i, char *ramstr);
int update_rt_labels(int index, char *ramstr);
UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
//UCHAR cursor_row, cursor_col;
int no_rt_labels;
int no_rtparams;
int no_menu_structs;
int no_menu_labels;
int no_func_labels;
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
//UINT send_data;
//UINT recv_data;
int global_fd;
void set_state_defaults(void);
int curr_checkbox;
int last_checkbox;
int scale_type;
int prev_scale_type;
UCHAR aux_index;
UCHAR new_data_ready;
UCHAR mod_data_ready;
UCHAR data_entry_mode;
UCHAR cur_param_string[NUM_UCHAR_PARAMS];
UCHAR aux_string[AUX_STRING_LEN];
UCHAR send_aux_data;
