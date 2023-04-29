#ifndef LIBASE_MOCK_H_
#define LIBASE_MOCK_H_

#include <stdlib.h>

extern int (*malloc_checker)(void);

extern void *(*malloc_func)(size_t);
extern void *(*calloc_func)(size_t, size_t);
extern void *(*realloc_func)(void *, size_t);

void *mock_malloc_user(size_t size);

void *mock_malloc_null(size_t size);
void *mock_realloc_null(void *ptr, size_t size);
void *mock_calloc_null(size_t nmemb, size_t size);

void *mock_malloc(size_t size);
void *mock_realloc(void *ptr, size_t size);
void *mock_calloc(size_t nmemb, size_t size);

#endif
