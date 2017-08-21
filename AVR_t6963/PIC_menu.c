#include "sfr_helper.h"
#include "t6963.h"
#ifdef TEST_WRITE_DATA
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
WINDOW *win;
#endif
#include "pic_main.h"

void dispSetCursorX(UCHAR mode, UCHAR row, UCHAR col, UCHAR type)
{
	cursor_row = row;
	cursor_col = col;
}

static UCHAR menu_change(UCHAR ch);
static UCHAR exec_choice(UCHAR ch);
static UCHAR do_chkbox(UCHAR ch);
static UCHAR non_func(UCHAR ch);
static UCHAR start_numentry(UCHAR ch);
static UCHAR test1(UCHAR ch);
static UCHAR test2(UCHAR ch);
static UCHAR test3(UCHAR ch);
static UCHAR test4(UCHAR ch);
static UCHAR generic_menu_function(UCHAR ch);
static int first_menu = MAIN;
static int last_menu = testnum8;
static int current_fptr;		// pointer into the menu_list[]
static int prev_fptr;
static int list_size;
static int curr_type;
static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
static int inc_index;
static int test_send_num;

#ifdef TEST_WRITE_DATA
static void display_menus(int index);
#endif
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM+1];
static UCHAR choose_alnum;
static void prev_list(void);
static int menu_list[LIST_SIZE];	// works like a stack for the how deep into the menus we go
static void set_list(int fptr);
static UCHAR get_fptr(void);
static char* get_fptr_label(void);
static int get_curr_menu_index(void);
#if 0
static UCHAR (*fptr[NUM_FPTS])(UCHAR) = {enter, backspace, escape, scr_alnum0, \
		 scr_alnum1, scr_alnum2, scr_alnum3, cursor_forward, alnum_enter, scrollup_checkboxes, \
			scrolldown_checkboxes, toggle_checkboxes, enter_checkboxes, escape_checkboxes };
#endif
static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, exec_choice, do_chkbox, non_func, start_numentry, 
		test1, test2, test3, test4 };
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
char *get_rt_label(int index)
{
	return rt_labels[index];
}
/*
char *get_menu_label(int index)
{
	return menu_labels[index];
}
*/
UCHAR get_row(int index)
{
//	return menu_structs[index].row;
	return 0;
}
UCHAR get_col(int index)
{
//	return menu_structs[index].col;
	return 0;
}
//#endif

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void init_list(void)
{
	int i = 0;
	UCHAR k;

	alnum_array[i++] = 33;		// first one is a '!'
	for(k = 34;k < 48;k++)		// '"' - '/'	14
		alnum_array[i++] = k;
	alnum_array[i++] = 32;		// 'space'		1
	for(k = 58;k < 65;k++)		// ':' - '@'	7
		alnum_array[i++] = k;
	for(k = 91;k < 97;k++)		// '[' - '`'	6
		alnum_array[i++] = k;
	for(k = 123;k < 127;k++)	// '{' - '~'	4
		alnum_array[i++] = k;
	for(k = 65;k < 91;k++)		// 'A' - 'Z'	26
		alnum_array[i++] = k;
	for(k = 97;k < 123;k++)		// 'a' - 'z'	26	total: 85 (NUM_ALNUM)
		alnum_array[i++] = k;
	choose_alnum = 0x30;
	scroll_ptr = 25;		// start showing at 'a' (skip '!' - '9')
	cur_alnum_col = NUM_ENTRY_BEGIN_COL;
	list_size = LIST_SIZE;
	current_fptr = first_menu;
	prev_fptr = first_menu;
	memset(menu_list,0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	curr_type = MAIN;
	dirty_flag = 0;
	curr_checkbox = 0;
	last_checkbox = NUM_CHECKBOXES-1;
	scale_type = 0;
	prev_scale_type = 1;
	new_data_ready = 0;
	data_entry_mode = 0;
	mod_data_ready = 0;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	aux_index = 0;
	inc_index = 0;
	test_send_num = 254;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void prev_list(void)
{
	UCHAR temp = menu_list[current_fptr];
	UCHAR temp2 = current_fptr;
	if(current_fptr == 0)
		return;
	else
	{
		menu_list[current_fptr] = 0;
		current_fptr--;
//		mvwprintw(win, DISP_OFFSET+35,2,"current_fptr:  %d                                ",current_fptr);
		dirty_flag = 1;
	}
//	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#if 0
static void set_list(int fptr)
{
	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;
		dirty_flag = 1;
	}
}
#endif
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i;
	int temp;
	int menu_index;
	
	temp = get_curr_menu();
	menu_index = temp * 6;
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		menu_index++;
		break;
		case KP_C:
		menu_index += 2;
		break;
		case KP_D:
		menu_index += 3;
		break;
		case KP_POUND:
		menu_index += 4;
		break;
		case KP_0:
		menu_index += 5;
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
	if(ch != KP_AST)
		set_list(menu_structs[menu_index].menu);

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
	mvwprintw(win, DISP_OFFSET+6,2, "menu_change %d  %d              ",menu_index,menu_structs[menu_index].menu);
//	mvwprintw(win, DISP_OFFSET+6,2, "*menu_change                           ");
	wrefresh(win);
#endif

//	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
//		write(global_fd,(void*)&cur_param_string[i],1);

//	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
//		cur_param_string[i]++;

//		write(global_fd,cur_param_string,NUM_UCHAR_PARAMS);


//#if 0

	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// pass in the keypress from the PIC24 keypad and pass out the string of UCHARs
UCHAR get_key(UCHAR ch, UCHAR *str)
{
	int i;
	memcpy(cur_param_string,str,NUM_UCHAR_PARAMS);
	UCHAR ret_char = generic_menu_function(ch);


	if(curr_fptr_changed())
	{
//#if 0
#ifdef TEST_WRITE_DATA
		display_menus(get_curr_menu2());
#endif
//#endif
	}
	return ret_char;

	if(new_data_ready == 1)
	{
		data_entry_mode = 1;
//		display_edit_value();
		new_data_ready = 0;
	}
/*
#ifdef TEST_WRITE_DATA
	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
		mvwprintw(win, DISP_OFFSET+i, 4, "%x ",cur_param_string);
#endif
*/
	return ret_char;
}
//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(UCHAR ch)
{
	UCHAR ret_char;
	int i;
	int init_flag = 0;
	UCHAR temp2;
	int temp3;
	char tlabel[MAX_LABEL_LEN];


	if(menu_structs[get_curr_menu2()].enabled == 1)
	{
#if 0
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET, 2, "menu_index:  %d   ",get_curr_menu());
		mvwprintw(win, DISP_OFFSET+1, 2,"fptr:   %s   %s  ",
//				menu_structs[get_curr_menu()*6].fptr,
				menu_labels[get_curr_menu()],
				get_fptr_label());
#endif
#endif
		// execute function pointer
		ret_char = (*fptr[menu_structs[get_curr_menu2()+inc_index].fptr])(ch);
//		cur_param_string[0] = ret_char;
#ifdef TEST_WRITE_DATA
//		mvwprintw(win, DISP_OFFSET+3, 2, "menu_index:  %d   ",get_curr_menu());
		mvwprintw(win, DISP_OFFSET+4, 2,"%s  %d  %s   %d             ",
//				menu_structs[get_curr_menu()*6].fptr,
				menu_labels[get_curr_menu()],
				get_curr_menu2(),				
				get_fptr_label(),
				inc_index);
		wrefresh(win);
#endif
		for(i = 0;i < 6;i++)
		{
			memset(tlabel,0,MAX_LABEL_LEN);
//			if(menu_structs[(get_curr_menu2())+i].enabled == 1)
//			{
/*
			mvwprintw(win, DISP_OFFSET+5,2,"press space bar...");
			wrefresh(win);
			getch();
			mvwprintw(win, DISP_OFFSET+5,2,"                    ");
*/
				strcpy(tlabel,menu_labels[menu_structs[(get_curr_menu2())+i].menu]);
				memcpy(cur_param_string+(i*MAX_LABEL_LEN),tlabel,MAX_LABEL_LEN);
//			}
		}

		if(strcmp(get_fptr_label(),"mchange") == 0)
			init_flag = 0;

		if(init_flag == 0 && strcmp(get_fptr_label(),"mchange") != 0)
		{
			switch(get_curr_menu_index())
			{
				case MENU1C:
					aux_string[3] = 10;
				break;
				case MENU1D:
					aux_string[3] = 11;
				break;
				case MENU2A:
					aux_string[3] = 12;
				break;
				case MENU2B:
					aux_string[3] = 13;
				break;
				case MENU2C:
					aux_string[3] = (UCHAR)test_send_num;
					temp3 = test_send_num;
					temp3 >>= 8;
					temp2 = (UCHAR)temp3;
					aux_string[4] = temp2;
					test_send_num++;
				break;
				default:
				break;
			}
		}

		aux_string[2] = init_flag;

		temp2 = get_fptr();
		write(global_fd,&temp2,1);
		write(global_fd,&ch,1);
		write(global_fd,cur_param_string,NUM_UCHAR_PARAMS);
		aux_string[0] = get_curr_menu2();
		write(global_fd,aux_string,AUX_STRING_LEN);
	}
//#if 0
#ifdef TEST_WRITE_DATA
		for(i = 0;i < current_fptr;i++)
			mvwprintw(win, DISP_OFFSET+8+i, 2,"%d -> %s  ",menu_list[i],menu_labels[menu_list[i]]);

		mvwprintw(win, DISP_OFFSET+8+i, 2,"%d -> %s  ",menu_list[i],menu_labels[get_curr_menu()]);
		mvwprintw(win, DISP_OFFSET+8+i+1, 2,"                                  ");
#endif
//#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR exec_choice(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i;
	int menu_index = 0;

	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		menu_index++;
		break;
		case KP_C:
		menu_index += 2;
		break;
		case KP_D:
		menu_index += 3;
		break;
		case KP_POUND:
		menu_index += 4;
		break;
		case KP_0:
		menu_index += 5;
		break;
		case KP_AST:
		prev_list();
		inc_index = 0;
		break;
		default:
		break;
	}
	if(ch != KP_AST)
		inc_index = menu_index;

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+6,2, "exec_choice %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
	mvwprintw(win, DISP_OFFSET+6,2, "*exec_choice: %x     ",ch);
	wrefresh(win);
#endif
//	for(i = 0;i < AUX_STRING_LEN;i++)
//		aux_string[i]++;
	return ret_char;
}

static UCHAR test1(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		inc_index = 0;
		break;
		default:
		break;
	}
	mvwprintw(win, DISP_OFFSET+6,2, "*test 1: %x         ",ch);
	return ret_char;
}
static UCHAR test2(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		inc_index = 0;
		break;
		default:
		break;
	}
	mvwprintw(win, DISP_OFFSET+6,2, "*test 2: %x         ",ch);
	return ret_char;
}
static UCHAR test3(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		inc_index = 0;
		break;
		default:
		break;
	}
	mvwprintw(win, DISP_OFFSET+6,2, "*test 3: %x         ",ch);
	return ret_char;
}
static UCHAR test4(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		inc_index = 0;
		break;
		default:
		break;
	}
	mvwprintw(win, DISP_OFFSET+6,2, "*test 4: %x         ",ch);
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_chkbox(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i;
	
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+6,2, "do_chkbox %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
	mvwprintw(win, DISP_OFFSET+6,2, "*do_chkbox 	               ");
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR non_func(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+6,2, "non_func %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
	mvwprintw(win, DISP_OFFSET+6,2, "*non_func %x              ",ch);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR start_numentry(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_0:
		case KP_1:
		case KP_2:
		case KP_3:
		case KP_4:
		case KP_5:
		case KP_6:
		case KP_7:
		case KP_8:
		case KP_9:
			ret_char = ch - 0xE2;
		break;	
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+6,2, "non_func %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
	mvwprintw(win, DISP_OFFSET+6,2, "*edit_value %x              ",ch);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#ifdef TEST_WRITE_DATA
static void display_menus(int index)
{
	int i;
	mvwprintw(win, DISP_OFFSET+23,2,"%d  ",index);
	for(i = 0;i < 6;i++)
	{
		if(menu_structs[index+i].enabled == 1)
			mvwprintw(win, DISP_OFFSET+24+i, 2,"%s   ",menu_labels[menu_structs[index+i].menu]);
	}
}
#endif
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR get_fptr(void)
{
	return menu_structs[get_curr_menu2()+inc_index].fptr;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static char* get_fptr_label(void)
{
	return menu_labels[menu_structs[get_curr_menu2()].fptr+no_menu_labels];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_curr_menu(void)
{
	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_curr_menu2(void)
{
	return menu_list[current_fptr]*6;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static int get_curr_menu_index(void)
{
	return menu_structs[get_curr_menu2()].index;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int curr_fptr_changed(void)
{
	int flag = dirty_flag;
	dirty_flag = 0;
	return flag;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_str_len(void)
{
	return cur_col-NUM_ENTRY_BEGIN_COL;
}

//#if 0
static void set_list(int fptr)
{
	int temp = current_fptr;

//#if 0
#ifdef TEST_WRITE_DATA
	if(menu_structs[get_curr_menu()].menu > last_menu)
	{
		mvwprintw(win, DISP_OFFSET+36,2," overflow   ");
	}else mvwprintw(win, DISP_OFFSET+36,2,"            ");
#endif
//#endif
	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;
		dirty_flag = 1;
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+35,2,"fptr:  %d current_fptr: %d (before)  %d (after)     ",fptr,temp,current_fptr);
#endif
	}
#ifdef TEST_WRITE_DATA
	else mvwprintw(win, DISP_OFFSET+26, 2, "oopps");
#endif
#ifdef TEST_WRITE_DATA
	wrefresh(win);
#endif
}
//#endif


