// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __TEST_H__
#define __TEST_H__

#include <core/types.h>
/**
 * Test information
 */
typedef struct test_s {
	char *name;			/** The name of the test */
	char *desc;			/** The description of the test */
	int (*init)(void);	/** The init function */
	int (*test)(void);	/** The test function */
	int (*fini)(void);	/** The restore function */
} test_t;

/**
 * Declare a test_t as a test and store it in the .tests section
 */
#define DECLARE_TEST(xXx)   test_t * __attribute__((section(".tests"))) xXx##_ptr = &xXx;

void tests();

#endif
