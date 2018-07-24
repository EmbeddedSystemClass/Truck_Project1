#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
#include "../../mytypes.h"
//#include "ioports.h"
/*
Physical Address Region 	Emulates x86 cycle

11E0_0000 thru 11E0_03FF 	8-bit I/O cycles
21E0_0000 thru 21E0_03FE 	16-bit I/O cycles
11A0_0000 thru 11AF_FFFF 	8-bit Memory cycles
21A0_0000 thru 21AF_FFFE 	16-bit Memory cycles

I/O cycles on the PC/104 expansion bus strobe either IOR# or IOW#,
 while memory cycles strobe the MEMR# or MEMW# signals. For example,
a TS-SER1 peripheral board can be jumper-selected as COM3, which would
correspond to a PC I/O base address of 0x3E8. Since this is an 8-bit
peripheral, this COM port must be accessed at the physical base
address of 0x11E0_03E8. */

/*

The PC-104 connector can be multiplexed between different functionalities
including ISA bus and GPIO. The power-up default is GPIO
mode, with all I/Os in a neutral state. To enable the PC-104 bus (ISA) signals,
it is necessary to write the following values to the registers specified:

0x55555555 to address 0xE8000030
0x55555555 to address 0xE8000034
0x55555 to address 0xE8000038
0x55555 to address 0xE800003C
*/

//#define 		PORTBASEADD1 0xE8000000

//#define			PORTBASEADD	0x11E00240 // this give an assertion at ports = mmap...
//#define			PORTBASEADD	0x11E00000	// for the TS-7200
//#define			DIOADD 0xE8000000
//#define			DIOADD 0x80840004
//#define			DDDRADD 0x80840004

// io map starting at base address
// 7/9/15 - using PC/104 card IR104 set at base address 0x300

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


#define			ROC_1	0x280	// relay output control registers
#define			ROC_2	0x281
#define			ROC_3	0x282	// bank 3 only uses first 4 bits

#define			DIR_1	0x284	// digital input reading registers
#define			DIR_2	0x285
#define			DIR_3	0x286	// bank 3 only uses first 4 bits

#define			ROC_4	0x300	// relay output control registers
#define			ROC_5	0x301
#define			ROC_6	0x302	// bank 3 only uses first 4 bits

#define			DIR_4	0x304	// digital input reading registers
#define			DIR_5	0x305
#define			DIR_6	0x306	// bank 3 only uses first 4 bits

#define			UCHAR unsigned char
#define			UINT unsigned int
#define			VUCHAR volatile unsigned char
#define			DELAYTIME 50	// delay time in ms

VUCHAR *card_ports;
VUCHAR *card_mem;
//UCHAR *ports;
UCHAR outportstatus[NUM_PORTS];
void display(void);
void Menu(void);
void TestRead(UINT);
void OutPortA(int onoff, UCHAR bit);
void OutPortB(int onoff, UCHAR bit);
void OutPortC(int onoff, UCHAR bit);
void OutPortD(int onoff, UCHAR bit);
void OutPortE(int onoff, UCHAR bit);
void OutPortF(int onoff, UCHAR bit);

static UCHAR current_portA;

/**********************************************************************************************************/
struct timeval tv;


static double curtime(void)
{
	gettimeofday (&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// param i is miliseconds to delay
/**********************************************************************************************************/

static void mydelay(unsigned long i)
{
	unsigned long j;

	do
	{
		for(j = 0;j < 20000;j++);
			i--;
	}while(i > 0);

}

/**********************************************************************************************************/

int main(int argc, char **argv)
{
	int key;
	int key2;
	size_t mem_pagesize;
	size_t card_pagesize;
	int i, j;
	UCHAR _mask;

	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	assert(fd != -1);

	setvbuf(stdout, NULL, _IONBF, 0);

	card_pagesize = getpagesize();
	printf("card_pagesize = %4x\n",card_pagesize);
	card_ports = (VUCHAR *)mmap(0, card_pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, IOCARDBASEADD78);

	assert(card_ports != MAP_FAILED);

	key2 = lseek(fd, card_pagesize - 1,SEEK_SET);
	if(key2 == -1)
	{
		close(fd);
		perror("error calling lseek on card\n");
		exit(1);
	}
#if 0
	key2 = write(fd,"",1);
	if(key2 != 1)
	{
		close(fd);
		perror("error writing to last byte of file\n");
		exit(1);
	}
#endif

	mem_pagesize = getpagesize();
	printf("mem_pagesize = %4x\n",mem_pagesize);
	card_mem = (VUCHAR *)mmap(0, mem_pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PORTBASEADD78);
	assert(card_mem != MAP_FAILED);

	key2 = lseek(fd, mem_pagesize - 1,SEEK_SET);
	if(key2 == -1)
	{
		close(fd);
		perror("error calling lseek on mem\n");
		exit(1);
	}

//	TestRead(0);
	Menu();

	// the memory for the DIO actually starts at E8000003 but the card_mem
	// must be mmap'd at an even address
	card_ports += 4;
	card_mem += 4;
	current_portA = 0;
	do
	{
		key = getc(stdin);
		printf("\n");
		switch(key)
		{

/*
0x55555555 to address 0xE8000030
0x55555555 to address 0xE8000034
0x55555 to address 0xE8000038
0x55555 to address 0xE800003C
*/
			case 'a':
				for(i = 0x30-4;i < 0x3C-4;i++)
					printf("%X : %2x\n",(UINT)(card_mem + i),*(card_mem + i));
			break;
			case 'b':
				printf("card ports: %X : %2x\n",(UINT)card_ports,*card_ports);
				for(i = 0;i < NUM_PORTS;i++)
					printf("%2x\n",outportstatus[i]);
/*
				for(i = 0;i < 13;i++)
				*(card_mem + i) = 0;
				for(i = 0;i < 14;i++)
					printf("%X : %2x\n",(UINT)(card_mem + i),*(card_mem + i));
*/
			break;
			case 'c':
				for(i = 0x30-4;i < 0x3C-4;i++)
				*(card_mem + i) = 0x55;

/*
				for(i = 8;i < 10;i++)
					*(card_mem + i) = 0x55;

				*(card_mem + i + 1) = 0x55;

				for(i = 0x0C;i < 0x0E;i++)
					*(card_mem + i) = 0x55;

				*(card_mem + i + 1) = 0x55;
*/
				for(i = 0x30-4;i < 0x3C-4;i++)
					printf("%X : %2x\n",(UINT)(card_mem + i),*(card_mem + i));
			break;
			case 'd':
				for(i = 0;i < 500;i++)
				{
					*(card_ports) = (UCHAR)(i<<2);
					mydelay(300);
				}
			break;
			case 'e':
			for(i = 0;i < 500;i++)
				{
					*(card_ports) = (UCHAR)(i<<4);
					mydelay(300);
				}
			break;
			case 'f':
				for(i = 0;i < 500;i++)
				{
					*(card_ports) = (UCHAR)(i<<6);
					mydelay(300);
				}
			break;
			case 'g':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortA(1,i);
				}
			break;
			case 'h':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortA(0,i);
				}
			break;
			case 'i':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortB(1,i);
				}
			break;
			case 'j':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortB(0,i);
				}
			break;
			case 'k':
				for(i = 0;i < 4;i++)
				{
					mydelay(300);
					OutPortC(1,i);
				}
			break;
			case 'l':
				for(i = 0;i < 4;i++)
				{
					mydelay(300);
					OutPortC(0,i);
				}
			break;
			case 'n':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortD(1,i);
				}
			break;
			case 'o':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortD(0,i);
				}
			break;
			case 'p':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortE(1,i);
				}
			break;
			case 'r':
				for(i = 0;i < 8;i++)
				{
					mydelay(300);
					OutPortE(0,i);
				}
			break;
			case 's':
				for(i = 0;i < 4;i++)
				{
					mydelay(300);
					OutPortF(1,i);
				}
			break;
			case 't':
				for(i = 0;i < 4;i++)
				{
					mydelay(300);
					OutPortF(0,i);
				}
			break;

			case 'u':
				OutPortA(0,1);
			break;
			case 'v':
				OutPortA(1,1);
			break;
			case 'w':
				OutPortA(0,2);
			break;
			case 'x':
				OutPortA(1,2);
			break;
			case 'y':
				OutPortA(0,3);
			break;
			case 'z':
				OutPortA(1,3);
			break;
#if 0
				if((outportstatus[OUTPORTA_OFFSET] & 0x01) == 1)
					OutPortA(0,1);
				else
					OutPortA(1,1);
				outportstatus[OUTPORTA_OFFSET] |= (outportstatus[OUTPORTA_OFFSET] & 0x01);
			break;
			case 'v':
				if((outportstatus[OUTPORTA_OFFSET] & 0x01) == 1)
					OutPortA(0,1);
				else
					OutPortA(1,1);
			break;
			case 'w':
				if((outportstatus[OUTPORTA_OFFSET] & 0x01) == 1)
					OutPortA(0,1);
				else
					OutPortA(1,1);
			break;
#endif
			case 'm':
				Menu();
			break;
			default:
			break;
		}

	}while(key != 'q' && key != 'Q' );

	card_ports -= 4;
	card_mem -= 4;

	if(munmap((void *)card_ports,card_pagesize) == -1)
		perror("error un-mapping card_ports file\n");

	// set the pointer back to the even address
	if(munmap((void *)card_mem,mem_pagesize) == -1)
		perror("error un-mapping card_mem file\n");

	close(fd);
	printf("exiting program\n");

 	return 0;
}

/**********************************************************************************************************/
void Menu()
{
	printf("a - show memory starting at 0xE8000030\n");
	printf("b - card_ports\n");
	printf("c - set 0xE8000030->3C to 0x55\n");
	printf("d - DIO of bit 3 & 4\n");
	printf("e - DIO of bit 5 & 6\n");
	printf("f - DIO of bit 7 & 8\n");
	printf("\n");
	printf("g - PORTA on\n");
	printf("h - PORTA off\n");
	printf("i - PORTB on\n");
	printf("j - PORTB off\n");
	printf("k - PORTC on\n");
	printf("l - PORTC off\n");
	printf("m - this menu\n");
	printf("n - PORTD on\n");
	printf("o - PORTD off\n");
	printf("p - PORTE on\n");
	printf("q - quit\n");
	printf("r - PORTE off\n");
	printf("s - PORTF on\n");
	printf("t - PORTF off\n");
	printf("\n");
	printf("u - PORTA0 off\n");
	printf("v - PORTA0 on\n");
	printf("w - PORTA1 off\n");
	printf("x - PORTA1 on\n");
	printf("y - PORTA2 off\n");
	printf("z - PORTA2 on\n");
}

/**********************************************************************************************************/
void OutPortA(int onoff, UCHAR bit)
{
	UCHAR mask;
	UCHAR *pstate;
// TODO: state needs to be set to the current state of the output register
	UCHAR state = 0;

	state = outportstatus[OUTPORTA_OFFSET];
	mask = 1;
	mask <<= bit;
	if(onoff)
		state |= mask;
	else
		state &= ~mask;
	pstate = &state;
	outportstatus[OUTPORTA_OFFSET] = state;
	*(card_ports + ROC_1) = *pstate;
//	printf("%2x ",state);
}
/**********************************************************************************************************/
void OutPortB(int onoff, UCHAR bit)
{
	UCHAR mask;
	UCHAR *pstate;
	UCHAR state = 0;

	state = outportstatus[OUTPORTB_OFFSET];
	mask = 1;
	mask <<= bit;
	if(onoff)
		state |= mask;
	else
		state &= ~mask;
	pstate = &state;
	outportstatus[OUTPORTB_OFFSET] = state;
	*(card_ports + ROC_2) = *pstate;
//	printf("%2x ",state);
}
/**********************************************************************************************************/
void OutPortC(int onoff, UCHAR bit)
{
	UCHAR mask;
	UCHAR *pstate;
	UCHAR state = 0;

	state = outportstatus[OUTPORTC_OFFSET];
	mask = 1;
	mask <<= bit;
	if(onoff)
		state |= mask;
	else
		state &= ~mask;
	pstate = &state;
	outportstatus[OUTPORTC_OFFSET] = state;
	*(card_ports + ROC_3) = *pstate;
//	printf("%2x ",state);
}
/**********************************************************************************************************/
void OutPortD(int onoff, UCHAR bit)
{
	UCHAR mask;
	UCHAR *pstate;
	UCHAR state = 0;

	state = outportstatus[OUTPORTD_OFFSET];
	mask = 1;
	mask <<= bit;
	if(onoff)
		state |= mask;
	else
		state &= ~mask;
	pstate = &state;
	outportstatus[OUTPORTD_OFFSET] = state;
	*(card_ports + ROC_4) = *pstate;
//	printf("%2x ",state);
}
/**********************************************************************************************************/
void OutPortE(int onoff, UCHAR bit)
{
	UCHAR mask;
	UCHAR *pstate;
	UCHAR state = 0;

	state = outportstatus[OUTPORTE_OFFSET];
	mask = 1;
	mask <<= bit;
	if(onoff)
		state |= mask;
	else
		state &= ~mask;
	pstate = &state;
	outportstatus[OUTPORTE_OFFSET] = state;
	*(card_ports + ROC_5) = *pstate;
//	printf("%2x ",state);
}
/**********************************************************************************************************/
void OutPortF(int onoff, UCHAR bit)
{
	UCHAR mask;
	UCHAR *pstate;
	UCHAR state = 0;

	state = outportstatus[OUTPORTF_OFFSET];
	mask = 1;
	mask <<= bit;
	if(onoff)
		state |= mask;
	else
		state &= ~mask;
	pstate = &state;
	outportstatus[OUTPORTF_OFFSET] = state;
	*(card_ports + ROC_6) = *pstate;
//	printf("%2x ",state);
}

/**********************************************************************************************************/
void TestRead(UINT which)
{
	int i;
	UINT dir;
	if(which == 0)
		printf("\nPORTA: %02x\n",*(card_ports + DIR_1 + 4));
	else if(which == 1)
		printf("\nPORTB: %02x\n",*(card_ports + DIR_2 + 4));
	else if(which == 2)
		printf("\nPORTC: %02x\n",*(card_ports + DIR_3 + 4));
	else
		printf("bad param\n");
}

/**********************************************************************************************************/

void display()
{
	int i;

	for (i = 512; i < 522; i++)
	{
		printf("%x ",*(card_ports + i));
	}
}

/**********************************************************************************************************/

