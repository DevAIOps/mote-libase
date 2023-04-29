#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

#include "testing.h"

#include "../abuff.h"
#include "../../mock.h"

static int count;
static int malloc_times(void)
{
	if (++count <= 1)
		return 1;
	return 0;
}

DEF_TEST(invalid) {
	struct abuff buf, *tmp;

	malloc_func = mock_malloc_user;
	CHECK_NULL(abuff_create(16, 256));
	malloc_func = malloc;

	malloc_checker = malloc_times;
	malloc_func = mock_malloc_user;
	CHECK_NULL(abuff_create(16, 256));
	malloc_func = malloc;

	EXPECT_EQ_INT(-EINVAL, abuff_init(NULL, 16, 256));
	EXPECT_EQ_INT(-EINVAL, abuff_init(&buf, 16, 8));
	EXPECT_EQ_INT(-EINVAL, abuff_init(&buf, 0, 8));

	EXPECT_EQ_INT(-EINVAL, abuff_reinit(NULL, 16, 256));
	EXPECT_EQ_INT(-EINVAL, abuff_reinit(&buf, 16, 8));
	EXPECT_EQ_INT(-EINVAL, abuff_reinit(&buf, 0, 8));

	tmp = abuff_create(2, 16);
	CHECK_NOT_NULL(tmp);
	realloc_func = mock_realloc_null;
	EXPECT_EQ_INT(-ENOMEM, abuff_reinit(tmp, 4, 32));
	realloc_func = realloc;
	abuff_destroy(tmp);

	memset(&buf, 0, sizeof(buf));
	malloc_func = mock_malloc_null;
	EXPECT_EQ_INT(-ENOMEM, abuff_init(&buf, 16, 256));
	malloc_func = malloc;

	CHECK_NULL(abuff_create(0, 256));
	CHECK_NULL(abuff_create(10, 5));
	abuff_destroy(NULL);

	memset(&buf, 0, sizeof(buf));
	abuff_cleanup(NULL);
	buf.head = xmalloc(10);
	abuff_cleanup(&buf);

	return 0;
}

DEF_TEST(reinit) {
	struct abuff *buf;

	buf = abuff_create(2, 16);
	CHECK_NOT_NULL(buf);
	EXPECT_EQ_INT(0, abuff_reinit(buf, 2, 16));
	EXPECT_EQ_INT(0, abuff_reinit(buf, 4, 32));
	abuff_destroy(buf);

	return 0;
}

int main(void)
{
	RUN_TEST(invalid);
	RUN_TEST(reinit);

	END_TEST;
	return 0;
}

