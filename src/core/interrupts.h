// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <core/types.h>
#include <core/segmentation.h>



/*
** Pmode IDT size
*/
#define IDT_SIZE         256

#define IDT_IRQ_MIN      32
#define IDT_IRQ_MAX      47
#define IDT_VEC_SZ       16

/*
** Gate types 
*/
#define INTERRUPT_GATE_32_TYPE  0xe  /* 1110 */
#define INTERRUPT_GATE_16_TYPE  0x6  /* 0110 */
#define TRAP_GATE_32_TYPE       0xf  /* 1111 */
#define TRAP_GATE_16_TYPE       0x7  /* 0111 */
#define TASK_GATE_TYPE          0x5  /* 0101 */

/**
** Real-mode IVT entry
*/
typedef struct ivt_entry
{
   uint16_t ip;
   uint16_t cs;

} __attribute__((packed)) ivt_entry_t;

/**
** Some BIOS interrupts
*/
#define BIOS_VIDEO_INTERRUPT          0x10
#define BIOS_DISK_INTERRUPT           0x13
#define BIOS_MISC_INTERRUPT           0x15
#define BIOS_KBD_INTERRUPT            0x16
#define BIOS_BOOT_INTERRUPT           0x19

/**
** BIOS services related to MISC_INTERRUPT
*/

/** ax values */
#define BIOS_GET_SMAP                 0xe820
#define BIOS_SMAP_ID                  0x534d4150
#define BIOS_SMAP_MIN_SZ              20
#define BIOS_SMAP_ERROR               0x86

#define BIOS_GET_EXT_MEM_32           0xe881
#define BIOS_GET_EXT_MEM              0xe801

#define BIOS_DISABLE_A20              0x2400
#define BIOS_ENABLE_A20               0x2401
#define BIOS_STATUS_A20               0x2402
#define BIOS_SUPPORT_A20              0x2403

/** ah values */

#define BIOS_GET_BIG_MEM              0x8a
#define BIOS_OLD_GET_EXT_MEM          0x88


/**
** An interrupt descriptor entry:
**
** - interrupt gate (16 or 32)
** - trap gate (16 or 32)
** - task gate
*/
typedef union interrupt_descriptor
{
   struct
   {
      uint16_t  offset_1;    /** bits 00-15 of the isr offset */
      uint16_t  selector;    /** isr segment selector */
      uint_t    reserved:5;  /** intel reserved */
      uint_t    zero_1:3;    /** if not task gate should be 0 */
      uint_t    type:4;      /** gate type */
      uint_t    zero_2:1;    /** must be zero */
      uint_t    dpl:2;       /** desc privilege */
      uint_t    p:1;         /** present flag */
      uint16_t  offset_2;    /** bits 16-31 of the isr offset */
   } __attribute__((packed));

   raw64_t;

} __attribute__((packed,aligned(8))) interrupt_descriptor_t;

/**
** Prepare interrupt gate descriptor in "entry"
*/
#define set_interrupt_gate(entry,isr)			\
({                                                      \
   (entry).offset_1 = (isr)&0xffff;			\
   (entry).offset_2 = ((isr)>>16)&0xffff;		\
   (entry).selector = get_kgdt_sel(1);			\
   (entry).zero_1 = 0;                                  \
   (entry).zero_2 = 0;                                  \
   (entry).type = INTERRUPT_GATE_32_TYPE;               \
   (entry).dpl = 3;                                     \
   (entry).p = 1;                                       \
})

/**
** Interrupt descriptor table register
*/
typedef struct interrupt_descriptor_table_register
{
   uint16_t   limit;      /** idt limit = 8*N-1 */
   uint32_t   base_addr;  /** idt addr */

}__attribute__((packed)) idt_reg_t;

/**
 * Context saved before the interrupt handling
 */
typedef struct context_t {
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t ss;

	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t old_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	uint8_t IT_number;

	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint32_t ss_it;
} context_t;
typedef void __attribute__((regparm(1))) (*isr_t)(context_t*);

/**
 * Interrupt routine format
 */
typedef struct routine_t {
	uint8_t code_array[16];
} routine_t;

/**
 * Initialize the array containing all interrupt and exception handlers
 * Handlers by default; specific handlers only for #DF, #GP and #PF
 */
void init_handler_array();

/**
 * Get value of the CR2 register
 * @param val the pointer containing the value of CR2
 */
#define get_cr2(val)  asm volatile ( "mov %%cr2, %%eax":"=a"(val) )

/**
 * Get value of the IDTR register
 * @param val the pointer containing the value of IDTR
 */
#define get_idtr(aLocation)   asm volatile ( "sidtl %0"::"m"(aLocation):"memory" )
/**
 * Set IDTR to a specific value
 * @param val the value used to update IDTR
 */
#define set_idtr(val)         asm volatile ( "lidt  %0"::"m"(val):"memory" )

#endif
