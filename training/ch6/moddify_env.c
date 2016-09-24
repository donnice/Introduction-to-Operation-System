#define _GNU_SOURCE	/* to get various declaration from <stdlib.g> */
#include <stdlib.h>
#include "tlpi_hdr.h"

extern char **environ;

int main(int argc, char *argv[])
{
	int j;
	char **ep;

	clearenv();		/* Erase entire environment */

	for(j = 1; j < argc; j++)
		if(putenv(agrv[j])!=0)
			errExit("putenv:%s",argv[j]);
	

}
