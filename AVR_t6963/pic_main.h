// pic_main.h
// include this before pic_main.h in the files for the PIC
// and before avr_main.h in the files for the AVR

#define LIST_SIZE 50
#define LAST_ROW_DISP LAST_ROW-13
#define LAST_COL 63

void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
#ifdef TEST_WRITE_DATA
void set_win(WINDOW *win);
#endif
void init_list(void);
UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type);
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_str_len(void);
int get_mlabel_offsets(void);
int get_cblabel_offsets(void);
void update_ram(void);
UCHAR current_param;

int no_func_labels;
int choice_offset;
UCHAR get_row(int index);
UCHAR get_col(int index);

int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index);

int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type);

//#if 0
typedef struct menu_func
{
	UCHAR fptr;								// which function to call (menu_types)
	int menus[6];							// which menu to goto if _menu_change is the fptr
//	UCHAR menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR index;
} MENU_FUNC_STRUCT;

#define NUM_MENUS 13

MENU_FUNC_STRUCT menu_structs[NUM_MENUS];
//#endif

#if 0
typedef struct rt_params
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// SHOWN_SENT = shown & sent; NOSHOWN_SENT = sent but not shown; NOSHOWN_NOSENT
	UCHAR dtype;		// 0 = UCHAR; 1 = UINT; 2 = dword?
	UCHAR type;			// rt_types
} RT_PARAM;
#endif

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
int tdelay;

