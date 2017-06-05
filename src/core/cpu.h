#ifndef __CPU_H__
#define __CPU_H__

#include <core/types.h>

#define get_esp(val) asm volatile ( "mov %%esp, %%eax":"=a"(val) )
#define get_ebp(val) asm volatile ( "mov %%ebp, %%eax":"=a"(val) )


/**
 * x86 CPUID() wrapper
 * @param index
 * @param pointers to uint32_t to receive eax,ebx,ecx,edx
 */
void cpuid(uint32_t index,
           uint32_t *eax, uint32_t *ebx,
           uint32_t *ecx, uint32_t *edx);

/**
 * Write MSR 64 bits value
 * @param _i MSR index
 * @param _h Upper 32 bits value
 * @param _l Lower 32 bits value
 */
#define wrmsr64(_i,_h,_l)                                               \
   ({                                                                   \
      asm volatile ("wrmsr"                                             \
                    ::"d"((_h)), "a"((_l)), "c"((_i)));                 \
   })

#endif
