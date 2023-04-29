#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "abuff.h"

static inline void update_size(struct abuff *buf, size_t smin, size_t smax)
{
	buf->min = smin;
	buf->max = smax;
	buf->real = buf->min;
}

static inline void update_pointer(struct abuff *buf)
{
	buf->tail = buf->head;
	buf->data = buf->head;
	buf->end = buf->head + buf->real;
}

static inline int is_invalid_size(size_t smin, size_t smax)
{
	if (smin == 0 || smin > smax)
		return 1;
	return 0;
}

int abuff_reinit(struct abuff *buf, size_t smin, size_t smax)
{
	uint8_t *tmp;

	if (unlikely(buf == NULL || is_invalid_size(smin, smax)))
		return -EINVAL;

	if (buf->min == smin && buf->max == smax)
		return 0;

	tmp = xrealloc(buf->head, smin);
	if (tmp == NULL)
		return -ENOMEM;

	update_size(buf, smin, smax);
	buf->head = tmp;
	update_pointer(buf);

	return 0;
}

static int do_init(struct abuff *buf, size_t smin, size_t smax)
{
	update_size(buf, smin, smax);

	buf->head = xmalloc(buf->real);
	if (unlikely(buf->head == NULL))
		return -ENOMEM;

	update_pointer(buf);

	return 0;
}

int abuff_init(struct abuff *buf, size_t smin, size_t smax)
{
	if (unlikely(buf == NULL || is_invalid_size(smin, smax)))
		return -EINVAL;

	return do_init(buf, smin, smax);
}

void abuff_cleanup(struct abuff *buf)
{
	if (unlikely(buf == NULL))
		return;
	if (likely(buf->head != NULL))
		xfree(buf->head);
	buf->real = 0;
	buf->head = NULL;
	buf->data = NULL;
	buf->tail = NULL;
	buf->end = NULL;
}

void abuff_destroy(struct abuff *buf)
{
	if (unlikely(buf == NULL))
		return;

	if (likely(buf->head != NULL))
		xfree(buf->head);
	xfree(buf);
}

struct abuff *abuff_create(size_t smin, size_t smax)
{
	struct abuff *buf;

	if (unlikely(is_invalid_size(smin, smax)))
		return NULL;

	buf = (struct abuff *)xmalloc(sizeof(struct abuff));
	if (unlikely(buf == NULL))
		return NULL;

	if (do_init(buf, smin, smax) < 0) {
		xfree(buf);
		return NULL;
	}

	return buf;
}

