/* This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md) */
/* Copyright Airbus Group */
OUTPUT_FORMAT("elf32-i386","elf32-i386","elf32-i386");
OUTPUT_ARCH("i386")

ENTRY(entry)

SECTIONS
{
   . = 0x200000;
    __kernel_start__ = .;
   .multiboot . : { *(.multiboot_hdr) . = ALIGN(4); }
   
   .text      . : { *(.text) . = ALIGN(4);          }

   __tests_init_start__ = .;
   .tests_init . : { *(.tests_init)                 }
   __tests_init_end__   = .;
   
   __tests_start__ = .;
   .tests     . : { *(.tests)                       }
   __tests_end__   = .;
   
    __tests_restore_start__ = .;
   .tests_restore . : { *(.tests_restore)           }
   __tests_restore_end__   = .;

   __real_mode_start__ =.;
   .real_mode   . : { *(.real_mode)                 }
   __real_mode_end__ =.;
   
   .rodata      : { *(.rodata)                      }
   .data        : { *(.data)                        }
   .bss         : { *(.bss COMMON)                  }
   /DISCARD/  	: { *(.note* .indent .comment)      }
   __kernel_end__   = .;
}

