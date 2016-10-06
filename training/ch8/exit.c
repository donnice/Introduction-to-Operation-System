#include "apue.h"
#include <sys/wait.h>

void pr_exit(int status)
{
	if(WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
			WEXITSTATUS(status));
	else if(WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",
			WTERMSIG(status),
#ifdef	WCOREDUMP
			WCOREDUMP(status)? "(core file generated)":"");
#endif
	else if(WTFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
			WSTOPSIG(status));
}

int main(void)
{
	pid_t pid;
	int status;

	if((pid=fork())<0)
		printf("fork error\n");
	
	else if(pid == 0)	/* child */
		exit(7);
	
	if(wait(&status)!=pid)
		printf("wait error\n");
	pr_exit(status);
}
