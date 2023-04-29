#include "testing.h"

#include "../abuff.h"
#include "../../mock.h"

DEF_TEST(ensure) {
	char buffer[32];
	struct abuff *buf;

	buf = abuff_create(2, 16);
	CHECK_NOT_NULL(buf);

	// have enough space, no need to expand
	EXPECT_EQ_INT(2, abuff_ensure_size(buf, 2));

	// no space now, need to expand
	EXPECT_EQ_INT(8, abuff_ensure_size(buf, 7));

	// have enough space, only need to move some space
	EXPECT_EQ_INT(6, abuff_put_data(buf, "123456", 6));
	EXPECT_EQ_INT(2, abuff_pull_data(buf, buffer, 2));
	abuff_data_step_fast(buf, 2);
	EXPECT_EQ_INT(6, abuff_leftroom(buf));
	EXPECT_EQ_INT(6, abuff_ensure_size(buf, 6));
	EXPECT_EQ_STR_LEN("56", abuff_data(buf), 2);

	// no space now, only 14 bytes left to use actually
	EXPECT_EQ_INT(2, abuff_length(buf));
	EXPECT_EQ_INT(14, abuff_ensure_size(buf, 16));

	// full now
	abuff_tail_step_fast(buf, 14);
	EXPECT_EQ_INT(0, abuff_ensure_size(buf, 16));

	abuff_destroy(buf);
	return 0;
}

DEF_TEST(remain) {
	struct abuff *buf;

	buf = abuff_create(2, 3);
	CHECK_NOT_NULL(buf);

	EXPECT_EQ_INT(2, abuff_remain(buf));

	// one byte unused on the head
	abuff_tail_step_fast(buf, 2);
	EXPECT_EQ_INT(2, abuff_length(buf));
	abuff_data_step_fast(buf, 1);
	EXPECT_EQ_INT(1, abuff_remain(buf));

	// no space now, raise the size with exponent
	abuff_tail_step_fast(buf, 1);
	EXPECT_EQ_INT(1, abuff_remain(buf));

	// full now
	abuff_tail_step_fast(buf, 1);
	EXPECT_EQ_INT(0, abuff_remain(buf));

	abuff_destroy(buf);
	return 0;
}

DEF_TEST(expand) {
	struct abuff *buf;

	buf = abuff_create(2, 16);
	CHECK_NOT_NULL(buf);
	EXPECT_EQ_INT(2, abuff_leftroom(buf));
	EXPECT_EQ_INT(4, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(8, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(16, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(16, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(16, abuff_exponent_expand(buf));
	NOT_OK(abuff_is_full(buf));
	abuff_destroy(buf);

	buf = abuff_create(2, 13);
	CHECK_NOT_NULL(buf);
	EXPECT_EQ_INT(2, abuff_leftroom(buf));
	EXPECT_EQ_INT(4, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(8, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(13, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(13, abuff_exponent_expand(buf));
	EXPECT_EQ_INT(13, abuff_exponent_expand(buf));
	NOT_OK(abuff_is_full(buf));
	abuff_destroy(buf);

	buf = abuff_create(2, 3);
	CHECK_NOT_NULL(buf);
	EXPECT_EQ_INT(3, abuff_expand(buf, 4));
	abuff_destroy(buf);

	return 0;
}

DEF_TEST(shift) {
	struct abuff *buf;
	buf = abuff_create(8, 16);
	CHECK_NOT_NULL(buf);

	// no need to move
	abuff_tail_step_fast(buf, 1);
	abuff_shift(buf);
	OK(buf->data == buf->head);

	// no data already, only need to reset the pointers
	abuff_data_step_fast(buf, 1);
	EXPECT_EQ_INT(0, abuff_length(buf));
	abuff_shift(buf);
	OK(buf->data == buf->head);
	OK(buf->data == buf->tail);

	// some data need to move
	abuff_tail_step_fast(buf, 6);
	abuff_data_step_fast(buf, 4);
	abuff_shift(buf);
	OK(buf->data == buf->head);

	abuff_destroy(buf);
	return 0;
}

DEF_TEST(move) {
	char buff[32];
	struct abuff *buf;
	buf = abuff_create(2, 8);
	CHECK_NOT_NULL(buf);
	EXPECT_EQ_INT(8, abuff_put_data(buf, "123456789", 9));
	EXPECT_EQ_INT(8, abuff_length(buf));

	EXPECT_EQ_INT(8, abuff_pull_data(buf, buff, sizeof(buff)));
	abuff_destroy(buf);
	return 0;
}

DEF_TEST(invalid) {
	struct abuff *buf;
	buf = abuff_create(2, 16);
	CHECK_NOT_NULL(buf);

	realloc_func = mock_realloc_null;
	EXPECT_EQ_INT(2, abuff_expand(buf, 5));
	realloc_func = realloc;

	abuff_destroy(buf);
	return 0;
}

DEF_TEST(shrink) {
	struct abuff *buf;
	buf = abuff_create(4, 16);
	CHECK_NOT_NULL(buf);

	// smaller than the min size, no need to shrink.
	EXPECT_EQ_INT(4, abuff_shrink(buf));
	EXPECT_EQ_INT(3, abuff_put_data(buf, "123", 3));
	EXPECT_EQ_INT(4, abuff_shrink(buf));

	// still have some data, no need to shrink.
	EXPECT_EQ_INT(4, abuff_put_data(buf, "4567", 4));
	EXPECT_EQ_INT(7, abuff_length(buf));
	EXPECT_EQ_INT(8, abuff_size(buf));
	EXPECT_EQ_INT(8, abuff_shrink(buf));

	// need to shrink.
	EXPECT_EQ_INT(3, abuff_pull_data(buf, NULL, 3));
	EXPECT_EQ_INT(4, abuff_shrink(buf));
	EXPECT_EQ_STR_LEN("4567", abuff_data(buf), 4);

	abuff_destroy(buf);
	return 0;
}

int main(void)
{
	RUN_TEST(ensure);
	RUN_TEST(remain);
	RUN_TEST(expand);
	RUN_TEST(shift);
	RUN_TEST(invalid);
	RUN_TEST(move);
	RUN_TEST(shrink);
	END_TEST;
	return 0;
}

