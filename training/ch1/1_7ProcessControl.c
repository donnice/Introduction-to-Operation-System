#include <stdio.h>
#include <sys/wait.h>

#define MAXLINE 100

int main(void)
{
	char buf[MAXLINE];
	pid_t pid;
	int status;

	printf("%% ");
	while(fgets(buf,MAXLINE,stdin)!=NULL) {
		if(buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;		/*replace new line with null*/

		if((pid=fork())<0)
			printf("fork error!\n");
		else if(pid == 0) {
			execlp(buf,buf,(char *)0);	/* google it */
			printf("Could't execute:%s",buf);
			exit(127);
		}

		/* parent */
		if((pid = waitpid(pid,&status,0)) < 0)
			printf("waitpid error\n");
		printf("%% ");
	}
	exit(0);
}
