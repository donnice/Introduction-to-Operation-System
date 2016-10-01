#include "apue.h"
#include <sys/wait.h>

#define MAXLINE 100

int main(void)
{
	char buf[MAXLINE];
	pid_t pid;
	int status;

	printf("%% ");
	while(fgets(buf,MAXLINE,stdin)!=NULL) {
	
	}
}
