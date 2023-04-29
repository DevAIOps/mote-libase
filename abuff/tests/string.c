#include <errno.h>

#include "../abuff.h"
#include "testing.h"

DEF_TEST(basic) {
	int rc;
	struct abuff *buf;

	buf = abuff_create(4, 16);
	CHECK_NOT_NULL(buf);

	rc = abuff_printf(buf, "12345%s", "67890");
	EXPECT_EQ_INT(10, rc);            /* normal */
	EXPECT_EQ_INT(10, abuff_length(buf));
	EXPECT_EQ_STR_LEN("1234567890", abuff_data(buf), 10);

	// format failed, no message changed
	rc = abuff_printf(buf, "12345%j");
	EXPECT_EQ_STR_LEN("1234567890", abuff_data(buf), 10);

	rc = abuff_printf(buf, "12345");  /* border */
	EXPECT_EQ_INT(5, rc);
	EXPECT_EQ_INT(15, abuff_length(buf));
	EXPECT_EQ_STR("123456789012345", abuff_data(buf)); /* it's safe */
	EXPECT_EQ_STR_LEN("123456789012345", abuff_data(buf), 15);

	rc = abuff_printf(buf, "1");      /* overflow */
	EXPECT_EQ_INT(-ENOSPC, rc);
	EXPECT_EQ_INT(15, abuff_length(buf));

	abuff_string_seal(buf);           /* add NULL termination */
	EXPECT_EQ_INT(16, abuff_length(buf));
	EXPECT_EQ_STR("123456789012345", abuff_data(buf)); /* it's safe */
	abuff_string_seal(buf);
	EXPECT_EQ_STR("123456789012345", abuff_data(buf));

	abuff_trim_null(buf);
	EXPECT_EQ_INT(15, abuff_length(buf));
	abuff_destroy(buf);

	buf = abuff_create(4, 16);
	CHECK_NOT_NULL(buf);
	rc = abuff_printf(buf, "12345%s", "67890");
	EXPECT_EQ_INT(10, rc);            /* normal */
	rc = abuff_printf(buf, "123456"); /* one byte overflow */
	EXPECT_EQ_INT(-ENOSPC, rc);
	EXPECT_EQ_INT(10, abuff_length(buf));
	EXPECT_EQ_STR("1234567890", abuff_string(buf));
        abuff_destroy(buf);

	return 0;
}

int main(void)
{
	RUN_TEST(basic);
	END_TEST;
	return 0;
}

