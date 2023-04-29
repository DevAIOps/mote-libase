#include <stdarg.h>

#include "testing.h"

static int wrap_printf(char *buf, size_t len, const char *fmt, ...)
{
	int rc;
	va_list args;

	va_start(args, fmt);
	rc = vsnprintf(buf, len, fmt, args);
	va_end(args);

	return rc;
}

DEF_TEST(platform_dep) {
	int rc;
	char buff[1024];

	rc = wrap_printf(buff, 5, "123%s", "45");
	EXPECT_EQ_INT(5, rc);
	EXPECT_EQ_INT(4, strlen(buff));
	EXPECT_EQ_STR("1234", buff);

	rc = wrap_printf(buff, 5, "123%d", 45);
	EXPECT_EQ_INT(5, rc);
	EXPECT_EQ_INT(4, strlen(buff));
	EXPECT_EQ_STR("1234", buff);

	rc = wrap_printf(buff, 1, "123%d", 45);
	EXPECT_EQ_INT(5, rc);
	EXPECT_EQ_INT(0, strlen(buff));
	EXPECT_EQ_STR("", buff);

	return 0;
}

int main(void)
{
	RUN_TEST(platform_dep);
	END_TEST;
	return 0;
}

