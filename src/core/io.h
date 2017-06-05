// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __IO_H__
#define __IO_H__

#include <core/types.h>

/*
** Simple I/O operations (Ramoo)
*/
/**
 * Write a byte on one IO port
 * @param d the byte to write
 * @param p the IO port number
 */
#define outb(_d_,_p_)   asm volatile ("outb %%al,  %%dx"::"a"(_d_),"d"(_p_))
/**
 * Write a word on one IO port
 * @param d the word to write
 * @param p the IO port number
 */
#define outw(_d_,_p_)   asm volatile ("outw %%ax,  %%dx"::"a"(_d_),"d"(_p_))
/**
 * Write a long word on one IO port
 * @param d the long word to write
 * @param p the IO port number
 */
#define outl(_d_,_p_)   asm volatile ("outl %%eax, %%dx"::"a"(_d_),"d"(_p_))
/**
 * Read a byte on one IO port
 * @param p the IO port number
 */
#define inb(_p_)						\
   ({								\
      uint8_t _d_;						\
      asm volatile ("inb %%dx,%%al":"=a"(_d_):"d"(_p_));	\
      _d_;							\
   })
/**
 * Read a word on one IO port
 * @param p the IO port number
 */
#define inw(_p_)						\
   ({								\
      uint16_t _d_;						\
      asm volatile ("inw %%dx,%%ax":"=a"(_d_):"d"(_p_));	\
      _d_;							\
   })
/**
 * Read a long word on one IO port
 * @param p the IO port number
 */
#define inl(_p_)						\
   ({								\
      uint32_t _d_;						\
      asm volatile("inl %%dx, %%eax":"=a"(_d_):"d"(_p_));	\
      _d_;							\
   })
#define out(_d,_p)  outb(_d,_p)
#define in(_p)      inb(_p)

#endif
