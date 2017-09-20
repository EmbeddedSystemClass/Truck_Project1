// avr_main.h

#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_START_ROW 12
#define MENU_START_COL 15
#define MENU_BLANK "          "

CHECKBOXES check_boxes[NUM_CHECKBOXES];

//#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
//#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
#ifdef TEST_WRITE_DATA
void set_win(WINDOW *win);
#endif
void init_list(void);
UCHAR read_get_key(UCHAR key);
UCHAR parse_sync(UCHAR key);
int get_str_len(void);
int burn_eeprom(void);
int read_eeprom(void);
UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
//UCHAR cursor_row, cursor_col;
//UINT label_info_offset;
char rt_labels[NUM_RT_LABELS][MAX_LABEL_LEN];
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
//#endif
char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
//int sample_data[10];
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
int menu_index;
int curr_menus[6];
int syncup;
UCHAR sync_buf[10];
