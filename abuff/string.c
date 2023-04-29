#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "abuff.h"
#include "internal.h"

/* restore to the original position */
static inline void abuff_string_seal_at_fast(const struct abuff *b, size_t pos)
{
	*(b->data + pos) = 0;
}

/* NOTE: this function doesn't include the NULL termination. */
int abuff_vprintf(struct abuff *buf, const char *fmt, va_list ap)
{
	int rc;
	va_list args;
	size_t left, orig;

	orig = abuff_length(buf);
	left = abuff_tailroom(buf);
	_debug("vprintf tailroom %ld length %ld.", left, orig);

	va_copy(args, ap);
	rc = vsnprintf(abuff_tail(buf), left, fmt, ap);
	if (unlikely(rc < 0)) {
		_debug("vprintf falied, %d:%s.", errno, strerror(errno));
		abuff_string_seal_at_fast(buf, orig);
		return rc;
	} else if (rc >= (int)left) { /* including NULL termination */
		left = abuff_expand(buf, abuff_length(buf) + rc + 1);
		if (rc >= (int)left) {
			abuff_string_seal_at_fast(buf, orig);
			return -ENOSPC;
		}
		rc = vsnprintf(abuff_tail(buf), left, fmt, args);
	}
	va_end(args);

	assert(rc >= 0);
	abuff_tail_step_fast(buf, rc);
	return rc;
}

int abuff_printf(struct abuff *buf, const char *fmt, ...)
{
	int rc;
	va_list args;

	va_start(args, fmt);
	rc = abuff_vprintf(buf, fmt, args);
	va_end(args);

	return rc;
}

