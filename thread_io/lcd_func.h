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
#define DIODDR	(0x14 / sizeof(UINT))
#define DIOADR	(0x04 / sizeof(UINT))
#define PORTFB  (0x30 / sizeof(UINT))
#define PORTFD	(0x34 / sizeof(UINT))
#define PORTLED	(0x20 / sizeof(UINT))

//The RED and Green LEDs can be controlled at physical address location 0x8084_0020.
//Bit 1 is the RED LED and bit 0 is the Green LED. A Logic “1” turns the LED on.

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

#define dio_set_bit8(A) setbiobit(portfb,1,A)
#define dio_set_ddr8(A) setbiobit(portfd,1,A)

volatile UINT *gpio;
volatile UINT *phdr;
volatile UINT *phddr;
volatile UINT *padr;
volatile UINT *paddr;
volatile UINT *dio_addr;
volatile UINT *dio_ddr;
volatile UINT *portfb;
volatile UINT *portfd;
volatile UINT *portled;

static void lcd_cmd(UINT);
static void lcd_write(UCHAR *);
static UINT lcd_wait(void);
static void lcdinit(void);
static void lcd_cursor(int row, int col, int page);
static void add_col(void);
static int setbiobit(UCHAR *ptr,int n,int v);
static UCHAR dio_get_ddr(void);
int getdioline(int n);
int setdioline(int n,int v);
static int getdioddr(int n);
static int setdioddr(int n,int v);
static void inc_bufptrs(void);

static void display_current(int special_case);
void scroll_up(void);
void scroll_down(void);
int myprintf1(char *str);
int myprintf2(char *str, int i);
int myprintf3(char *str, int i, int j);
void shift_left(void);
void shift_right(void);
void lcd_home(void);
void lcd_cls(void);
void red_led(int onoff);
void green_led(int onoff);
int lcd_enabled;

#endif
#endif

