#ifndef __IOPORTS_H
#define __IOPORTS_H
//#warning "IOPORTS defined"
#define			VUCHAR volatile unsigned char
#define			PORTBASEADD72			0x11E00000
#define			PORTBASEADD78			0xE8000000
//#define			IOCARDBASEADD78			0xEC000000	// 8-bit memory
#define			IOCARDBASEADD78			0xEE000000	// 8-bit io				(this one works)
//#define			IOCARDBASEADD78			0xED000000	// 16-bit memory
//#define			IOCARDBASEADD7I			0xEF000000	// 16-bit io
// (see section 6.2.5 in manual)

//#define NUM_PORTS (OUTPORTF_OFFSET-OUTPORTA_OFFSET)+1
#define NUM_PORTS 6
#define NUM_PORT_BITS (NUM_PORTS*8)-8	// PORTC & F only has 6 bits each
#define			OUTPORTA_OFFSET		0
#define			OUTPORTB_OFFSET		1
#define			OUTPORTC_OFFSET		2
#define			OUTPORTD_OFFSET		3
#define			OUTPORTE_OFFSET		4
#define			OUTPORTF_OFFSET		5
#ifdef MAKE_TARGET
VUCHAR *card_ports;
#else
UCHAR *card_ports;
UCHAR fake_port[NUM_PORTS];
#endif
UCHAR outportstatus[NUM_PORTS];
UCHAR current_io_settings;
size_t pagesize;
int fd;

/*
VUCHAR *card_ports	- IOCARDBASEADD72	0xE[C/E/D/F]000000 
VUCHAR *card_mem 	- PORTBASEADD78		0xE8000000
*/

/*
 io map starting at base address
 7/9/15 - using PC/104 card IR104 set at base address 0x300
*/
/* relay output control registers */
#define			ROC_1	0x300
#define			ROC_2	0x301
/* bank 3 only uses first 4 bits */
#define			ROC_3	0x302

/* digital input reading registers */
#define			DIR_1	0x304
#define			DIR_2	0x305
/* bank 3 only uses first 4 bits */
#define			DIR_3	0x306

/*
 7/19/15 - using 2nd PC/104 card at address 280
*/

/*  relay output control registers */
#define			ROC_4	0x280
#define			ROC_5	0x281
/* bank 3 only uses first 4 bits */
#define			ROC_6	0x282

/* digital input reading registers */
#define			DIR_4	0x284
#define			DIR_5	0x285
/* bank 3 only uses first 4 bits */
#define			DIR_6	0x286

#define			DELAYTIME 50

#define			COMMQ_BAD_CH		160u

/*
 IO port defines

(from manual: https://wiki.embeddedarm.com/wiki/TS-7800#DIO_Header)

Access to the DIO header is described here. These pins can be driven low, otherwise they are inputs
with pull-up resistors. The pull-ups are via 2.2k Ohms on the odd numbered pins 1-15. Pin 10 has a
4.7k pull-up and is a read-only input. The rest are pulled up by the CPLD through 20k-150k nominal
resistance. The pinout for the DIO header is shown below.

There is no built-in SPI bus on the TS-7800, but SPI is easily implemented in software as
demonstrated in our sample code.

Although the distinction is transparent to the user, the most of the pins on the DIO and LCD headers are driven by CPLD, which means that they are 5V tolerant. However logic levels of 0-3.3V are still recommended. Applying 5V to a pin that is connected to the CPLD before it has been initialized may damage the CPLD. The only pins that come directly from the FPGA are output pins for the SPI bus. The SPI_MISO pin goes through a separate buffer chip to get to the FPGA so it is also 5V tolerant.

The DIO and LCD headers are controlled by 32 bit registers at 0xe8000004 and 0xe8000008. Bits 15-0 control the DIO header and bits 29-16 control the LCD header. The register at 0xe8000008 is the output register. Writing a 0 drives the pin low but writing a 1 only tri-states. To use these pins for input, write a 1 to the output register and read the register at 0xe8000004. See the #LCD Header and #DIO Header sections for more details.

TS-7800 pinout:

facing top of card with DIO1 at bottom:

	2	4	6	8	10	12	14	16
	1	3	5	7	9	11	13	15
  (dot)

1) DIO_01
2) GND
3) DIO_03
4) DIO_04
5) DIO_05
6) SPI_FRAME
7) DIO_07
8) DIO_08
9) DIO_09
10) SPI_MISO
11) DIO_11
12) SPI_MOSI
13) DIO_13
14) SPI_CLK
15) DIO_15
16) 3v3

LCD:

1) +5v
2) GND
3) RS
4) BIAS
5) END
6) RW
7) DB1
8) DB0
9) DB3
10) DB2
11) DB5
12) DB4
13) DB7
14) DB6	

PC104 on TS-7800: (from 6.2.5 in manual)

To access peripherals on the PC104 bus it is necessary to add the base address from the table below to the offset of the peripheral to get a memory address for accessing the peripheral. For example, for ISA 8-bit I/O address 0x100, add 0xEE000000 to 0x100 to get 0xEE000100.

		Memory		I/O
8-bit 	0xEC000000 	0xEE000000
16-bit 	0xED000000 	0xEF000000

IRQs 5, 6, and 7 on the PC104 bus from the TS-7800 are 64 + the PC104 IRQ number. These will be IRQs 69, 70, and 71. 


the following is for the TS-7200

DIO lines DIO_0 thru DIO_7 = 0x8084_0004.
The DDR for this port is at address location 0x8084_0014.
DIO_8 is accessed via bit 1 of Port F in the EP9302. The Port F data register is at address
location 0x8084_0030. The DDR address for this port is location 0x8084_0034.
The Pin 4 of the DIO1 Header, in the default configuration, is accessed via bit 0 of Port C
in the EP9302. The address location 0x8084_0008 is Port C Data Register and
0x8084_0018 is Port C Directon Register.
When accessing these registers, it is important not to change the other bit positions in
these Port F registers. Other DIO1 Port functionality, used for dedicated TS-7200
functions, utilize these same control registers. All accesses to these registers should use
read-modify-write cycles.
Warning
All pins on the DIO header use 0-3.3V logic levels. Do not drive these lines to 5V.
When the DIO pins are configured as outputs, they can “source” 4 mA or “sink” 8 mA and
have logic swings between GND and 3.3V. When configured as inputs, they have
standard TTL level thresholds and must not be driven below 0 Volts or above 3.3 Volts.
DIO lines DIO_0 thru DIO_3 have 4.7K Ohm “pull-up” resistors to 3.3V biasing these
signals to a logic”1”. The other DIO pins have 100K Ohm bias resistors biasing these
inputs to a logic “1”.
SPI Interface
The EP9302 Synchronous Serial Port is available on the DIO1 header. This port can
implement either a master or slave interface to peripheral devices that have either
Motorola SPI, or National Semiconductor Microwire serial interfaces.
The transmit and receive data paths are buffered with internal FIFO memories allowing up
to eight 16-bit values to be stored for both transmit and receive modes. The clock rate is
programmable up to 3.7 MHz and has programmable phase and polarity. The data frame
size is programmable from 4 to 16 bits.
By using some of the DIO1 Header pins as peripheral Chip Select signals, a complete
interface is available for addressing up to 9 SPI peripherals. The SPI bus pins are defined
in the table below:

TS-7200 pinout:

DIO1 pin	signal
1			DIO_08
2			GND
3			DIO_09
4			ADC0
5			DIO_10
6			ADC4
7			DIO_11
8			DIO_16
9			DIO_12
10			SPI_MISO
11			DIO_13
12			SPI_MOSI
13			DIO_14
14			SPI_CLK
15			DIO_15
16			+3.3v
*/

typedef struct mystruct
{
//	UCHAR		configfilename[20];
//	char		logfilename[20];
	UCHAR		outportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET];
/*
	int			logfile;
	int			save_on_exit;
	int			log_errors_only;
	int			logfile_update_rate;
	int			records_in_queue;
	int			using_spi;
	int			Port1;
	int			Port2;
	int			Port3;
	UINT		Port1Speed;
	UINT		Port2Speed;
	UINT		Port3Speed;
	UCHAR		blinker_state;
	UCHAR		blinker_count;
	int			general_timer_count;
	UCHAR		output_state[4];
	UCHAR		input_state[4];
	char		passcode[10];
*/
} MYSTRUCT;

MYSTRUCT ms;

void init_mem(void);
void close_mem(void);
void OutPortA(int onoff, UCHAR bit);
void OutPortB(int onoff, UCHAR bit);
void OutPortC(int onoff, UCHAR bit);
void OutPortD(int onoff, UCHAR bit);
void OutPortE(int onoff, UCHAR bit);
void OutPortF(int onoff, UCHAR bit);
UCHAR InPortByteA(void);
UCHAR InPortByteB(void);
UCHAR InPortByteC(void);
UCHAR InPortByteD(void);
UCHAR InPortByteE(void);
UCHAR InPortByteF(void);
void OutPortByteA(UCHAR byte);
void OutPortByteB(UCHAR byte);
void OutPortByteC(UCHAR byte);
void OutPortByteD(UCHAR byte);
void OutPortByteE(UCHAR byte);
void OutPortByteF(UCHAR byte);
void TurnOffAllOutputs(void);
void DIO1_out0_3(int onoff, int port);
void DIO1_out4_7(int onoff, int port);
void setDIO1_0_3(int io);
void setDIO1_4_7(int io);
int DIO1_in0_3(int port);
int DIO1_in4_7(int port);

// currently there are (2) IO cards each with 20 ports
// so each card has (3) byte-wide ports (A,B & C) but C only has
// 6 valid bits so we may have to assign bits as 0->19 and 24->43
#if 0
enum outputs
{
	COOLINGFAN,
	FUELPUMP,
	PRELUBE,
	STARTER,
	HEADLAMP,
	BRIGHTS,
	LEFTBLINKER,
	RIGHTBLINKER,
	RUNNINGLIGHTS,
	ALARMSPEAKER,
	ALARMLIGHT,
	BATTERYCHARGERELAY,
	DASHHEATER,
	BATTERYCOMPHEATER,
	CRANKCASEHEATER,
	TRAILERBRAKES,
	TRAILERLEFTBLINKER,
	TRAILERRIGHTBLINKER,
	INTRUDERALARM,
	DOMELIGHT,
	// there's a gap between 19 and 24 because the 3rd byte port (PORTC) on each card only has 6 bits
	UNKNOWN1,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	LIGHTBAR,
	BLINKINDICATE,
	ENGINERUN,
	ACCON,
	BRAKELIGHTS,
	BACKUPLIGHTS,
	TESTOUTPUT7,
	TESTOUTPUT8,
	TESTOUTPUT9,
	TESTOUTPUT10,
	TESTOUTPUT11,
	TESTOUTPUT12,
	TESTOUTPUT13,
	TESTOUTPUT14,
	TESTOUTPUT15,
	TESTOUTPUT16,
	TESTOUTPUT17,
	TESTOUTPUT18,
	TESTOUTPUT19,
	TESTOUTPUT20
} OUTPUTS;
// ...
// can only go up to 43 max
#define LAST_OUTPUT_PORT TESTOUTPUT20
#define FIRST_OUTPUT_PORT COOLINGFAN
#define	CURNOOUTPUTS (LAST_OUTPUT_PORT - FIRST_OUTPUT_PORT)
#endif
// input types (used by QMEMINPUT)
#if 0
// input port defines
// currently there are (2) IO cards each with 20 ports
// so each card has (3) byte-wide ports (A,B & C) but C only has
// 6 valid bits so we may have to assign bits as 0->19 and 24->43

enum inputs
{
	STARTSWITCH,
	SHUTDOWN,
	BRAKES,
	HEADLIGHTS,
	LEFTBLINKERI,
	RIGHTBLINKERI,
	RUNNINGLIGHTSI,
	LEFTDOOROPEN,
	RIGHTDOOROPEN,
	TESTINPUT9,
	TESTINPUT10,
	TESTINPUT11,
	TESTINPUT12,
	TESTINPUT13,
	TESTINPUT14,
	TESTINPUT15,
	TESTINPUT16,
	TESTINPUT17,
	TESTINPUT18,
	TESTINPUT19,
	// there's a gap here for the same reason as above (see output defines)
	UNKNOWN1A,
	UNKNOWN2A,
	UNKNOWN3A,
	UNKNOWN4A,
	TESTINPUT24,
	TESTINPUT25,
	TESTINPUT26,
	TESTINPUT27,
	TESTINPUT28,
	TESTINPUT29,
	TESTINPUT30,
	TESTINPUT31,
	TESTINPUT32,
	TESTINPUT33,
	TESTINPUT34,
	TESTINPUT35,
	TESTINPUT36,
	TESTINPUT37,
	TESTINPUT38,
	TESTINPUT39,
	TESTINPUT40,
	TESTINPUT41,
	TESTINPUT42,
	TESTINPUT43
} INPUTS;
//#define LAST_INPUT_PORT RUNNINGLIGHTSI
#define LAST_INPUT_PORT TESTINPUT42
#define FIRST_INPUT_PORT STARTSWITCH
#define	CURNOINPUTS	(LAST_INPUT_PORT - FIRST_INPUT_PORT)
#endif
#endif

