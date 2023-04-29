#include <assert.h>
#include <string.h>

#include "abuff.h"

size_t abuff_put_data(struct abuff *buf, const void *data, size_t len)
{
	size_t left;

	left = abuff_ensure_size(buf, len);
	if (left < len)
		len = left;
	memcpy(buf->tail, data, len);
	buf->tail += len;
	assert(buf->end >= buf->tail);

	return len;
}

/* remove data from the start of a buffer and return the actual length removed. */
size_t abuff_pull_data(struct abuff *buf, void *dest, size_t len)
{
	size_t curr;

	curr = abuff_length(buf);
	if (len > curr)
		len = curr;
	if (dest)
		memcpy(dest, buf->data, len);
	buf->data += len;

	return len;
}

