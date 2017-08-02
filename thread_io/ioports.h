#ifndef __IOPORTS_H
#define __IOPORTS_H

#if 0
#ifdef NOMAIN
#warning "NOMAIN defined"
#else
#warning "NOMAIN not defined"
#endif

#ifdef NOTARGET
#warning "NOTARGET defined"
#else
#warning "NOTARGET not defined"
#endif
#endif

#define			VUCHAR volatile unsigned char
#define			PORTBASEADD	0x11E00000
//#define NUM_PORTS (OUTPORTF_OFFSET-OUTPORTA_OFFSET)+1
#define NUM_PORTS 6
#define NUM_PORT_BITS (NUM_PORTS*8)-8	// PORTC & F only has 6 bits each
#define			OUTPORTA_OFFSET		0
#define			OUTPORTB_OFFSET		1
#define			OUTPORTC_OFFSET		2
#define			OUTPORTD_OFFSET		3
#define			OUTPORTE_OFFSET		4
#define			OUTPORTF_OFFSET		5
#ifndef NOTARGET
VUCHAR *ports;
#else
UCHAR *ports;
UCHAR fake_port[NUM_PORTS];
#endif
UCHAR outportstatus[NUM_PORTS];
UCHAR current_io_settings;
size_t pagesize;
int fd;

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

DIO1 pin	signal
1			DIO_0
2			GND
3			DIO_1
4			Port_C0
5			DIO_2
6			SPI_Frame
7			DIO_3
8			DIO_8
9			DIO_4
10			SPI_MISO
11			DIO_5
12			SPI_MOSI
13			DIO_6
14			SPI_CLK
15			DIO_7
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

