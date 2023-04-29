#ifndef LIBASE_INTERNAL_H_
#define LIBASE_INTERNAL_H_

#include <stdio.h>

//#define __ABUFF_DEBUG__

#ifdef __ABUFF_DEBUG__
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
	#define _debug(...)           do { printf("debug: " __VA_ARGS__); putchar('\n'); } while(0);
#elif defined __GNUC__
	#define _debug(fmt, args...)  do { printf("debug: " fmt, ## args); putchar('\n'); } while(0);
#endif
#else
	#define _debug(fmt, args...)
#endif

#endif
