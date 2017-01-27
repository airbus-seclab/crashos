#ifndef __START_H__
#define __START_H__

#include <core/types.h>
#include <core/multiboot.h>

/**
 * Start the OS
 * @param mbi_t* the multiboot structure information
 */
void __attribute__((regparm(1))) start(mbi_t*);

/**
 * Set the kernel in panic
 */
void panic();

#endif
