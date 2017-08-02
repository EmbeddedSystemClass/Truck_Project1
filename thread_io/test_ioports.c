#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
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

//#define			PORTBASEADD	0x11E00240 // this give an assertion at ports = mmap...
#define			PORTBASEADD	0x11E00000
#define			DIOADD 0x80840004
#define			DDDRADD 0x80840004

// io map starting at base address
// 7/9/15 - using PC/104 card IR104 set at base address 0x300

#define			ROC_1	0x300	// relay output control registers
#define			ROC_2	0x301
#define			ROC_3	0x302	// bank 3 only uses first 4 bits

#define			DIR_1	0x304	// digital input reading registers
#define			DIR_2	0x305
#define			DIR_3	0x306	// bank 3 only uses first 4 bits

// 7/19/15 - using 2nd PC/104 card at address 280

#define			ROC_4	0x280	// relay output control registers
#define			ROC_5	0x281
#define			ROC_6	0x282	// bank 3 only uses first 4 bits

#define			DIR_4	0x284	// digital input reading registers
#define			DIR_5	0x285
#define			DIR_6	0x286	// bank 3 only uses first 4 bits

#define			UCHAR unsigned char
#define			UINT unsigned int
#define			VUCHAR volatile unsigned char
#define			DELAYTIME 50	// delay time in ms

VUCHAR *ports;
UCHAR outportstatus[16];
UCHAR current_io_settings;

void display(void);
void Menu(void);
void TestRead(UINT);
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
	struct timespec sleepTime;
	struct timespec rettime;
	sleepTime.tv_sec = 0;
	sleepTime.tv_nsec = 948540;
	for(j = 0;j < i;j++)
	{
		nanosleep(&sleepTime, &rettime);
	}
}

/**********************************************************************************************************/

int main(int argc, char **argv)
{
	int key;
	int key2;
	size_t pagesize;
	int i, j;
	UCHAR mask;

	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	assert(fd != -1);

	setvbuf(stdout, NULL, _IONBF, 0);

//	pagesize = getpagesize();
	pagesize = 522;
	ports = (VUCHAR *)mmap(0, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PORTBASEADD);
	assert(ports != MAP_FAILED);

	key2 = lseek(fd, pagesize - 1,SEEK_SET);
	if(key2 == -1)
	{
		close(fd);
		perror("error calling lseek\n");
		exit(1);
	}
	key2 = write(fd,"",1);
	if(key2 != 1)
	{
		close(fd);
		perror("error writing to last byte of file\n");
		exit(1);
	}

	TestRead(0);
	Menu();

	do
	{
		key = getc(stdin);
		printf("\n");
		if (key == 'A' || key == 'a')
		{
			*(ports + ROC_1) = 0xAA;
			*(ports + ROC_2) = 0xAA;
			*(ports + ROC_3) = 0xAA;
		}
		if (key == 'B' || key == 'b')
		{
			*(ports + ROC_1) = 0x55;
			*(ports + ROC_2) = 0x55;
			*(ports + ROC_3) = 0x55;
		}
		if (key == 'C' || key == 'c')
		{
			*(ports + ROC_1) = 0x00;
			*(ports + ROC_2) = 0x00;
			*(ports + ROC_3) = 0x00;
		}
		if (key == 'D' || key == 'd')
		{
			*(ports + ROC_1) = 0xFF;
			*(ports + ROC_2) = 0xFF;
			*(ports + ROC_3) = 0xFF;
		}
		if (key == 'E' || key == 'e')
		{
			mask = 1;
			printf("\nreading DIR_1\n");
//			for(j = 0;j < 8;j++)
			do
			{
//				printf("\nmask: %x\n",mask);
				*(ports + ROC_1) = mask;
				mask = mask << 1;
				mydelay(DELAYTIME);
				TestRead(0);
			}while(mask > 0);
			mask = 1;
			printf("\nreading DIR_2\n");
			for(j = 0;j < 8;j++)
			{
//				printf("\nmask: %x\n",mask);
				*(ports + ROC_2) = mask;
				mask = mask << 1;
				mydelay(DELAYTIME);
				TestRead(1);
			}
			mask = 1;
			printf("\nreading DIR_3\n");
			for(j = 0;j < 4;j++)
			{
//				printf("\nmask: %x\n",mask);
				*(ports + ROC_3) = mask;
				mask = mask << 1;
				mydelay(DELAYTIME);
				TestRead(2);
			}
			printf("\ndone\n");
		}

		if(key == 'F' || key == 'f')
		{
			printf("\n");
			for(j = 3;j < 7;j++)
				printf("%02x ",*(ports + DIR_1 + (UCHAR)j));
			printf("\n");
		}
		if(key == 'G' || key == 'g')
			TestRead(0);
		if(key == 'H' || key == 'h')
			TestRead(1);
		if(key == 'I' || key == 'i')
			TestRead(2);
		if(key == 'M' || key == 'm')
			Menu();

	}while(key != 'q' && key != 'Q' );

	if(munmap((void *)ports,pagesize) == -1)
		perror("error un-mapping file\n");

	close(fd);

 	return 0;
}

/**********************************************************************************************************/
void TestRead(UINT which)
{
	int i;
	UINT dir;
	if(which == 0)
		printf("\nPORTA: %02x\n",*(ports + DIR_1 + 4));
	else if(which == 1)
		printf("\nPORTB: %02x\n",*(ports + DIR_2 + 4));
	else if(which == 2)
		printf("\nPORTC: %02x\n",*(ports + DIR_3 + 4));
	else
		printf("bad param\n");
}

/**********************************************************************************************************/

void display()
{
	int i;

	for (i = 512; i < 522; i++)
	{
		printf("%x ",*(ports + i));
	}
}

/**********************************************************************************************************/

void Menu()
{
	printf("a - turn on all odd ports\n");
	printf("b - turn off all odd ports\n");
	printf("c - turn all ports off\n");
	printf("d - turn all ports on\n");
	printf("e - rolling output on all ports\n");
	printf("f - test read of all port\n");
	printf("g - read PORTA\n");
	printf("h - read PORTB\n");
	printf("I - read PORTC\n");
	printf("m - this menu\n");
	printf("q - quit\n");
}
