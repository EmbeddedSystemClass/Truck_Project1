// pic_main.h
// include this before pic_main.h in the files for the PIC
// and before avr_main.h in the files for the AVR

#define LIST_SIZE 50
#define LAST_ROW_DISP LAST_ROW-11
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

/*
MENU_FUNC_STRUCT menu_structs[] = {
//												'A' 	'B'		'C'		'D'		'#'		'0'
	{ _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU1E, MENU2A,  MAIN  },
// 1a
	{ 	_menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A  },
// 1b
	{ 	_menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B  },
// 1c
	{ 	_exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU1C  },
// 1d
	{ 	_do_chkbox, 	ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1D  },
// 1e
	{ 	_do_chkbox, 	ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1E  },
// 2a
	{ 	_non_func, 		blank, blank, blank, blank, blank, blank, MENU2A  },
// 2b
	{ 	_exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2B  },
// 2c
	{ 	_do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2C  },
// 2d
	{ 	_do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2D  },
// 2e
	{ 	_do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2E  },
// 3a
	{ 	_do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3A  },
// 3b
	{ 	_do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3B  }
};
*/

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
//UINT send_data;
//UINT recv_data;
int global_fd;
void set_state_defaults(void);
int curr_checkbox;
int last_checkbox;
int scale_type;
int prev_scale_type;
UCHAR aux_index;
int send_aux_data;
int tdelay;

