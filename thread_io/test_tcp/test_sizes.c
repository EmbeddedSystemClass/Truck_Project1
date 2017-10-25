#include <stdio.h>

#define ILABELSIZE 20

typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned long ULONG;

typedef struct i_data
{
	char label[ILABELSIZE];
	UCHAR port;
	UCHAR affected_output;
	UINT temp;
} I_DATA;

typedef struct test
{
	char ch;
	int i;
	long li;
	UCHAR uch;
	char *pch;
	UINT *puch;
}TEST;

int main(void)
{
	TEST mytest;
	I_DATA itest;

	printf("sizeof():\n");
	printf("char: %ld\n",sizeof(char));
	printf("char *: %ld\n",sizeof(char*));
	printf("unsigned char: %ld\n",sizeof(UCHAR));

	printf("int: %ld\n",sizeof(int));
	printf("int *: %ld\n",sizeof(int*));
	printf("unsigned int: %ld\n",sizeof(UINT));

	printf("long: %ld\n",sizeof(long));
	printf("long *: %ld\n",sizeof(long*));
	printf("unsigned long: %ld\n",sizeof(ULONG));

	printf("struct: %ld\n",sizeof(mytest));
	printf("struct: %ld\n",sizeof(itest));
	printf("struct: %ld\n",sizeof(I_DATA));

	return 0;
}
