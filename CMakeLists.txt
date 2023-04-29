SET(LIBASE_SRC
	abuff/init.c
	abuff/helper.c
	abuff/resize.c
	abuff/string.c

	mock.c               mock.h
)

ADD_LIBRARY(ase STATIC ${LIBASE_SRC})

ADD_SUBDIRECTORY(abuff)
