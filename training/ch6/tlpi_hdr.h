#ifndef TLPI_HDR_H			/* prevent accidental double inclusion */
#define TLPI_HDR_H

#include <sys/types.h>		/* Type def used by many programs */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>			/* Prototypes for many sys calls */
#include <errno.h>			/* Declares errno and defines error const */
#include <string.h>

#include "get_num.h"		/* getInt(),getLong() */
#include "error_functions.h"

typedef enum {FALSE,TRUE} Boolean;

#define min(m,n) ((m)<(n)?(m):(n))
#define max(m,n) ((m)>(n)?(m):(n))

#endif
