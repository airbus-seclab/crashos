// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/keyboard.h>
#include <core/io.h>
#include <core/print.h>


unsigned char poll_keyscan(void)
{
    unsigned char c,r;

    do {
	r = inb(0x64);
	if (!(r & 1)) continue; // output buffer empty
	c = inb(0x60);
    } while ((c&0x80));
    do {
	r = inb(0x64);
    }
    while (!(r&1));
    return c;
}
