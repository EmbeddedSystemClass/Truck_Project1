#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
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
#ifdef TS-7800
#define			PORTBASEADD			0xEE000000
#else
#define			PORTBASEADD			0x11E00000
#endif
//#define			IOCARDBASEADD78			0xEC000000	// 8-bit memory
//#define			IOCARDBASEADD78			0xEE000000	// 8-bit io				(this one works)
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
void ToggleOutPortA(int port);
void OutPortA(int onoff, UCHAR bit);
void OutPortB(int onoff, UCHAR bit);
void OutPortC(int onoff, UCHAR bit);
void OutPortD(int onoff, UCHAR bit);
void OutPortE(int onoff, UCHAR bit);
void OutPortF(int onoff, UCHAR bit);
void do_scr(int key);
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
	char filename[20];
	char scriptfilename[20];
	int fp, fp2;
	char *fptr;
	char errmsg[30];
	char script[200];
	int scr_ptr;

	if(argc > 1)
	{
		strcpy(scriptfilename,argv[1]);

		fptr = scriptfilename;
		fp2 = open((const char *)fptr, O_RDONLY);
		if(fp2 < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp2);
			printf("%s  %s\n",errmsg,filename);
			fp2 = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 666);
			printf("created new file: %s\n",fptr);
			exit(1);
		}
		lseek(fp2,0,SEEK_SET);
		j = lseek(fp2,0,SEEK_END);
		printf("\nscript file length: %d\n",j);
#if 0
			scr_ptr = malloc(j);
			if(scr_ptr == NULL)
			{
				printf("problem with malloc\n");
				exit(1);
			}
#endif
		lseek(fp2,0,SEEK_SET);
		if(fp2 < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp2);
			printf("%s  %s\n",errmsg,filename);
			exit(1);
		}
		read(fp2,&script[0],j);
		close(fp2);
		for(i = 0;i < j;i++)
			printf("%c ",script[i]);
	}


	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	assert(fd != -1);

	setvbuf(stdout, NULL, _IONBF, 0);

	card_pagesize = getpagesize();
	printf("card_pagesize = %4lx\n",card_pagesize);
	card_ports = (VUCHAR *)mmap(0, card_pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PORTBASEADD);

	assert(card_ports != MAP_FAILED);

	key2 = lseek(fd, card_pagesize - 1,SEEK_SET);
	if(key2 == -1)
	{
		close(fd);
		perror("error calling lseek on card\n");
		exit(1);
	}

	fptr = filename;
	strcpy(filename,"iostate.bin\0");
	fp = open((const char *)fptr, O_RDONLY);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
		printf("%s  %s\n",errmsg,filename);
		fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 666);
		if(fp < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp);
			printf("%s  %s\n",errmsg,filename);
			exit(1);
		}
	}
	read(fp,&outportstatus,NUM_PORTS);
	close(fp);
	Menu();

	// the memory for the DIO actually starts at E8000003 but the card_mem
	// must be mmap'd at an even address
//	card_mem += 4;
	current_portA = 0;

	do
	{
		key = getc(stdin);
		do_scr(key);
		printf("\n");

	}while(key != 'q' && key != 'Q' );

	if(munmap((void *)card_ports,card_pagesize) == -1)
		perror("error un-mapping card_ports file\n");

	// set the pointer back to the even address
//	if(munmap((void *)card_mem,mem_pagesize) == -1)
//		perror("error un-mapping card_mem file\n");

	close(fd);

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
		printf("%s  %s\n",errmsg,filename);
		fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 666);
		if(fp < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp);
			printf("%s  %s\n",errmsg,filename);
			exit(1);
		}
	}
	write(fp,&outportstatus,NUM_PORTS);
	close(fp);

	printf("exiting program\n");

 	return 0;
}

void do_scr(int key)
{
	int i;
	switch(key)
	{
		case 'a':
			printf("all on\n");
			OutPortA(1,0);
			mydelay(100);
			OutPortA(1,1);
			mydelay(100);
			OutPortA(1,2);
			mydelay(100);
			OutPortA(1,3);
		break;
		case 'b':
			printf("all off\n");
			OutPortA(0,0);
			mydelay(100);
			OutPortA(0,1);
			mydelay(100);
			OutPortA(0,2);
			mydelay(100);
			OutPortA(0,3);
		break;
		case 'c':
			printf("toggle 4 bulb overhead\n");
			ToggleOutPortA(0);
		break;
		case 'd':
			printf("toggle 2 bulb overhead\n");
			ToggleOutPortA(1);
		break;
		case 'e':
			printf("toggle near workbench\n");
			ToggleOutPortA(2);
		break;
		case 'f':
			printf("toggle far workbench\n");
			ToggleOutPortA(3);
		break;
		case 'g':
			printf("toggle all\n");
			ToggleOutPortA(0);
			mydelay(100);
			ToggleOutPortA(1);
			mydelay(100);
			ToggleOutPortA(2);
			mydelay(100);
			ToggleOutPortA(3);
			mydelay(100);
		break;
		case 'h':
			printf("short delay\n");
			mydelay(200);
		break;
		case 'i':
			printf("regular delay\n");
			mydelay(2000);
		break;
		case 'j':
			printf("long delay\n");
			mydelay(4000);
		break;
		case 'k':
			printf("very long delay\n");
			mydelay(10000);
		break;
		case 'l':
			printf("very very long delay\n");
			mydelay(100000);
		break;
		case 'n':
			for(i = 0;i < 20;i++)
			{
				OutPortA(1,0);
				mydelay(3000);
				OutPortA(0,0);

				mydelay(200);

				OutPortA(1,1);
				mydelay(3000);
				OutPortA(0,1);

				mydelay(200);

				OutPortA(1,2);
				mydelay(3000);
				OutPortA(0,2);

				mydelay(200);

				OutPortA(1,3);
				mydelay(3000);
				OutPortA(0,3);
				mydelay(200);
			}
			OutPortA(1,1);
		break;
		case 'o':
		break;

		case 'm':
			Menu();
		break;

		default:
		break;
	}
	printf("done\n");
}


/**********************************************************************************************************/
void Menu()
{
	printf("a - all on\n");
	printf("b - all off\n");
	printf("c - toggle 4 bulb overhead\n");
	printf("d - toggle 2 bulb overhead\n");
	printf("e - toggle near workbench\n");
	printf("f - toggle far workbench\n");
	printf("g - toggle all\n");
	printf("h - short time delay\n");
	printf("i - regular time delay\n");
	printf("j - long time delay\n");
	printf("k - very long time delay\n");
	printf("l - very very long time delay\n");
	printf("m - Menu\n");
	printf("n - special function\n");
	printf("q - quit\n");
}

/**********************************************************************************************************/
void ToggleOutPortA(int port)
{
	UCHAR mask;
	UCHAR *pstate;
	UCHAR state = 0;
	UCHAR temp;

	mask = 1;
	port = (port<7?port:7);
	mask <<= port;
	temp = outportstatus[OUTPORTA_OFFSET] & mask;
	temp = ~temp;
	temp &= (mask | outportstatus[OUTPORTA_OFFSET]);

	outportstatus[OUTPORTA_OFFSET] = temp;
	state = temp;
	pstate = &state;
//	printf("outportstatus: %x\n",outportstatus[OUTPORTA_OFFSET]);
	pstate = &state;
	*(card_ports + ROC_1) = *pstate;
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

