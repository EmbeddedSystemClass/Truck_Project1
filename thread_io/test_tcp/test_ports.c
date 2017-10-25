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

#define			CTLDIO1			0x7A		// control port for DIO_1
											// bit 0: dir for DIO1_0->3	(1=output; 0=input)
											// bit 1: dir for DIO1_4->7
											// bit 5: dir for DIO1_8->11 (12 & 13 are always inputs)
											// bit 7: enable int 7 on pin 14 (DIO_13)
// 5/8/16 using DIO1 bits 0->3 as inputs for keypad code and bit 4 as output for keypad ack
#define			DIO1LOW			0x7B		// DIO1_0->7
#define			DIO1HIGH		0x7C		// DIO1_8->13 (0 -> 5 lower 6 bits only)

#define			CTLDIO2			0x7D		// control port for DIO_2 and bits 2 & 3 control LCD
											// bit 0: dir for DIO2_0->3
											// bit 1: dir for DIO2_4->7
											// bit 2: dir for LCD 0->3
											// bit 3: dir for LCD 4->7
											// bit 4: enable LCD mode
											// bit 5: dir for DIO2_8->11
											// bit 6: enable LCD_RS to drive IRQ1
											// bit 7: enable DIO2_13 to drive IRQ6
#define			DIO2LOW			0x7E		// DIO2_0->7
#define			DIO2HIGH		0x7F		// DIO2_8->13 (lower 6 bits only)
#define			LCDLOWERINMASK	0X2B
#define			LCDUPPERINMASK	0X27
#define			LCDALLINMASK	0X23
#define			LCDALLOUTMASK	0X2F

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

UCHAR inportb(UCHAR temp)
{
	UCHAR test;
	test = 0;
	return test;
}

void outportb(UCHAR a, UCHAR b)
{
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
// turn on or off port 0->3 of DIO1
// this is assuming that all are set to outputs
void DIO1_out0_3(int onoff, int port)
{
	UCHAR mask;
	UCHAR ucPort;
	int fail;

	fail = 0;
	if(port > 3)
	{
		printf("can only set 4 bits in function DIO1_out0_3\n");
		return;
	}
	ucPort = inportb(CTLDIO1);
	if(!(ucPort & 0x01))
		fail = 1;
	ucPort = inportb(DIO1LOW);

	if(fail == 1)
	{
		printf("can't set bits on DIO1 0->3 because its and input\n");
		return;
	}
	switch(port)
	{
	case 0:
		mask = 1;
		break;
	case 1:
		mask = 2;
		break;
	case 2:
		mask = 4;
		break;
	case 3:
		mask = 8;
		break;
	default:
		mask = 0;
	}

	if(onoff)
		ucPort |= mask;
	else
		ucPort &= ~mask;

	outportb(DIO1LOW,ucPort);
}

/**********************************************************************************************************/
// turn on or off port 4->7 of DIO1
// this is assuming that all are set to outputs
void DIO1_out4_7(int onoff, int port)
{
	UCHAR mask;
	UCHAR ucPort;
	int fail;

	fail = 0;
	if(port > 3)
	{
		printf("can only set 4 bits in function DIO1_out4_7\n");
		return;
	}
	ucPort = inportb(CTLDIO1);
	if(!(ucPort & 0x02))
		fail = 1;
	ucPort = inportb(DIO1LOW);
//	printf("%x ",ucPort);
	if(fail == 1)
	{
		printf("can't set bits on DIO1 4->7 because its and input\n");
		return;
	}
	switch(port)
	{
	case 0:
		mask = 0x10;
		break;
	case 1:
		mask = 0x20;
		break;
	case 2:
		mask = 0x40;
		break;
	case 3:
		mask = 0x80;
		break;
	default:
		mask = 0;
	}

	if(onoff)
		ucPort |= mask;
	else
		ucPort &= ~mask;

	outportb(DIO1LOW,ucPort);
//	printf("%x ",ucPort);
}

/**********************************************************************************************************/
// turn on or off port 8->11 of DIO1
// this is assuming that all are set to outputs
void DIO1_out8_11(int onoff, int port)
{
	UCHAR mask;
	UCHAR ucPort;
	int fail;

	fail = 0;
	if(port > 3)
	{
		printf("can only set 4 bits 8->11 in function DIO1_out8_11\n");
		return;
	}
	ucPort = inportb(CTLDIO1);
	if(!(ucPort & 0x20))
		fail = 1;
	ucPort = inportb(DIO1HIGH);
//	printf("%x ",ucPort);
	if(fail == 1)
	{
		printf("can't set bits on DIO1 8->11 because its and input\n");
		return;
	}
	switch(port)
	{
	case 0:
		mask = 1;
		break;
	case 1:
		mask = 2;
		break;
	case 2:
		mask = 4;
		break;
	case 3:
		mask = 8;
		break;
	default:
		mask = 0;
	}

	if(onoff)
		ucPort |= mask;
	else
		ucPort &= ~mask;

	outportb(DIO1HIGH,ucPort);
//	printf("%x ",ucPort);
}

/**********************************************************************************************************/
/* io = 1 - output; io = 0 - input */
void setDIO1_0_3(int io)
{
	UCHAR ucPort;
	ucPort = inportb(CTLDIO1);
	if(io == 1)
		ucPort |= 0X01;
	else if(io == 0)
		ucPort &= 0XFE;
	outportb(CTLDIO1,ucPort);
}
/**********************************************************************************************************/
/* io = 1 - output; io = 0 - input */
void setDIO1_4_7(int io)
{
	UCHAR ucPort;
	ucPort = inportb(CTLDIO1);
	if(io == 1)
		ucPort |= 0X02;
	else if(io == 0)
		ucPort &= 0XFD;
	outportb(CTLDIO1,ucPort);
}
/**********************************************************************************************************/
/* io = 1 - output; io = 0 - input */
void setDIO1_8_11(int io)
{
	UCHAR ucPort;
	ucPort = inportb(CTLDIO1);
	if(io == 1)
		ucPort |= 0X20;
	else if(io == 0)
		ucPort &= 0XDF;
	outportb(CTLDIO1,ucPort);
}

/**********************************************************************************************************/
int DIO1_in0_3(int port)
{
	UCHAR mask;
	UCHAR ucPort;
	int fail;

	fail = 0;

	if(port > 3)
	{
		printf("can only read 4 bits 0->3 in function DIO1_in0_3\n");
		return -1;
	}
	ucPort = inportb(CTLDIO1);
	ucPort &= 0x01;
	if(ucPort != 0)
		fail = 1;
	ucPort = inportb(DIO1LOW);
	printf("%x ",ucPort);
	if(fail == 1)
	{
		printf("can't read bits on DIO1 8->11 because its an output\n");
		ucPort = inportb(CTLDIO1);
		printf("CTLDIO1 = %x\n",ucPort);
		return -1;
	}

	switch(port)
	{
	case 0:
		mask = 1;
		break;
	case 1:
		mask = 2;
		break;
	case 2:
		mask = 4;
		break;
	case 3:
		mask = 8;
		break;
	default:
		mask = 0;
	}
	ucPort = inportb(DIO1LOW);

	ucPort &= mask;
	if(ucPort)
		return 1;
	else
		return 0;
}

/**********************************************************************************************************/
int DIO1_in4_7(int port)
{

	UCHAR mask;
	UCHAR ucPort;
	int fail;

	fail = 0;

	if(port > 3)
	{
		printf("can only read 4 bits 4-> in function DIO1_in4_7\n");
		return -1;
	}
	ucPort = inportb(CTLDIO1);
	ucPort &= 0x02;
	if(ucPort != 0)
		fail = 1;
	ucPort = inportb(DIO1LOW);
	ucPort &= 0xf0;
	ucPort >>= 4;
//	printf("L = %x ",ucPort);
	if(fail == 1)
	{
		printf("can't read bits on DIO1 4->7 because its and output\n");
		ucPort = inportb(CTLDIO1);
		printf("CTLDIO1 = %x\n",ucPort);
		return -1;
	}

	switch(port)
	{
	case 0:
		mask = 0x01;
		break;
	case 1:
		mask = 0x02;
		break;
	case 2:
		mask = 0x04;
		break;
	case 3:
		mask = 0x08;
		break;
	default:
		mask = 0;
	}

	ucPort &= mask;
//	printf("%x %x ",mask,ucPort);
	if(ucPort == 0)
		return 0;
	else
		return 1;
}
/**********************************************************************************************************/
int DIO1_in8_11(int port)
{
	UCHAR mask;
	UCHAR ucPort;
	int fail;

	fail = 0;

	if(port > 3)
	{
		printf("can only read 4 bits 8->11 in function DIO1_in8_11\n");
		return -1;
	}
	ucPort = inportb(CTLDIO1);
	ucPort &= 0x20;
	if(ucPort != 0)
		fail = 1;
	ucPort = inportb(DIO1HIGH);
//	printf("H = %x ",ucPort);
	if(fail == 1)
	{
		printf("can't read bits on DIO1 8->11 because its an output\n");
		ucPort = inportb(CTLDIO1);
		printf("CTLDIO1 = %x\n",ucPort);
		return -1;
	}

	switch(port)
	{
	case 0:
		mask = 0x01;
		break;
	case 1:
		mask = 0x02;
		break;
	case 2:
		mask = 0x04;
		break;
	case 3:
		mask = 0x08;
		break;
	default:
		mask = 0;
	}

	ucPort &= mask;
//	printf("%x ",ucPort);
	if(ucPort == 0)
		return 0;
	else
		return 1;
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


