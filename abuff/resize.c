#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "abuff.h"
#include "internal.h"

static inline void abuff_do_shift(struct abuff *buf)
{
	size_t len = abuff_length(buf);
	memmove(buf->head, buf->data, len);
	buf->data = buf->head;
	buf->tail = buf->head + len;
}

void abuff_shift(struct abuff *buf)
{
	size_t len, head;

	head = abuff_headroom(buf);
	if (head <= 0)
		return;

	len = abuff_length(buf);
	if (len <= 0) {
		abuff_restart(buf);
		return;
	}

	memmove(buf->head, buf->data, len);
	buf->data = buf->head;
	buf->tail = buf->head + len;
}

static size_t abuff_do_resize(struct abuff *buf, size_t nsz)
{
	uint8_t *tmp;
	size_t osz; /* orignal size */

	assert(buf->head == buf->data);
	_debug("resize >>>>>> real=%ld max=%ld tailroom=%ld new=%ld len=%ld",
		buf->real, buf->max, abuff_tailroom(buf), nsz, abuff_length(buf));

	tmp = xrealloc(buf->head, nsz);
	if (tmp == NULL) /* use original */
		return abuff_tailroom(buf);
	osz = abuff_length(buf);

	buf->head = tmp;
	buf->data = tmp;
	buf->end = buf->head + nsz;
	buf->tail = buf->head + osz;
	buf->real = nsz;

	_debug("resize <<<<<< real=%ld max=%ld tailroom=%ld",
		buf->real, buf->max, abuff_tailroom(buf));

	return abuff_tailroom(buf);
}

#define abuff_check_size(buf) do {                             \
	if (unlikely(buf->max > 0 && buf->real >= buf->max)) { \
		_debug("reach top size %ld", buf->max);        \
		return abuff_tailroom(buf);                    \
	}                                                      \
} while(0)

size_t abuff_exponent_expand(struct abuff *buf)
{
	size_t nsz;

	abuff_check_size(buf);

	nsz = buf->real * 2;
	if (nsz > buf->max)
		nsz = buf->max;

	return abuff_do_resize(buf, nsz);
}

size_t abuff_expand(struct abuff *buf, size_t expect)
{
	size_t nsz; /* new size */

	abuff_check_size(buf);

	for (nsz = buf->real; nsz < expect && nsz < buf->max; nsz *= 2);
	if (nsz > buf->max)
		nsz = buf->max;

	abuff_shift(buf);
	return abuff_do_resize(buf, nsz);
}

size_t abuff_remain(struct abuff *buf)
{
	size_t left, nsz;

	left = abuff_tailroom(buf);
	if (left > 0)
		return left;

	left = abuff_headroom(buf);
	if (left > 0) {
		abuff_do_shift(buf);
		return left;
	}
	assert(buf->head == buf->data);

	if (buf->max > 0 && buf->real >= buf->max) {
		_debug("try expand failed, reach top size %ld", buf->max);
		return 0;
	}

	nsz = buf->real * 2;
	if (nsz > buf->max)
		nsz = buf->max;
	_debug("expand to %ld", nsz);
	return abuff_do_resize(buf, nsz);
}

size_t abuff_ensure_size(struct abuff *buf, size_t len)
{
	size_t left;

	left = abuff_tailroom(buf);
	if (likely(left >= len)) {
		_debug("enough tail room, len=%ld real=%ld max=%ld left=%ld",
			len, buf->real, buf->max, left);
		return left;
	}

	left = abuff_leftroom(buf);
	if (likely(left >= len)) {
		_debug("enough room, len=%ld real=%ld max=%ld left=%ld",
			len, buf->real, buf->max, left);
		abuff_do_shift(buf);
		return left;
	}

	left = abuff_expand(buf, buf->real + (len - left));
	if (likely(left >= len)) {
		_debug("resized, len=%ld real=%ld max=%ld left=%ld",
			len, buf->real, buf->max, left);
		return left;
	} else if (left <= 0) {
		_debug("full now, len=%ld max=%ld", len, buf->max);
		return 0;
	}

	_debug("partial space, len=%ld real=%ld max=%ld left=%ld",
			len, buf->real, buf->max, left);
	return left;
}

// shrink half every time
size_t abuff_shrink(struct abuff *buf)
{
	size_t osz, nsz;

	if (buf->real <= buf->min) {
		return buf->real;
	}

	nsz = buf->real / 2;
	osz = abuff_length(buf);
	if (osz > nsz) {
		return buf->real;
	}

	_debug("shrink >>>>>>, real=%d max=%d length=%d nsz=%d",
			buf->real, buf->max, osz, nsz);
	abuff_shift(buf);
	abuff_do_resize(buf, nsz);
	return buf->real;
}
