#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
#include "mytypes.h"
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

#ifdef TS-7800
#define			PORTBASEADD			0xEE000000
#else
#define			PORTBASEADD			0x11E00000
#endif

//#define NUM_PORTS (OUTPORTF_OFFSET-OUTPORTA_OFFSET)+1
#define NUM_PORTS 6
#define NUM_PORT_BITS (NUM_PORTS*8)-8	// PORTC & F only has 6 bits each
#define			OUTPORTA_OFFSET		0
#define			OUTPORTB_OFFSET		1
#define			OUTPORTC_OFFSET		2
#define			OUTPORTD_OFFSET		3
#define			OUTPORTE_OFFSET		4
#define			OUTPORTF_OFFSET		5


// card_port can start from IOCARDBASEADD72 - just add 4 for the outputs
// the inputs can start from "     "

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

#if 0
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
#endif
#define			UCHAR unsigned char
#define			UINT unsigned int
#define			VUCHAR volatile unsigned char
#define			DELAYTIME 50	// delay time in ms

VUCHAR *card_ports;
//VUCHAR *card_mem;
//UCHAR *ports;
UCHAR inportstatus[NUM_PORTS];
UCHAR outportstatus[NUM_PORTS];
void display(void);
void Menu(void);
void TestRead(UINT);

void OutPortA(int onoff, UCHAR bit);
UCHAR InPortByteA(void);
UCHAR InPortByteB(void);
UCHAR InPortByteC(void);
UCHAR InPortByteD(void);
UCHAR InPortByteE(void);
UCHAR InPortByteF(void);

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
	UCHAR temp;
	UCHAR mask;
	int done;
	UCHAR port,bit;

	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	assert(fd != -1);

	setvbuf(stdout, NULL, _IONBF, 0);

	card_pagesize = getpagesize();
	printf("card_pagesize = %4x\n",card_pagesize);
	card_ports = (VUCHAR *)mmap(0, card_pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PORTBASEADD);

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

#if 0
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
#endif
//	TestRead(0);
//	Menu();

	// the memory for the DIO actually starts at E8000003 but the card_mem
	// must be mmap'd at an even address

	inportstatus[0] = InPortByteA();
	inportstatus[1] = InPortByteB();
	inportstatus[2] = InPortByteC();
	inportstatus[3] = InPortByteD();
	inportstatus[4] = InPortByteE();
	inportstatus[5] = InPortByteF();

	for(i = 0;i < 6;i++)
		printf("%2x ",inportstatus[i]);

/*
	printf("\n1st card:\n");

	for(i = 0x280;i < 0x291;i++)
		printf("%2x %2x\n",card_ports + i, *(card_ports + i));
*/
	printf("\ntest_inports\n");
	printf("\n2nd card:\n");

	for(i = 0x300;i < 0x306;i++)
		printf("%2x %2x :",card_ports + i, *(card_ports + i));

	for(j = 0;j < 10;j++)
	for(i = 0;i < 16;i++)
	{
/*
		temp = InPortByteA();
		if(temp != inportstatus[0])
		{
			printf("inputA: %2x \n",temp);
			inportstatus[0] = temp;
		}
		temp = InPortByteB();
		if(temp != inportstatus[1])
		{
			printf("inputB: %2x \n",temp);
			inportstatus[1] = temp;
		}
		temp = InPortByteC();
		if(temp != inportstatus[2])
		{
			printf("inputC: %2x \n",temp);
			inportstatus[2] = temp;
		}
*/
		temp = InPortByteD();
		if(temp != inportstatus[3])
		{
			inportstatus[3] = temp;
			printf("inputD: %2x\n",~temp);
		}
/*
		temp = InPortByteE();
		if(temp != inportstatus[4])
		{
			printf("inputE: %2x \n",temp);
			inportstatus[4] = temp;
		}
		temp = InPortByteF();
		if(temp != inportstatus[5])
		{
			printf("inputF: %2x \n",temp);
			inportstatus[5] = temp;
		}
*/
#if 0
		port = i / 2;
		bit = i & 1;
//		OutPortA(bit,port);
		OutPortA(bit,0);
//		printf("%d %d\n",bit,port);
#endif

		mydelay(100);
//		for(i = 0x280;i < 0x292;i++)
//			printf("%2x %2x\n",card_ports + i, *(card_ports + i));
	}

	for(i = 0x300;i < 0x306;i++)
		printf("%2x %2x :",card_ports + i, *(card_ports + i));


	printf("\n");


	if(munmap((void *)card_ports,card_pagesize) == -1)
		perror("error un-mapping card_ports file\n");

	close(fd);
	printf("exiting program\n");

 	return 0;
}

/**********************************************************************************************************/
void Menu()
{
	printf("a - PORTA inputs\n");
	printf("b - PORTB inputs\n");
	printf("c - PORTC inputs\n");
	printf("d - PORTD inputs\n");
	printf("e - PORTE inputs\n");
	printf("f - PORTF inputs\n");
	printf("m - this menu\n");
	printf("q - quit\n");
}

/***********************************************************************************************************/
UCHAR InPortByteA(void)
{
	UCHAR state;
	state = *(card_ports + DIR_1);
	return state;
}
/***********************************************************************************************************/
UCHAR InPortByteB(void)
{
	UCHAR state;
	state = *(card_ports + DIR_2);
	return state;
}
/***********************************************************************************************************/
UCHAR InPortByteC(void)
{
	UCHAR state;
	state = *(card_ports + DIR_3);
	return state;
}
/***********************************************************************************************************/
UCHAR InPortByteD(void)
{
	UCHAR state;
	state = *(card_ports + DIR_4);
	return state;
}
/***********************************************************************************************************/
UCHAR InPortByteE(void)
{
	UCHAR state;
/*
#ifndef NOTARGET
	printf("ouch port e access\n");
	return state;
#endif
*/
	state = *(card_ports + DIR_5);
	return state;
}
/***********************************************************************************************************/
UCHAR InPortByteF(void)
{
	UCHAR state;
	state = *(card_ports + DIR_6);
	return state;
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

