#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

/* The ellipses mean that there are a variable number of arguments */
/* prints a message on std error */
void errMsg(const char *format,...);

/* GNU compiler */
#ifdef _GNUC_
/* This macro stops 'gcc -Wall' complaining that "control reaches end of
 * non-void function" if we use the following functions to terminate
 * main() or some other non-void function. */

/* this attribute, attached to a variable, means that the variable is meant to be possibly unused */
#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

/* print error and terminate the program */
void errExit(const char *format,...) NORETURN;
/* similar, but (1) doesn't flush std opt (2) terminate by _exit() not exit() */
void err_exit(const char *format,...) NORETURN;
/* It prints the error number rather than text  */
void errExitEN(int errnum,const char *format,...) NORETURN;
/* diagnose general errors */
void fatal(const char *format,...) NORETURN;
/* diagnose errors in command line argument usage */
void usageErr(const char *format,...) NORETURN;
/* diagnose errors cmd args specified in program */
void cmdLineErr(const char *format,...) NORETURN;

#endif

