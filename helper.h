#ifndef LIBASE_HELPER_H_
#define LIBASE_HELPER_H_

#include <stdlib.h>
#include <string.h>

#ifdef __TEST__
	void *mock_malloc(size_t size);
	void *mock_calloc(size_t nmemb, size_t size);
	void *mock_realloc(void *ptr, size_t size);
	void mock_free(void *ptr);
	char *mock_strdup(const char *ptr);
	char *mock_strndup(const char *ptr, const size_t size);
	#define xmalloc   mock_malloc
	#define xcalloc   mock_calloc
	#define xrealloc  mock_realloc
	#define xfree     mock_free
	#define xstrdup   mock_strdup
	#define xstrndup  mock_strndup

#if 0
	/* because of sfree(), the mock api defined here */
	extern void *_test_malloc(const size_t size, const char *file, const int line);
	extern void *_test_calloc(const size_t number_of_elements, const size_t size,
				const char *file, const int line);
	extern void _test_free(void *const ptr, const char *file, const int line);
	extern char *_test_strdup(const char *ptr, const char *file, const int line);
	extern char *_test_strndup(const char *ptr, const size_t size,
				const char *file, const int line);
	#define malloc(size)       _test_malloc(size, __FILE__, __LINE__)
	#define calloc(num, size)  _test_calloc(num, size, __FILE__, __LINE__)
	#define free(ptr)          _test_free(ptr, __FILE__, __LINE__)
	#define strdup(ptr)        _test_strdup(ptr, __FILE__, __LINE__)
	#define strndup(ptr, size) _test_strndup(ptr, size, __FILE__, __LINE__)
#endif
#else
	#define xmalloc   malloc
	#define xcalloc   calloc
	#define xrealloc  realloc
	#define xfree     free
	#define xstrdup   strdup
	#define xstrndup  strndup
#endif

#ifndef MIN
	#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef MAX
	#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef __unused
	#define __unused  __attribute__((unused))
#endif

#ifndef __weak
	#define __weak    __attribute__((weak))
#endif

#ifndef likely
	#define likely(x)   __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
	#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#ifndef __inline
	#define __inline    __attribute((always_inline))
#endif

#ifndef sfree
	#define sfree(ptr) do {       \
		if (ptr != NULL) {    \
			free(ptr);    \
			(ptr) = NULL; \
		}                     \
	} while (0)
#endif

#ifndef sclose
	#define sclose(fd) do {       \
		if (fd >= 0) {        \
			close(fd);    \
			(fd) = -1;    \
		}                     \
	} while (0)
#endif

#ifndef ASIZE
	#define ASIZE(a)   (sizeof(a)/sizeof(a[0]))
#endif

#ifndef container_of
	#define container_of(ptr, type, member)                         \
		(type *)((char *)(ptr) - (char *) &((type *)0)->member)
#endif

#ifndef is_empty_string
	#define is_empty_string(s) ((s) == NULL || (s)[0] == '\0')
#endif


#endif
