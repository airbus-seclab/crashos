// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __STRING_H__
#define __STRING_H__

#include <core/types.h>

/**
 * Fill the memory by a value
 * @param dst the address of memory to fill
 * @param c the value
 * @param size the size of the buffer to fill
 */
void* memset(void *dst, int c, size_t size);

/**
 * Copy one buffer in an other
 * @param dst0 the address of the destination buffer
 * @param src0 the address of the buffer to copy
 * @param size the size of the source buffer
 */
void* memcpy(void *dst0, const void *src0, unsigned int size);

/**
 * Match two strings
 * @param s1 first string address
 * @param s2 second string address
 * @param size the string size to compare
 */
int strcmp(void *str1, void *str2, unsigned int size);

#endif
