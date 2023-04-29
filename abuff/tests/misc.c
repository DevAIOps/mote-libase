#include "testing.h"

#include "../abuff.h"

DEF_TEST(basic) {
	struct abuff *buf;
	buf = abuff_create(8, 64);
        CHECK_NOT_NULL(buf);
	abuff_put_u8_fast(buf, 0x81);
	EXPECT_EQ_INT(1, abuff_length(buf));
	EXPECT_EQ_INT(0x81, abuff_get_u8_fast(buf));
        abuff_destroy(buf);
	return 0;
}

int main(void)
{
	RUN_TEST(basic);
	END_TEST;
	return 0;
}

