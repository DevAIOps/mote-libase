CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Determine this library is built as a subproject (using add_subdirectory) or master project.
SET(MASTER_PROJECT OFF)
IF(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    PROJECT(mote-libase-abuff)

    SET(MASTER_PROJECT ON)
    MESSAGE(STATUS "CMake version: ${CMAKE_VERSION}")

    SET(LIBASE_ABUFF_SRC
        init.c
        helper.c
        resize.c
        string.c
    )
    ADD_LIBRARY(ase-abuff STATIC ${LIBASE_ABUFF_SRC})
ENDIF()

IF(WITH_CONVERAGE AND MASTER_PROJECT)
	SET(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

	INCLUDE(CodeCoverage)
    SET(COVERAGE_EXCLUDES "'*mock*'")
	SETUP_TARGET_FOR_COVERAGE(coverage ctest covdir)
ENDIF()

IF(WITH_UNIT_TESTS AND MASTER_PROJECT)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(tests)
ENDIF()

IF(WITH_EXAMPLES AND MASTER_PROJECT)
    ADD_SUBDIRECTORY(examples)
ENDIF()

