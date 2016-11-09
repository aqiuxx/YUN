#ifndef _DDEBUG_
#define _DDEBUG_
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#ifdef DEBUG
#define ddebug(arg...)  printf(arg)

#else
#define ddebug(arg...)
#endif

#define syserr(arg...) do{ 	\
	fprintf(stderr,arg);	\
	fprintf(stderr,": %s\n",strerror(errno));			\
	exit(1);				\
}while(1)



#endif
