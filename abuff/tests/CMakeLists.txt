SET(BASE_COMMON_SRC
	../init.c
	../helper.c
	../../mock.c
)

# some basic dependence for glibc, mainly for string format functions.
ADD_EXECUTABLE(libase-abuff-platform platform.c)
ADD_TEST(NAME libase-abuff-platform COMMAND ${WRAP_CMD} ./libase-abuff-platform)
SET_TESTS_PROPERTIES(libase-abuff-platform PROPERTIES FAIL_REGULAR_EXPRESSION "reachable")

ADD_EXECUTABLE(libase-abuff-init init.c ../resize.c ${BASE_COMMON_SRC})
ADD_TEST(NAME libase-abuff-init COMMAND ${WRAP_CMD} ./libase-abuff-init)
TARGET_COMPILE_OPTIONS(libase-abuff-init PRIVATE "-D__TEST__")
SET_TESTS_PROPERTIES(libase-abuff-init PROPERTIES FAIL_REGULAR_EXPRESSION "reachable")

ADD_EXECUTABLE(libase-abuff-resize resize.c ../resize.c ${BASE_COMMON_SRC})
ADD_TEST(NAME libase-abuff-resize COMMAND ${WRAP_CMD} ./libase-abuff-resize)
TARGET_COMPILE_OPTIONS(libase-abuff-resize PRIVATE "-D__TEST__")
SET_TESTS_PROPERTIES(libase-abuff-resize PROPERTIES FAIL_REGULAR_EXPRESSION "reachable")

ADD_EXECUTABLE(libase-abuff-string string.c ../string.c ../resize.c ${BASE_COMMON_SRC})
ADD_TEST(NAME libase-abuff-string COMMAND ${WRAP_CMD} ./libase-abuff-string)
SET_TESTS_PROPERTIES(libase-abuff-string PROPERTIES FAIL_REGULAR_EXPRESSION "reachable")

ADD_EXECUTABLE(libase-abuff-misc misc.c ../init.c ../../mock.c)
ADD_TEST(NAME libase-abuff-misc COMMAND ${WRAP_CMD} ./libase-abuff-misc)
SET_TESTS_PROPERTIES(libase-abuff-misc PROPERTIES FAIL_REGULAR_EXPRESSION "reachable")

