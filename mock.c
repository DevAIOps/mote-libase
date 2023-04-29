#include <stdlib.h>
#include <string.h>

int (*malloc_checker)(void);

void *(*malloc_func)(size_t) = malloc;
void *(*calloc_func)(size_t, size_t) = calloc;
void *(*realloc_func)(void *, size_t) = realloc;

void *mock_malloc_user(size_t size)
{
	if (malloc_checker == NULL)
		return NULL;
	if (malloc_checker())
		return malloc(size);
	return NULL;
}

void *mock_malloc_null(size_t size)
{
	(void) size;
	return NULL;
}

void *mock_realloc_null(void *ptr, size_t size)
{
	(void) ptr;
	(void) size;
	return NULL;
}

void *mock_calloc_null(size_t nmemb, size_t size)
{
	(void) size;
	(void) nmemb;
	return NULL;
}

void *mock_malloc(size_t size)
{
	return malloc_func(size);
}

void *mock_realloc(void *ptr, size_t size)
{
	return realloc_func(ptr, size);
}

void *mock_calloc(size_t nmemb, size_t size)
{
	return calloc_func(nmemb, size);
}

void mock_free(void *ptr)
{
	free(ptr);
}

char *mock_strdup(const char *ptr)
{
	return strdup(ptr);
}

char *mock_strndup(const char *ptr, const size_t size)
{
	return strndup(ptr, size);
}
