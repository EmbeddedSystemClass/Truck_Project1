#ifndef FORM_SUP_H
#define FORM_SUP_H

#if 0
#ifdef TRACE
#warning "TRACE defined"
#else
#warning "TRACE not defined"
#endif
#endif

#if 0
#ifdef HAVE_RIPOFFLINE
#warning "HAVE_RIPOFFLINE defined"
#else
#warning "HAVE_RIPOFFLINE not defined"
#endif
#endif

#define STARTY 5
#define STARTX 0

#define FORM_WIN_STARTY 13
#define FORM_WIN_STARTX 3

#define NUM_FIELDSX 1
#define NUM_FIELDSY 4

#define NUM_FIELDSY2 NUM_FIELDSY-3
#define STARTY2 STARTY-3

#if defined(NCURSES_VERSION_PATCH) && (NCURSES_VERSION_PATCH < 20060218)
#warning "NUCURSES patch defined"
#endif

static const struct
{
	int code;
	int result;
}
lookup[] =
{
	{ CTRL('A'),    REQ_NEXT_CHOICE },
	{ CTRL('B'),    REQ_PREV_WORD },
	{ CTRL('C'),    REQ_CLR_EOL },
	{ CTRL('D'),    REQ_DOWN_FIELD },
	{ CTRL('E'),    REQ_END_FIELD },
	{ CTRL('F'),    REQ_NEXT_PAGE },
	{ CTRL('G'),    REQ_DEL_WORD },
	{ CTRL('H'),    REQ_DEL_PREV },
	{ CTRL('I'),    REQ_INS_CHAR },
	{ CTRL('K'),    REQ_CLR_EOF },
	{ CTRL('L'),    REQ_LEFT_FIELD },
	{ CTRL('M'),    REQ_NEW_LINE },
	{ CTRL('N'),    REQ_NEXT_FIELD },
	{ CTRL('O'),    REQ_INS_LINE },
	{ CTRL('P'),    REQ_PREV_FIELD },
	{ CTRL('R'),    REQ_RIGHT_FIELD },
	{ CTRL('S'),    REQ_BEG_FIELD },
	{ CTRL('U'),    REQ_UP_FIELD },
	{ CTRL('V'),    REQ_DEL_CHAR },
	{ CTRL('W'),    REQ_NEXT_WORD },
	{ CTRL('X'),    REQ_CLR_FIELD },
	{ CTRL('Y'),    REQ_DEL_LINE },
	{ CTRL('Z'),    REQ_PREV_CHOICE },
	{ ESCAPE,   	MAX_FORM_COMMAND + 1 },
	{ KEY_BACKSPACE, REQ_DEL_PREV },
	{ KEY_DOWN, 	REQ_DOWN_CHAR },
	{ KEY_END,  	REQ_LAST_FIELD },
	{ KEY_HOME, 	REQ_FIRST_FIELD },
	{ KEY_LEFT, 	REQ_LEFT_CHAR },
	{ KEY_LL,   	REQ_LAST_FIELD },
	{ KEY_NEXT, 	REQ_NEXT_FIELD },
	{ KEY_NPAGE,    REQ_NEXT_PAGE },
	{ KEY_PPAGE,    REQ_PREV_PAGE },
	{ KEY_PREVIOUS, REQ_PREV_FIELD },
	{ KEY_RIGHT,    REQ_RIGHT_CHAR },
	{ KEY_UP,   	REQ_UP_CHAR },
	{ QUIT,     	MAX_FORM_COMMAND + 1 }
};


typedef struct
{
	chtype background;
	int row_count;
	int *row_lengths;
} FieldAttrs;

extern FieldAttrs *field_attrs(FIELD * f);
extern int buffer_length(FIELD * f);
extern void set_buffer_length(FIELD * f, int length);
extern void init_edit_field(FIELD * f, char *value, int bufs);
extern int wGetchar(WINDOW *win);
extern void wGetstring(WINDOW *win, char *buffer, int limit);
extern FIELD *make_label(int frow, int fcol, NCURSES_CONST char *label);
extern FIELD *make_field(int frow, int fcol, int rows, int cols, int nbufs);
extern void display_form(FORM * f);
extern void erase_form(FORM * f);
extern int edit_secure(FIELD * me, int c);
extern int form_virtualize(FORM * f, WINDOW *w);
extern int my_form_driver(FORM * form, int c);
extern void print_req_type(WINDOW *win, int c, int line, int col);

#endif
