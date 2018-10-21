#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
//#include "../queue/rt_llist_threads_rw.h"
#include "../ioports.h"
#include "config_file.h"

#ifdef MAKE_TARGET
#warning "MAKE_TARGET defined"
#else
#include "test.priv.h"
#warning "MAKE_TARGET not defined"
#endif

static char open_br = '<';
static char close_br = '>';
static char open_br_slash[2] = "</";
static char nl = 0x0A;
static char tabx = 0x09;
static char first_line[] = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";
static char table[6] = "Table\0";
static char tick = '\'';
static char space = 0x21;

/////////////////////////////////////////////////////////////////////////////
// CONFIG_FILE is the define used for compiling the list_db and init_db programs
// so (i/ol)(Load/Write)Config is used by sched/tasks etc
#ifndef CONFIG_FILE
int ilLoadConfig(char *filename, illist_t *ill, size_t size, char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	int j;
	fptr = (char *)filename;
	UCHAR id;
	I_DATA i_data;
	UCHAR temp[150];
	int rc;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#else
		printf("%s  %s\n",errmsg,filename);
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
	if(id != 0x55)
	{
		strcpy(errmsg,"invalid file format - id is not 0x55\0");
		close(fp);
		return -1;
	}

//	printf("sizeof I_DATA: %lu\r\n",sizeof(I_DATA));
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
//		rc = read(fp,&i_data,sizeof(I_DATA));
		rc = read(fp,&temp[0],sizeof(I_DATA));

		memcpy((void *)&i_data,(void *)&temp[0],sizeof(I_DATA));
//		for(j = 30;j < 50;j++)
//			printf("%02x ",temp[j]);

//		printf("\r\n");	
/*
		printf("%2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %s\r\n",rc, i_data.port,
			i_data.affected_output[0],
			i_data.affected_output[1],
			i_data.affected_output[2],
			i_data.affected_output[3],
			i_data.affected_output[4],
			i_data.affected_output[5],
			i_data.affected_output[6],
			i_data.affected_output[7],
			i_data.affected_output[8],
			i_data.affected_output[9],
			i_data.label);
*/
		illist_insert_data(i, ill, &i_data);
	}

//	i = read(fp,(void*)curr_i_array,size);
//	mvprintw(LINES - 4,0,"%s\n",strerror(errno));
//	display_legend(6,5,strerror(errno)," ");
//	printf("fp:%d  read: %d bytes in iLoadConfig\n",fp,i);
//	attron(COLOR_PAIR(6));
//	mvprintw(LINES - 3, 0,"fp:%d  read: %d bytes\n",fp,i);
//	attroff(COLOR_PAIR(6));
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int olLoadConfig(char *filename, ollist_t *oll, size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;
	O_DATA o_data;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
	if(id != 0xAA)
	{
		strcpy(errmsg,"invalid file format - id is not 0x55\0");
		close(fp);
		return -1;
	}
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		read(fp,&o_data,sizeof(O_DATA));
		ollist_insert_data(i, oll, &o_data);
	}
//	printf("fp:%d  read: %d bytes in oLoadConfig\n",fp,i);
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
#if 0
int rlLoadConfig(char *filename, rt_llist_t *oll, size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;
	RI_DATA o_data;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
	if(id != 0xAA)
	{
		strcpy(errmsg,"invalid file format - id is not 0x55\0");
		close(fp);
		return -1;
	}
/*
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		read(fp,&o_data,sizeof(RI_DATA));
		ollist_insert_data(i, oll, &o_data);
	}
*/
//	printf("fp:%d  read: %d bytes in oLoadConfig\n",fp,i);
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
#endif
/////////////////////////////////////////////////////////////////////////////
int ilWriteConfig(char *filename, illist_t *ill, size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	I_DATA io;
	I_DATA *pio = &io;
	UCHAR id = 0x55;

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}
	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);
	write(fp,&id,1);
	for(i = 0;i < size/sizeof(I_DATA);i++)
	{
		illist_find_data(i,&pio,ill);
		j += write(fp,(const void*)pio,sizeof(I_DATA));
	}

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int olWriteConfig(char *filename,  ollist_t *oll, size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	O_DATA io;
	O_DATA *pio = &io;
	UCHAR id = 0xAA;

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);
	write(fp,&id,1);
	for(i = 0;i < size/sizeof(O_DATA);i++)
	{
		ollist_find_data(i,&pio,oll);
		j += write(fp,(const void*)pio,sizeof(O_DATA));

	}

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

#if 0
/////////////////////////////////////////////////////////////////////////////
int rlWriteConfig(char *filename,  rt_llist_t *oll, size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	RI_DATA io;
	RI_DATA *pio = &io;
	UCHAR id = 0xAA;

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);
	write(fp,&id,1);
	for(i = 0;i < size/sizeof(RI_DATA);i++)
	{
		ollist_find_data(i,&pio,oll);
		j += write(fp,(const void*)pio,sizeof(RI_DATA));
	}

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
#endif
#endif
/////////////////////////////////////////////////////////////////////////////
int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
	if(id != 0x55)
	{
		strcpy(errmsg,"invalid file format - id is not 0x55\0");
		close(fp);
		return -1;
	}
	size = 1760;
	i = read(fp,(void*)curr_i_array,size);
//	i = read(fp,(void*)curr_i_array,size);
//	mvprintw(LINES - 4,0,"%s\n",strerror(errno));
//	display_legend(6,5,strerror(errno)," ");
//	printf("fp:%d  read: %d bytes in iLoadConfig\n",fp,i);
//	attron(COLOR_PAIR(6));
//	mvprintw(LINES - 3, 0,"fp:%d  read: %d bytes\n",fp,i);
//	attroff(COLOR_PAIR(6));
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
	if(id != 0xAA)
	{
		strcpy(errmsg,"invalid file format - id is not 0x55\0");
		close(fp);
		return -1;
	}
	i = read(fp,(void*)curr_o_array,size);
//	printf("fp:%d  read: %d bytes in oLoadConfig\n",fp,i);
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
#if 0
int rtLoadConfig(char *filename, RI_DATA *curr_o_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
	if(id != 0xAA)
	{
		strcpy(errmsg,"invalid file format - id is not 0x55\0");
		close(fp);
		return -1;
	}
	i = read(fp,(void*)curr_o_array,size);
//	printf("fp:%d  read: %d bytes in oLoadConfig\n",fp,i);
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
#endif
///////////////////// Write/LoadConfig functions used by init/list_db start here (see make_db) ///////////////////////

/////////////////////////////////////////////////////////////////////////////
int iWriteConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	I_DATA io;
	I_DATA *pio = &io;
	I_DATA *curr_i_array2 = curr_i_array;
	UCHAR id = 0x55;
//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}
	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);
	write(fp,&id,1);
	for(i = 0;i < size/sizeof(I_DATA);i++)
	{
		pio = curr_i_array2;
		j += write(fp,(const void*)pio,sizeof(I_DATA));
		curr_i_array2++;
	}

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	O_DATA io;
	O_DATA *pio = &io;
	O_DATA *curr_o_array2 = curr_o_array;
	UCHAR id = 0xAA;

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);
	write(fp,&id,1);
	for(i = 0;i < size/sizeof(O_DATA);i++)
	{
//		memset(pio,0,sizeof(IO_DATA));
		pio = curr_o_array2;
		j += write(fp,(const void*)pio,sizeof(O_DATA));
		curr_o_array2++;
	}

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int iWriteConfigXML(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	I_DATA io;
	I_DATA *pio = &io;
	I_DATA *curr_i_array2 = curr_i_array;
	char labels[5][20] = {"I_DATA","label","port","affected_output","temp"};
	char temp[5];
	char tempx[30];

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);

	write(fp,(const void*)&first_line[0],strlen(first_line));
	write(fp,(const void*)&nl,1);
	write(fp,(const void*)&open_br,1);
	write(fp,(const void*)&table[0],strlen(table));
	write(fp,(const void*)&close_br,1);
	write(fp,(const void*)&nl,1);

	for(i = 0;i < size/sizeof(O_DATA);i++)
	{
		pio = curr_i_array2;

		write(fp,(const void*)&tabx,1);
		write(fp,(const void*)&open_br,1);
		write(fp,(const void*)&labels[0],strlen(labels[0]));
		write(fp,(const void*)&close_br,1);
		write(fp,(const void*)&nl,1);

		k = 0;
		for(j = 1;j < 13;j++)
		{
			write(fp,(const void*)&tabx,1);
			write(fp,(const void*)&tabx,1);
			write(fp,(const void*)&open_br,1);

			if(j == 1)
			{
				write(fp,(const void*)&labels[1],strlen(labels[1]));
				write(fp,(const void*)&close_br,1);
				sprintf(tempx,pio->label,strlen(pio->label));
				write(fp,(const void*)&tempx[0],strlen(tempx));
				write(fp,(const void*)&open_br_slash,2);
				write(fp,(const void*)&labels[1],strlen(labels[1]));
				write(fp,(const void*)&close_br,1);
			}else if(j == 2)
			{
				write(fp,(const void*)&labels[2],strlen(labels[2]));
				write(fp,(const void*)&close_br,1);
				sprintf(temp,"%d",pio->port);
				write(fp,(const void*)&temp[0],strlen(temp));
				write(fp,(const void*)&open_br_slash,2);
				write(fp,(const void*)&labels[2],strlen(labels[2]));
				write(fp,(const void*)&close_br,1);
			}else
			{
				write(fp,(const void*)&labels[3],strlen(labels[3]));
				sprintf(temp,"%d",k);
				write(fp,(const void*)&tick,1);
				write(fp,(const void*)&temp[0],strlen(temp));
				write(fp,(const void*)&tick,1);
				write(fp,(const void*)&close_br,1);

				sprintf(temp,"%d",pio->affected_output[k]);
				write(fp,(const void*)&temp[0],strlen(temp));

				write(fp,(const void*)&open_br_slash,2);
				write(fp,(const void*)&labels[3],strlen(labels[3]));
				sprintf(temp,"%d",k++);
				write(fp,(const void*)&tick,1);
				write(fp,(const void*)&temp[0],strlen(temp));
				write(fp,(const void*)&tick,1);
				write(fp,(const void*)&close_br,1);
			}
			write(fp,(const void*)&nl,1);
		}
		write(fp,(const void*)&tabx,1);
		write(fp,(const void*)&open_br_slash,2);
		write(fp,(const void*)&labels[0],strlen(labels[0]));
		write(fp,(const void*)&close_br,1);
		write(fp,(const void*)&nl,1);
		curr_i_array2++;
	}
	write(fp,(const void*)&open_br_slash,2);
	write(fp,(const void*)&table[0],strlen(table));
	write(fp,(const void*)&close_br,1);
	write(fp,(const void*)&nl,1);

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int oWriteConfigXML(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	O_DATA io;
	O_DATA *pio = &io;
	O_DATA *curr_o_array2 = curr_o_array;
	char labels[10][20] = {"O_DATA","label","port","onoff",
			"polarity","type","time_delay","time_left","pulse_time","reset"};
	char temp[5];
	char tempx[30];

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);

	write(fp,(const void*)&first_line[0],strlen(first_line));
	write(fp,(const void*)&nl,1);
	write(fp,(const void*)&open_br,1);
	write(fp,(const void*)&table[0],strlen(table));
	write(fp,(const void*)&close_br,1);
	write(fp,(const void*)&nl,1);

	for(i = 0;i < size/sizeof(O_DATA);i++)
	{
//		memset(pio,0,sizeof(IO_DATA));
		pio = curr_o_array2;

		write(fp,(const void*)&tabx,1);
		write(fp,(const void*)&open_br,1);
		write(fp,(const void*)&labels[0],strlen(labels[0]));
		write(fp,(const void*)&close_br,1);
		write(fp,(const void*)&nl,1);

		for(j = 1;j < 10;j++)
		{
			write(fp,(const void*)&tabx,1);
			write(fp,(const void*)&tabx,1);
			write(fp,(const void*)&open_br,1);
			write(fp,(const void*)&labels[j],strlen(labels[j]));
			write(fp,(const void*)&close_br,1);

			switch(j)
			{
				case 1:
				sprintf(tempx,pio->label,strlen(pio->label));
				write(fp,(const void*)&tempx[0],strlen(tempx));
				break;
				case 2:
				sprintf(temp,"%d",pio->port);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 3:
				sprintf(temp,"%d",pio->onoff);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 4:
				sprintf(temp,"%d",pio->polarity);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 5:
				sprintf(temp,"%d",pio->type);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 6:
				sprintf(temp,"%d",pio->time_delay);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 7:
				sprintf(temp,"%d",pio->time_left);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 8:
				sprintf(temp,"%d",pio->pulse_time);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
				case 9:
				sprintf(temp,"%d",pio->reset);
				write(fp,(const void*)&temp[0],strlen(temp));
				break;
			}

			write(fp,(const void*)&open_br_slash,2);
			write(fp,(const void*)&labels[j],strlen(labels[j]));
			write(fp,(const void*)&close_br,1);
			write(fp,(const void*)&nl,1);
		}
		write(fp,(const void*)&tabx,1);
		write(fp,(const void*)&open_br_slash,2);
		write(fp,(const void*)&labels[0],strlen(labels[0]));
		write(fp,(const void*)&close_br,1);
		write(fp,(const void*)&nl,1);
		curr_o_array2++;
	}
//	write(fp,(const void*)&nl,1);
	write(fp,(const void*)&open_br_slash,2);
	write(fp,(const void*)&table[0],strlen(table));
	write(fp,(const void*)&close_br,1);
	write(fp,(const void*)&nl,1);

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
#if 0
/////////////////////////////////////////////////////////////////////////////
int rtWriteConfig(char *filename, RI_DATA *curr_o_array,size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;
	RI_DATA io;
	RI_DATA *pio = &io;
	RI_DATA *curr_o_array2 = curr_o_array;
	UCHAR id = 0xAA;

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
	i = lseek(fp,0,SEEK_SET);
	write(fp,&id,1);
	for(i = 0;i < size/sizeof(RI_DATA);i++)
	{
//		memset(pio,0,sizeof(IO_DATA));
		pio = curr_o_array2;
		j += write(fp,(const void*)pio,sizeof(RI_DATA));
		curr_o_array2++;
	}

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
#endif
/////////////////////////////////////////////////////////////////////////////
int GetFileFormat(char *filename)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;
	char errmsg[40];
	fp = open((const char *)fptr, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//	fp = open((const char *)fptr, O_RDONLY);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	read(fp,&id,1);
//	printf("%x\n",id);
	if(id == 0x55)		// this means it's an input file
		return 0;
	else if(id == 0xAA)	// output file
		return 1;
	else return -1;
	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int getFileCreationTime(char *path,char *str)
{
// MM:DD-HH:MM:SS
    struct stat attr;
    stat(path, &attr);
    strncpy(str,ctime(&attr.st_mtime),24);
    *(str + 19) = 0;
    strcpy(str,str+4);
	return 0;
}
#if 0
/////////////////////////////////////////////////////////////////////////////
int LoadOdometer(char *filename, int *odo, char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;

	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
#ifdef MAKE_TARGET
		printf("%s  %s\n",errmsg,filename);
#else
#ifndef MAKE_SIM
		mvprintw(LINES-2,20,"%s  %s   ",errmsg,filename);
		refresh();
#endif
#endif
		return -2;
	}

	i = lseek(fp,0,SEEK_SET);
	i = 0;
	i = read(fp,(void*)odo,sizeof(int));

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int WriteOdometer(char *filename, int *odo, char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i,j,k;
	fptr = (char *)filename;

//#ifdef NOTARGET
	fp = open((const char *)fptr, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//	fp = open((const char *)fptr, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//	fp = open((const char *)fptr, O_RDWR | O_TRUNC);
//#else
//	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
//#endif
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);

		printf("%s  %s\n",errmsg,filename);
		return -2;
	}

	j = 0;
//	printf("fp = %d\n",fp);
//	printf("seek=%lu\n",lseek(fp,0,SEEK_SET));
//	i = lseek(fp,0,SEEK_SET);
	write(fp,(const void*)odo,sizeof(int));

	close(fp);
	strcpy(errmsg,"Success\0");
	return 0;
}
#endif
