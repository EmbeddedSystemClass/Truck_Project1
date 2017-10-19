#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include "test.priv.h"
#include <form.h>
#include "form_sup.h"

/***************************************************************************/
FieldAttrs *field_attrs(FIELD * f)
{
	return (FieldAttrs *) field_userptr(f);
}

/***************************************************************************/
int buffer_length(FIELD * f)
{
	return field_attrs(f)->row_lengths[0];
}

/***************************************************************************/
void set_buffer_length(FIELD * f, int length)
{
	field_attrs(f)->row_lengths[0] = length;
}

/***************************************************************************/
/*
 * The userptr is used in edit_field.c's inactive_field(), as well as for
 * keeping track of the actual lengths of lines in a multiline field.
 */
void init_edit_field(FIELD * f, char *value,int nbufs)
{
	char empty[1];
	FieldAttrs *ptr = field_attrs(f);
	if (ptr == 0)
	{
		int rows, cols, frow, fcol, nrow, nbuf;

		ptr = typeCalloc(FieldAttrs, (size_t) 1);
		ptr->background = field_back(f);
		if (field_info(f, &rows, &cols, &frow, &fcol, &nrow, &nbuf) == E_OK)
		{
			ptr->row_count = nrow;
			ptr->row_lengths = typeCalloc(int, (size_t) nrow + 1);
		}
	}
	if (value == 0)
	{
		value = empty;
		*value = '\0';
	}
	set_field_userptr(f, (void *) ptr);
	if(nbufs == 1)
	{
		set_field_buffer(f, 0, value);				  /* will be formatted */
	}else if(nbufs == 2)
	{
		set_field_buffer(f, 1, value);				  /* will be unformatted */
		set_field_buffer(f, 0, value);				  /* will be formatted */
	}
	set_buffer_length(f, (int) strlen(value));
}

/***************************************************************************/
int wGetchar(WINDOW *win)
{
	int c;
#ifdef TRACE
#warning "TRACE defined"
	while ((c = wgetch(win)) == CTRL('T'))
	{
		if (_nc_tracing)
		{
			save_trace = _nc_tracing;
			Trace(("TOGGLE-TRACING OFF"));
			_nc_tracing = 0;
		}
		else
		{
			_nc_tracing = save_trace;
		}
		trace(_nc_tracing);
		if (_nc_tracing)
			Trace(("TOGGLE-TRACING ON"));
	}
#else
	c = wgetch(win);
#endif
	return c;
}

//#define Getchar() wGetchar(stdscr)

/***************************************************************************/
FIELD *make_label(int frow, int fcol, NCURSES_CONST char *label)
{
	FIELD *f = new_field(1, (int) strlen(label), frow, fcol, 0, 0);

	if (f)
	{
		set_field_buffer(f, 0, label);
		set_field_opts(f, (int) ((unsigned) field_opts(f) & ~O_ACTIVE));
//		set_field_fore(f, COLOR_PAIR(3));
//		set_field_back(f, COLOR_PAIR(4));
	}
	return (f);
}

/***************************************************************************/
FIELD *make_field(int frow, int fcol, int rows, int cols, int nbufs)
{
//	FIELD *f = new_field(rows, cols, frow, fcol, 0, secure ? 1 : 0);
	FIELD *f = new_field(rows, cols, frow, fcol, 0, nbufs);

	if (f)
	{
		set_field_back(f, A_UNDERLINE);
		field_opts_on(f, O_AUTOSKIP | O_ACTIVE | O_EDIT );
//		set_field_userptr(f, (void *) 0);
//		set_field_fore(f, COLOR_PAIR(1));
//		set_field_back(f, COLOR_PAIR(2));
	}
	return (f);
}

/***************************************************************************/
void display_form(FORM * f)
{
	WINDOW *w;
	int rows, cols;
	int x,y;

	scale_form(f, &rows, &cols);

//	if ((w = newwin(rows + 2, cols + 4, 0, 0)) != (WINDOW *) 0)
	if ((w = newwin(rows+2, cols + 4, FORM_WIN_STARTY, FORM_WIN_STARTX)) != (WINDOW *) 0)
	{
		set_form_win(f, w);
//		set_form_sub(f, derwin(w, rows, cols, 1, 2));
		set_form_sub(f, derwin(w, rows, cols, 1, 2));
//		box(w, 0,0);
		box(w, 0, 0);
//		box(w, STARTY, STARTX);
		keypad(w, TRUE);
	}

/*
	x = STARTX;
	y = STARTY;
	mvprintw(LINES - 30, 0, "display_forms: %d  %d  %d  %d         ",rows,cols,x,y);
	refresh();
*/
	if (post_form(f) != E_OK)
		wrefresh(w);
}

/***************************************************************************/
void erase_form(FORM * f)
{
	WINDOW *w = form_win(f);
	WINDOW *s = form_sub(f);

	unpost_form(f);
	werase(w);
	wrefresh(w);
	delwin(s);
	delwin(w);
}

/***************************************************************************/
int edit_secure(FIELD * me, int c)
{
	int rows, cols, frow, fcol, nrow, nbuf;
	int firet;

//	MvPrintw(4, COLS - 10,"1:%c ",c);
//	refresh();

	firet = field_info(me, &rows, &cols, &frow, &fcol, &nrow, &nbuf);
//	MvPrintw(5, COLS - 10,"%d %d  ",nbuf,firet);
//	refresh();

	if(firet == E_OK && nbuf > 0)
	{
		char *source = field_buffer(me, 1);
		char temp[80];
		long len;

//		MvPrintw(6, COLS - 10,"2:%c ",c);
//		refresh();

		strcpy(temp, source ? source : "");
		len = (long) (char *) field_userptr(me);
		if (c <= KEY_MAX)
		{
//			MvPrintw(7, COLS - 10,"3:%c ",c);
//			refresh();
			if (isgraph(c) && (len + 1) < (int) sizeof(temp))
			{
				temp[len++] = (char) c;
				temp[len] = 0;
				set_field_buffer(me, 1, temp);
				c = '*';
			}
			else
			{
				c = 0;
			}
		}
		else
		{
			switch (c)
			{
				case REQ_BEG_FIELD:
				case REQ_CLR_EOF:
				case REQ_CLR_EOL:
				case REQ_DEL_LINE:
				case REQ_DEL_WORD:
				case REQ_DOWN_CHAR:
				case REQ_END_FIELD:
				case REQ_INS_CHAR:
				case REQ_INS_LINE:
				case REQ_LEFT_CHAR:
				case REQ_NEW_LINE:
				case REQ_NEXT_WORD:
				case REQ_PREV_WORD:
				case REQ_RIGHT_CHAR:
				case REQ_UP_CHAR:
					c = 0;						  /* we don't want to do inline editing */
					break;
				case REQ_CLR_FIELD:
					if (len)
					{
						temp[0] = 0;
						set_field_buffer(me, 1, temp);
					}
					break;
				case REQ_DEL_CHAR:
				case REQ_DEL_PREV:
					if (len)
					{
						temp[--len] = 0;
						set_field_buffer(me, 1, temp);
					}
					break;
			}
		}
		set_field_userptr(me, (void *) len);
	}
	return c;
}

/***************************************************************************/
int form_virtualize(FORM * f, WINDOW *w)
{
/* *INDENT-OFF* */
/* *INDENT-ON* */

	static int mode = REQ_INS_MODE;
	int c = wGetchar(w);
	unsigned n;
	FIELD *me = current_field(f);
	bool current = TRUE;

	if (c == CTRL(']'))
	{
		if (mode == REQ_INS_MODE)
		{
			mode = REQ_OVL_MODE;
		}
		else
		{
			mode = REQ_INS_MODE;
		}
		c = mode;
	}
	else
	{
		for (n = 0; n < SIZEOF(lookup); n++)
		{
			if (lookup[n].code == c)
			{
				c = lookup[n].result;
//				print_req_type(w, c,2, COLS-20);
				break;
			}
		}
	}
//	MvPrintw(0, COLS - 6, "(%s)", mode == REQ_INS_MODE ? "INS" : "OVL");
/*
 * Force the field that the user is typing into to be in reverse video,
 * while the other fields are shown underlined.
 */
	switch (c)
	{
		case REQ_BEG_FIELD:
		case REQ_CLR_EOF:
		case REQ_CLR_EOL:
		case REQ_CLR_FIELD:
		case REQ_DEL_CHAR:
		case REQ_DEL_LINE:
		case REQ_DEL_PREV:
		case REQ_DEL_WORD:
		case REQ_END_FIELD:
		case REQ_INS_CHAR:
		case REQ_INS_LINE:
		case REQ_LEFT_CHAR:
		case REQ_LEFT_FIELD:
		case REQ_NEXT_WORD:
		case REQ_RIGHT_CHAR:
			current = TRUE;
			break;
		default:
			current = (c < KEY_MAX);
			MvPrintw(1, COLS - 20,"%d %d  ",c,current);
			break;
	}
//	print_req_type(w, c,2, COLS-20);
	c = edit_secure(me, c);
	if (current)
	{
		set_field_back(me, A_REVERSE);
	}
	else
	{
		set_field_back(me, A_UNDERLINE);
	}
	return c;
}

/***************************************************************************/
int my_form_driver(FORM * form, int c)
{
	if (c == (MAX_FORM_COMMAND + 1)
		&& form_driver(form, REQ_VALIDATION) == E_OK)
		return (TRUE);
	else if(c == KEY_F(3))
		return (TRUE);
	else if (c == QUIT)
		return 2;
	else
	{
//		print_req_type(form_win(form), c,60, 1);
		return (FALSE);
	}
}

/***************************************************************************/
void print_req_type(WINDOW *win, int c, int line, int col)
{
	char buffer[40];
	int print_this = 1;
//	MvAddStr(8,57,"test");
	switch(c)
	{
		case REQ_NEXT_PAGE:			/* move to next page		*/
			strcpy(buffer,"REQ_NEXT_PAGE\0");
			break;
		case REQ_PREV_PAGE:					/* move to previous page	*/
			strcpy(buffer,"REQ_PREV_PAGE\0");
			break;
		case REQ_FIRST_PAGE:				/* move to first page		*/
			strcpy(buffer,"REQ_FIRST_PAGE\0");
			break;
		case REQ_LAST_PAGE:			/* move to last page		*/
			strcpy(buffer,"REQ_LAST_PAGE\0");
			break;

		case REQ_NEXT_FIELD:			/* move to next field		*/
			strcpy(buffer,"REQ_NEXT_FIELD\0");
			break;
		case REQ_PREV_FIELD:			/* move to previous field	*/
			strcpy(buffer,"REQ_PREV_FIELD\0");
			break;
		case REQ_FIRST_FIELD:		/* move to first field		*/
			strcpy(buffer,"REQ_FIRST_FIELD\0");
			break;
		case REQ_LAST_FIELD:			/* move to last field		*/
			strcpy(buffer,"REQ_LAST_FIELD\0");
			break;
		case REQ_SNEXT_FIELD:		/* move to sorted next field	*/
			strcpy(buffer,"REQ_SNEXT_FIELD\0");
			break;
		case REQ_SPREV_FIELD:		/* move to sorted prev field	*/
			strcpy(buffer,"REQ_SPREV_FIELD\0");
			break;
		case REQ_SFIRST_FIELD:		/* move to sorted first field	*/
			strcpy(buffer,"REQ_SFIRST_FIELD\0");
			break;
		case REQ_SLAST_FIELD:		/* move to sorted last field	*/
			strcpy(buffer,"REQ_SLAST_FIELD\0");
			break;
		case REQ_LEFT_FIELD:		/* move to left to field	*/
			strcpy(buffer,"REQ_LEFT_FIELD\0");
			break;
		case REQ_RIGHT_FIELD:		/* move to right to field	*/
			strcpy(buffer,"REQ_RIGHT_FIELD\0");
			break;
		case REQ_UP_FIELD:			/* move to up to field		*/
			strcpy(buffer,"REQ_UP_FIELD\0");
			break;
		case REQ_DOWN_FIELD:		/* move to down to field	*/
			strcpy(buffer,"REQ_DOWN_FIELD\0");
			break;

		case REQ_NEXT_CHAR:			/* move to next char in field	*/
			strcpy(buffer,"REQ_NEXT_CHAR\0");
			break;
		case REQ_PREV_CHAR:			/* move to prev char in field	*/
			strcpy(buffer,"REQ_PREV_CHAR\0");
			break;
		case REQ_NEXT_LINE:			/* move to next line in field	*/
			strcpy(buffer,"REQ_NEXT_LINE\0");
			break;
		case REQ_PREV_LINE:			/* move to prev line in field	*/
			strcpy(buffer,"REQ_PREV_LINE\0");
			break;
		case REQ_NEXT_WORD:			/* move to next word in field	*/
			strcpy(buffer,"REQ_NEXT_WORD\0");
			break;
		case REQ_PREV_WORD:			/* move to prev word in field	*/
			strcpy(buffer,"REQ_PREV_WORD\0");
			break;
		case REQ_BEG_FIELD:			/* move to first char in field	*/
			strcpy(buffer,"REQ_BEG_FIELD\0");
			break;
		case REQ_END_FIELD:			/* move after last char in fld	*/
			strcpy(buffer,"REQ_END_FIELD\0");
			break;
		case REQ_BEG_LINE:			/* move to beginning of line	*/
			strcpy(buffer,"REQ_BEG_LINE\0");
			break;
		case REQ_END_LINE:			/* move after last char in line	*/
			strcpy(buffer,"REQ_END_LINE\0");
			break;
		case REQ_LEFT_CHAR:			/* move left in field		*/
			strcpy(buffer,"REQ_LEFT_CHAR\0");
			break;
		case REQ_RIGHT_CHAR:		/* move right in field		*/
			strcpy(buffer,"REQ_RIGHT_CHAR\0");
			break;
		case REQ_UP_CHAR:			/* move up in field		*/
			strcpy(buffer,"REQ_UP_CHAR2\0");
			break;
		case REQ_DOWN_CHAR:			/* move down in field		*/
			strcpy(buffer,"REQ_DOWN_CHAR\0");
			break;

		case REQ_NEW_LINE:			/* insert/overlay new line	*/
			strcpy(buffer,"REQ_NEW_LINE\0");
			break;
		case REQ_INS_CHAR:			/* insert blank char at cursor	*/
			strcpy(buffer,"REQ_INS_CHAR\0");
			break;
		case REQ_INS_LINE:			/* insert blank line at cursor	*/
			strcpy(buffer,"REQ_INS_LINE\0");
			break;
		case REQ_DEL_CHAR:			/* delete char at cursor	*/
			strcpy(buffer,"REQ_DEL_CHAR\0");
			break;
		case REQ_DEL_PREV:			/* delete char before cursor	*/
			strcpy(buffer,"REQ_DEL_PREV\0");
			break;
		case REQ_DEL_LINE:			/* delete line at cursor	*/
			strcpy(buffer,"REQ_DEL_LINE\0");
			break;
		case REQ_DEL_WORD:			/* delete word at cursor	*/
			strcpy(buffer,"REQ_DEL_WORD\0");
			break;
		case REQ_CLR_EOL:			/* clear to end of line		*/
			strcpy(buffer,"REQ_CLR_EOL\0");
			break;
		case REQ_CLR_EOF:			/* clear to end of field	*/
			strcpy(buffer,"REQ_CLR_EOF\0");
			break;
		case REQ_CLR_FIELD:			/* clear entire field		*/
			strcpy(buffer,"REQ_CLR_FIELD\0");
			break;
		case REQ_OVL_MODE:			/* begin overlay mode		*/
			strcpy(buffer,"REQ_OVL_MODE\0");
			break;
		case REQ_INS_MODE:			/* begin insert mode		*/
			strcpy(buffer,"REQ_INS_MODE\0");
			break;
		case REQ_SCR_FLINE:			/* scroll field forward a line	*/
			strcpy(buffer,"REQ_SCR_FLINE\0");
			break;
		case REQ_SCR_BLINE:			/* scroll field backward a line	*/
			strcpy(buffer,"REQ_SCR_BLINE\0");
			break;
		case REQ_SCR_FPAGE:			/* scroll field backward a page	*/
			strcpy(buffer,"REQ_SCR_FPAGE\0");
			break;
		case REQ_SCR_FHPAGE:		/* scroll field forward	 half page */
			strcpy(buffer,"REQ_SCR_FHPAGE\0");
			break;
		case REQ_SCR_BHPAGE:		/* scroll field backward half page */
			strcpy(buffer,"REQ_SCR_BHPAGE\0");
			break;
		case REQ_SCR_FCHAR:			/* horizontal scroll char	*/
			strcpy(buffer,"REQ_SCR_FCHAR\0");
			break;
		case REQ_SCR_BCHAR:			/* horizontal scroll char	*/
			strcpy(buffer,"REQ_SCR_BCHAR\0");
			break;
		case REQ_SCR_HFLINE:		/* horizontal scroll line	*/
			strcpy(buffer,"REQ_SCR_HFLINE\0");
			break;
		case REQ_SCR_HBLINE:		/* horizontal scroll line	*/
			strcpy(buffer,"REQ_SCR_HBLINE\0");
			break;
		case REQ_SCR_HFHALF:		/* horizontal scroll half line	*/
			strcpy(buffer,"REQ_SCR_HFHALF\0");
			break;
		case REQ_SCR_HBHALF:		/* horizontal scroll half line	*/
			strcpy(buffer,"REQ_SCR_HBHALF\0");
			break;

		case REQ_VALIDATION:		/* validate field		*/
			strcpy(buffer,"REQ_VALIDATION\0");
			break;
		case REQ_NEXT_CHOICE:		/* display next field choice	*/
			strcpy(buffer,"REQ_NEXT_CHOICE\0");
			break;
		case REQ_PREV_CHOICE:		/* display prev field choice	*/
			strcpy(buffer,"REQ_PREV_CHOICE\0");
			break;
		default:
			print_this = 0;
			break;
	}
	if(print_this)
	{
//		clrtoeol();
		MvAddStr(line,col,"              ");
		MvAddStr(line,col,buffer);
		refresh();
	}
}
/***************************************************************************/
/* replaces wgetnstr(), since we want to be able to edit values */
void wGetstring(WINDOW *win, char *buffer, int limit)
{
	int y0, x0, x, ch;
	bool done = FALSE;

	echo();
	getyx(win, y0, x0);
	(void) wattrset(win, A_REVERSE);

	x = (int) strlen(buffer);
	while (!done)
	{
		if (x > (int) strlen(buffer))
			x = (int) strlen(buffer);
		wmove(win, y0, x0);
		wprintw(win, "%-*s", limit, buffer);
		wmove(win, y0, x0 + x);
		switch (ch = wGetchar(win))
		{
			case '\n':
			case KEY_ENTER:
				done = TRUE;
				break;
			case CTRL('U'):
				*buffer = '\0';
				break;
			case '\b':
			case KEY_BACKSPACE:
			case KEY_DC:
				if (x > 0)
				{
					int j;
					for (j = --x; (buffer[j] = buffer[j + 1]) != '\0'; ++j)
					{
						;
					}
				}
				else
				{
					beep();
				}
				break;
			case KEY_LEFT:
				if (x > 0)
				{
					--x;
				}
				else
				{
					flash();
				}
				break;
			case KEY_RIGHT:
				++x;
				break;
			default:
				if (!isprint(ch) || ch >= KEY_MIN)
				{
					beep();
				}
				else if ((int) strlen(buffer) < limit)
				{
					int j;
					for (j = (int) strlen(buffer) + 1; j > x; --j)
					{
						buffer[j] = buffer[j - 1];
					}
					buffer[x++] = (char) ch;
				}
				else
				{
					flash();
				}
		}
	}

	wattroff(win, A_REVERSE);
	wmove(win, y0, x0);
	noecho();
}

