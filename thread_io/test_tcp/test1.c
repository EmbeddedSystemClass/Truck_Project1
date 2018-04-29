#include <stdio.h>

// index: 0->40
// bank: 0->5 (PORTA->F)
// index: 0->7 for all ports but C & F which are 0->3

int main(void)
{
	int index,index2,i,bank;
	unsigned char mask;

	for(index = 0;index < 40;index++)
	{
		mask = 1;
		if(index > 19)
			index2 = index+4;
		else index2 = index;

		printf("%d:\t",index);

		bank = index2/8;
		mask <<= index2-bank*8;
		printf("bank: %d\tmask: %2x\n",bank,mask);

	}
	return 0;
}
