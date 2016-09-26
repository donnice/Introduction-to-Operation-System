#include "apue.h"

int main(void)
{
	int c;
	while((c=getc(stdin))!=EOF)
		if(putc(c,stdout) == EOF)
		{
			printf("output error!\n");
			exit(1);
		}

		if(ferror(stdin))
		{
			printf("input error!\n");
			exit(1);
		}

		exit(0);
}
