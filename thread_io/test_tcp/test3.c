#include <stdio.h>

int main(void)
{
	int i;
	for(i = 0;i < 16;i++)
	printf("%2x %2x\n",(i & 1),(i / 2));
}
