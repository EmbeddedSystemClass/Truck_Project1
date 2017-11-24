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
#include "../ioports.h"
#include "config_file.h"

#ifdef MAKE_TARGET
#warning "MAKE_TARGET defined"
#else
#include "test.priv.h"
#warning "MAKE_TARGET not defined"
#endif

/////////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_FILE
int ilLoadConfig(char *filename, illist_t *ill, size_t size,char *errmsg)
{
	char *fptr;
	int fp = -1;
	int i = 0;
	fptr = (char *)filename;
	UCHAR id;
	I_DATA i_data;

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

	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		read(fp,&i_data,sizeof(I_DATA));
//		printf("%d %d %d %s\n",i_data.port,i_data.affected_output,i_data.temp,i_data.label);
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
	i = read(fp,(void*)curr_i_array,size);
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
/*********************************************************************/
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

