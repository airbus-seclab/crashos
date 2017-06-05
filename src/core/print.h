// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __PRINT_H__
#define __PRINT_H__

#include <core/types.h>

#define  va_start(v,l)           __builtin_va_start(v,l)
#define  va_end(v)               __builtin_va_end(v)
#define  va_arg(v,l)             __builtin_va_arg(v,l)
typedef  __builtin_va_list       va_list;

/**
 * Print a message on the screen and on the serial port
 * @param format message
 * @return the number of read bytes
 */
size_t printf_log(char *format, ...);
/**
 * Print a message on the serial port
 * @param format message
 * @return the number of read bytes
 */
size_t log(char *format, ...); // print on serial port
size_t sprintf(char *format, va_list params); // to print on serial port
/**
 * Print a message on the screen
 * @param format message
 * @return the number of read bytes
 */
size_t printf(char *format, ...);
size_t vprintf(char *format, va_list params);

size_t vsnprintf(char *buffer, size_t len, char *format, va_list params);

#endif
