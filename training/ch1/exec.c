#include "apue.h"
#include <sys/wait.h>

int main(void)
{
	char buf[MAXLINE];	/* from apue.h */
	pid_t pid;
	int status;

	printf("%% ");
	while(fgets(buf,MAXLINE,stdin)!=NULL){
		if(buf[strlen(buf)-1]=="\n")
			buf[strlen(buf)-1] = 0;	/* replace newline with null */

		if((pid = fork()) < 0){
			printf("fork error!\n");
			exit(1);
		} else if(pid == 0) {	/* child */
			execlp(buf,buf,(char *)0);	/* execute file */
			printf("Could not execute: %s",buf);
			exit(127);
		}

		/* parent */
		if((pid = waitpid(pid,&status,0)) < 0) {
			printf("wait pid error!\n");
			exit(1);
		}

		printf("%% ");
	}

	exit(0);
}
