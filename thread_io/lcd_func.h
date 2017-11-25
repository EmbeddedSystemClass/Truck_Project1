#ifndef __LCD_FUNC_H
#define __LCD_FUNC_H

#ifdef TS_7800
#warning "TS_7800 defined"
#else
#define			LCDBASEADD			0x80840000
#define PADR    0							// address offset of LCD
#define PADDR   (0x10 / sizeof(UINT))		// address offset of DDR LCD
#define PHDR    (0x40 / sizeof(UINT))		// bits 3-5: EN, RS, WR
#define PHDDR   (0x44 / sizeof(UINT))		// DDR for above

#define HOME 0x80
#define ROW2 0xA8
#define CLEAR 0x01
#define SHIFTLEFT 0x18
#define SHIFTRIGHT 0x1c

// These delay values are calibrated for the EP9301
// CPU running at 166 Mhz, but should work also at 200 Mhz
#define SETUP   15
#define PULSE   36
#define HOLD    22

#define COUNTDOWN(x)    asm volatile ( \
"1:\n"\
"subs %1, %1, #1;\n"\
"bne 1b;\n"\
: "=r" ((x)) : "r" ((x)) \
);

volatile UINT *gpio;
volatile UINT *phdr;
volatile UINT *phddr;
volatile UINT *padr;
volatile UINT *paddr;

static void lcd_cmd(UINT);
static void lcd_write(UCHAR *);
static UINT lcd_wait(void);
static void lcdinit(void);
static void lcd_cursor(int row, int col, int page);
static void add_col(void);
void myprintf1(char *str);
void myprintf2(char *str, int i);
void myprintf3(char *str, int i, int j);
void shift_left(void);
void shift_right(void);
void lcd_home(void);
void lcd_cls(void);
#endif
#endif

