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
#include "ioports.h"
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

void init_mem(void)
{
	int key;
#ifdef MAKE_TARGET
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	assert(fd != -1);

	setvbuf(stdout, NULL, _IONBF, 0);

	pagesize = getpagesize();
	card_ports = (VUCHAR *)mmap(0, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PORTBASEADD);
	assert(card_ports != MAP_FAILED);

	key = lseek(fd, pagesize - 1,SEEK_SET);
	if(key == -1)
	{
		close(fd);
		perror("error calling lseek\n");
		exit(1);
	}
	key = write(fd,"",1);
	if(key != 1)
	{
		close(fd);
		perror("error writing to last byte of file\n");
		exit(1);
	}
#else
	card_ports = &fake_port[0];
#endif
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
void TurnOffAllOutputs(void)
{
	OutPortByteA(0);
	OutPortByteB(0);
	OutPortByteC(0);
	OutPortByteD(0);
	OutPortByteE(0);
	OutPortByteF(0);
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
void OutPortByteA(UCHAR byte)
{
//	pms->outportstatus[OUTPORTA_OFFSET] = byte;
	*(card_ports + ROC_1) = byte;
#ifdef MAKE_TARGET
	printf("port A: %x\n",byte);
#endif
}

/**********************************************************************************************************/
void OutPortByteB(UCHAR byte)
{
//	pms->outportstatus[OUTPORTB_OFFSET] = byte;
	*(card_ports + ROC_2) = byte;
#ifdef MAKE_TARGET
	printf("port B: %x\n",byte);
#endif
}

/**********************************************************************************************************/
void OutPortByteC(UCHAR byte)
{
//	pms->outportstatus[OUTPORTC_OFFSET] = byte;
	*(card_ports + ROC_3) = byte;
#ifdef MAKE_TARGET
	printf("port C: %x\n",byte);
#endif
}
/**********************************************************************************************************/
void OutPortByteD(UCHAR byte)
{
//	pms->outportstatus[OUTPORTA_OFFSET] = byte;
	*(card_ports + ROC_4) = byte;
#ifdef MAKE_TARGET
	printf("port D: %x\n",byte);
#endif
}

/**********************************************************************************************************/
void OutPortByteE(UCHAR byte)
{
//	pms->outportstatus[OUTPORTB_OFFSET] = byte;
	*(card_ports + ROC_5) = byte;
#ifdef MAKE_TARGET
	printf("port E: %x\n",byte);
#endif
}

/**********************************************************************************************************/
void OutPortByteF(UCHAR byte)
{
//	pms->outportstatus[OUTPORTC_OFFSET] = byte;
	*(card_ports + ROC_6) = byte;
#ifdef MAKE_TARGET
	printf("port F: %x\n",byte);
#endif
}
/***********************************************************************************************************/
UCHAR InPortByte(int bank)
{
	UCHAR state;
	if(bank > 2)
		state = *(card_ports + DIR_4 + bank-3);
	else	
		state = *(card_ports + DIR_1 + bank);
	return state;
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
#ifdef MAKE_TARGET
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
//		printf("can only set 4 bits in function DIO1_out0_3\n");
		return;
	}
//	ucPort = inportb(CTLDIO1);
	if(!(ucPort & 0x01))
		fail = 1;
//	ucPort = inportb(DIO1LOW);
	if(fail == 1)
	{
//		printf("can't set bits on DIO1 0->3 because its and input\n");
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

//	outportb(DIO1LOW,ucPort);
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
//		printf("can only set 4 bits in function DIO1_out4_7\n");
		return;
	}
//	ucPort = inportb(CTLDIO1);
	if(!(ucPort & 0x02))
		fail = 1;
//	ucPort = inportb(DIO1LOW);
//	printf("%x ",ucPort);
	if(fail == 1)
	{
//		printf("can't set bits on DIO1 4->7 because its and input\n");
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

//	outportb(DIO1LOW,ucPort);
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
//		printf("can only set 4 bits 8->11 in function DIO1_out8_11\n");
		return;
	}
//	ucPort = inportb(CTLDIO1);
	if(!(ucPort & 0x20))
		fail = 1;
//	ucPort = inportb(DIO1HIGH);
//	printf("%x ",ucPort);
	if(fail == 1)
	{
//		printf("can't set bits on DIO1 8->11 because its and input\n");
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

//	outportb(DIO1HIGH,ucPort);
//	printf("%x ",ucPort);
}

/**********************************************************************************************************/
/* io = 1 - output; io = 0 - input */
void setDIO1_0_3(int io)
{
	UCHAR ucPort;
//	ucPort = inportb(CTLDIO1);
	if(io == 1)
		ucPort |= 0X01;
	else if(io == 0)
		ucPort &= 0XFE;
//	outportb(CTLDIO1,ucPort);
}
/**********************************************************************************************************/
/* io = 1 - output; io = 0 - input */
void setDIO1_4_7(int io)
{
	UCHAR ucPort;
//	ucPort = inportb(CTLDIO1);
	if(io == 1)
		ucPort |= 0X02;
	else if(io == 0)
		ucPort &= 0XFD;
//	outportb(CTLDIO1,ucPort);
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
//		printf("can only read 4 bits 0->3 in function DIO1_in0_3\n");
		return COMMQ_BAD_CH;
	}
//	ucPort = inportb(CTLDIO1);
	ucPort &= 0x01;
	if(ucPort != 0)
		fail = 1;
//	ucPort = inportb(DIO1LOW);
	printf("%x ",ucPort);
	if(fail == 1)
	{
//		printf("can't read bits on DIO1 8->11 because its an output\n");
//		ucPort = inportb(CTLDIO1);
//		printf("CTLDIO1 = %x\n",ucPort);
		return COMMQ_BAD_CH;
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
//	ucPort = inportb(DIO1LOW);

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
//		printf("can only read 4 bits 4-> in function DIO1_in4_7\n");
		return COMMQ_BAD_CH;
	}
//	ucPort = inportb(CTLDIO1);
	ucPort &= 0x02;
	if(ucPort != 0)
		fail = 1;
//	ucPort = inportb(DIO1LOW);
	ucPort &= 0xf0;
	ucPort >>= 4;
//	printf("L = %x ",ucPort);
	if(fail == 1)
	{
//		printf("can't read bits on DIO1 4->7 because its and output\n");
//		ucPort = inportb(CTLDIO1);
//		printf("CTLDIO1 = %x\n",ucPort);
		return COMMQ_BAD_CH;
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
void close_mem(void)
{
	if(munmap((void *)card_ports,pagesize) == -1)
		perror("error un-mapping file\n");
	close(fd);
}

