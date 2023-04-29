#ifndef LIBASE_ABUFF_ABUFF_H_
#define LIBASE_ABUFF_ABUFF_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../helper.h"

#ifndef ABUFF_SIZE_MIN
#define ABUFF_SIZE_MIN    64
#endif

struct abuff {
	size_t min;
	size_t max;
	size_t real;

	uint8_t *head;
	uint8_t *data;
	uint8_t *tail;
	uint8_t *end;
};


// init
struct abuff *abuff_create(size_t smin, size_t smax);
void abuff_destroy(struct abuff *buf);

int abuff_init(struct abuff *buf, size_t smin, size_t smax);
void abuff_cleanup(struct abuff *buf);

int abuff_reinit(struct abuff *buf, size_t smin, size_t smax);


// string
int abuff_printf(struct abuff *buf, const char *fmt, ...);
int abuff_vprintf(struct abuff *buf, const char *fmt, va_list ap);


// resize
void abuff_shift(struct abuff *buf);
size_t abuff_expand(struct abuff *buf, size_t expect);
size_t abuff_exponent_expand(struct abuff *buf);
size_t abuff_ensure_size(struct abuff *buf, size_t len);


static inline size_t abuff_headroom(const struct abuff *b)
{
	return b->data - b->head;
}

static inline size_t abuff_tailroom(const struct abuff *b)
{
	return b->end - b->tail;
}

static inline size_t abuff_leftroom(const struct abuff *b)
{
	return b->data - b->head + b->end - b->tail;
}

static inline size_t abuff_length(const struct abuff *b)
{
	return b->tail - b->data;
}

static inline void *abuff_data(const struct abuff *b)
{
	return b->data;
}

static inline char *abuff_string(const struct abuff *b)
{
	return (char *)b->data;
}

static inline void *abuff_tail(const struct abuff *b)
{
	return b->tail;
}

static inline size_t abuff_size(const struct abuff *b)
{
	return b->real;
}

static inline void abuff_restart(struct abuff *b)
{
	b->data = b->head;
	b->tail = b->head;
}

static inline void abuff_tail_step_fast(struct abuff *b, size_t len)
{
	b->tail += len;
}

static inline void abuff_data_step_fast(struct abuff *b, size_t len)
{
	b->data += len;
}

static inline size_t abuff_drain(struct abuff *b, size_t step)
{
	size_t len;

	abuff_data_step_fast(b, step);
	len = abuff_length(b);
	if (len <= 0) {
		abuff_restart(b);
		return 0;
	}

	return len;
}

static inline void abuff_trim_null(struct abuff *b)
{
	uint8_t *ptr;
	for (ptr = b->tail - 1; ptr >= b->data; ptr--)
		if (*ptr != 0)
			break;
	if (ptr != b->tail)
		b->tail = ptr + 1;
}

static inline void abuff_string_seal(struct abuff *b)
{
	if (b->end == b->tail) {
		*(b->tail - 1) = 0;
	} else {
		*(b->tail) = 0;
		b->tail++;
	}
}

static inline void abuff_seal(struct abuff *b)
{
	if (b->end == b->tail) {
		*(b->tail - 1) = 0;
	} else {
		*(b->tail) = 0;
	}
}

#define abuff_seal_at(b, off) do {                    \
	if ((off) >= (int)((b)->tail - (b)->head)) { \
		off = (int)((b)->tail - (b)->head);  \
		if ((b)->tail == (b)->end)            \
			off--;                        \
	}                                             \
	(b)->tail = (b)->head + off;                 \
	*((b)->head + off) = 0;                      \
} while(0)

static inline void abuff_must_push(struct abuff *b, char c)
{
	if ((b)->end == (b)->tail) {
		*((b)->tail - 1) = c;
	} else {
		*(b)->tail = c;
		b->tail++;
	}
}

static inline __unused void abuff_put_u8_fast(struct abuff *b, uint8_t val)
{
	uint8_t *ptr;
	ptr = (uint8_t *)b->tail;
	*ptr = val;
	b->tail += 1;
}

static inline __unused uint8_t abuff_get_u8_fast(struct abuff *b)
{
	uint8_t ret;
	ret = *((uint8_t *)b->data);
	b->data += 1;
	return ret;
}

static inline __unused void abuff_put_u16_fast(struct abuff *b, uint16_t val)
{
	uint16_t *ptr;
	ptr = (uint16_t *)b->tail;
	*ptr = val;
	b->tail += 2;
}

static inline __unused uint16_t abuff_get_u16_fast(struct abuff *b)
{
	uint16_t ret;
	ret = *((uint16_t *)b->data);
	b->data += 2;
	return ret;
}

static inline __unused void abuff_put_u64_fast(struct abuff *b, uint64_t val)
{
	uint64_t *ptr;
	ptr = (uint64_t *)b->tail;
	*ptr = val;
	b->tail += 8;
}

static inline __unused uint64_t abuff_get_u64_fast(struct abuff *b)
{
	uint64_t ret;
	ret = *((uint64_t *)b->data);
	b->data += 4;
	return ret;
}

#define abuff_is_full(b)  ((b)->max > 0 && (b)->real >= (b)->max && (b)->tail >= (b)->end)

#define abuff_step(b, s)  ((b)->tail += (s)) /* check if there are enough space */

size_t abuff_remain(struct abuff *buf);

size_t abuff_pull_data(struct abuff *buf, void *dest, size_t len);

size_t abuff_put_data(struct abuff *buf, const void *data, size_t len);
#define abuff_put_string(buf, data) abuff_put_data(buf, data, strlen(data))

size_t abuff_shrink(struct abuff *buf);

#endif

