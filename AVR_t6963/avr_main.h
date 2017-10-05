// avr_main.h

#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_BLANK "          "

//#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
//#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void update_ram(void);
void display_labels(void);
#ifdef TEST_WRITE_DATA
void set_win(WINDOW *win);
#endif
void init_list(void);
UCHAR read_get_key(UCHAR key);
int parse_sync(void);
int get_str_len(void);
int get_mlabel_offsets(void);
int get_cblabel_offsets(void);
UCHAR current_param;
//UCHAR cursor_row, cursor_col;
//UINT label_info_offset;
char rt_labels[NUM_RT_LABELS][MAX_LABEL_LEN];
UCHAR get_row(int index);
UCHAR get_col(int index);
char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
//int sample_data[10];
int global_fd;
void set_state_defaults(void);
int scale_type;
int prev_scale_type;
int menu_index;
UCHAR curr_menus[6];
int syncup;
UCHAR sync_buf[10];
int bp;
int tdelay;
int size, start_addr;

