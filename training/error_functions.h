#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

/* The ellipses mean that there are a variable number of arguments */
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

void errExit(const char *format,...) NORETURN;
void err_exit(const char *format,...) NORETURN;
void fatal(const char *format,...) NORETURN;
void usageErr(const char *format,...) NORETURN;
void cmdLineErr(const char *format,...) NORETURN;

#endif

