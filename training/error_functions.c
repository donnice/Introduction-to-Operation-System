#include <stdarg.h>
#include "error_functions.h"
#include "tl[o_hdr.h"
#include "ename.c.inc"		/* Defines ename and MAX_ENAME */

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif

static void terminate(Boolean useExit3)
{
	char *s;

	/* dump core if EF_DUMPCORE is defined and is a nonempty string
	 * otherwise call exit(3) or _exit(2)
	 */
	
	s = getenv("EF_DUMPCORE");

	if(s!=NULL && *s != '\0')
		abort();
	else if(useExit3)
		exit(EXIT_FAILURE);
	else
		_exit(EXIT_FAILURE);
	/* use _exit (or its synonym _Exit) to abort the child program when the exec fails */

}

/* holding the information needed by the macros va_start, va_copy, va_arg, and va_end. */
static void outputError(Boolean useErr, int err, Boolean flushStdout,const char *format, va_list ap)
{
	#define BUF_SIZE 500
	char buf[BUF_SIZE],userMsg[BUF_SIZE],errText[BUF_SIZE];
	
	/* Write formatted data from variable argument list to sized buffer */
	vsnprintf(userMsg,BUF_SIZE,format,ap);

	if(useErr)
	{
		snprintf(errText,BUF_SIZE," [%s %s]",
				(err > 0 && err < MAX_ENAME)?
				ename[err]:"?UNKNOWN?",strerror(err));
	} else {
		snprintf(errText,BUF_SIZE,":");
	}

	snprintf(buf,BUF_SIZE,"ERROR%s %s\n",errText,userMsg);

	if(flushStdout)
		fflush(stdout);		/* Flush any pending stdout */
	fputs(buf,stderr);		/* writes a string to the specified stream up to but not including the null character */
	fflush(stderr);

}


