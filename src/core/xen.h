// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
** XEN interfaces (from Linux kernel)
*/
#ifndef __XEN_H__
#define __XEN_H__

#include <core/types.h>
#include <core/print.h>

/* Linux include/asm-generic/errno.h */
#define     ENOSYS          38      /* Function not implemented */


#define CONFIG_X86_32

#ifdef CONFIG_X86_32
typedef unsigned long   pteval_t;
typedef unsigned long   pmdval_t;
typedef unsigned long   pudval_t;
typedef unsigned long   pgdval_t;
typedef unsigned long   pgprotval_t;

typedef union {
        pteval_t pte;
        pteval_t pte_low;
} xen_pte_t;
#else
typedef u64     pteval_t;
typedef u64     pmdval_t;
typedef u64     pudval_t;
typedef u64     pgdval_t;
typedef u64     pgprotval_t;

typedef union {
        struct {
                unsigned long pte_low, pte_high;
        };
        pteval_t pte;
} xen_pte_t;
#endif

/*
 * x86_32: EAX = vector; EBX, ECX, EDX, ESI, EDI = args 1, 2, 3, 4, 5.
 *         EAX = return value
 *         (argument registers may be clobbered on return)
 * x86_64: RAX = vector; RDI, RSI, RDX, R10, R8, R9 = args 1, 2, 3, 4, 5, 6.
 *         RAX = return value
 *         (argument registers not clobbered on return; RCX, R11 are)
 */
#define __XEN_set_trap_table        0
#define __XEN_mmu_update            1
#define __XEN_set_gdt               2
#define __XEN_stack_switch          3
#define __XEN_set_callbacks         4
#define __XEN_fpu_taskswitch        5
#define __XEN_sched_op_compat       6
#define __XEN_dom0_op               7
#define __XEN_set_debugreg          8
#define __XEN_get_debugreg          9
#define __XEN_update_descriptor    10
#define __XEN_memory_op            12
#define __XEN_multicall            13
#define __XEN_update_va_mapping    14
#define __XEN_set_timer_op         15
#define __XEN_event_channel_op_compat 16
#define __XEN_xen_version          17
#define __XEN_console_io           18
#define __XEN_physdev_op_compat    19
#define __XEN_grant_table_op       20
#define __XEN_vm_assist            21
#define __XEN_update_va_mapping_otherdomain 22
#define __XEN_iret                 23 /* x86 only */
#define __XEN_vcpu_op              24
#define __XEN_set_segment_base     25 /* x86/64 only */
#define __XEN_mmuext_op            26
#define __XEN_acm_op               27
#define __XEN_nmi_op               28
#define __XEN_sched_op             29
#define __XEN_callback_op          30
#define __XEN_xenoprof_op          31
#define __XEN_event_channel_op     32
#define __XEN_physdev_op           33
#define __XEN_hvm_op               34
#define __XEN_tmem_op              38

/* Architecture-specific hypercall definitions. */
#define __XEN_arch_0               48
#define __XEN_arch_1               49
#define __XEN_arch_2               50
#define __XEN_arch_3               51
#define __XEN_arch_4               52
#define __XEN_arch_5               53
#define __XEN_arch_6               54
#define __XEN_arch_7               55

/*
 * MMU-UPDATE REQUESTS
 *
 * XEN_mmu_update() accepts a list of (ptr, val) pairs.
 * A foreigndom (FD) can be specified (or DOMID_SELF for none).
 * Where the FD has some effect, it is described below.
 * ptr[1:0] specifies the appropriate MMU_* command.
 *
 * ptr[1:0] == MMU_NORMAL_PT_UPDATE:
 * Updates an entry in a page table. If updating an L1 table, and the new
 * table entry is valid/present, the mapped frame must belong to the FD, if
 * an FD has been specified. If attempting to map an I/O page then the
 * caller assumes the privilege of the FD.
 * FD == DOMID_IO: Permit /only/ I/O mappings, at the priv level of the caller.
 * FD == DOMID_XEN: Map restricted areas of Xen's heap space.
 * ptr[:2]  -- Machine address of the page-table entry to modify.
 * val      -- Value to write.
 *
 * ptr[1:0] == MMU_MACHPHYS_UPDATE:
 * Updates an entry in the machine->pseudo-physical mapping table.
 * ptr[:2]  -- Machine address within the frame whose mapping to modify.
 *             The frame must belong to the FD, if one is specified.
 * val      -- Value to write into the mapping entry.
 *
 * ptr[1:0] == MMU_PT_UPDATE_PRESERVE_AD:
 * As MMU_NORMAL_PT_UPDATE above, but A/D bits currently in the PTE are ORed
 * with those in @val.
 */
#define MMU_NORMAL_PT_UPDATE      0 /* checked '*ptr = val'. ptr is MA.       */
#define MMU_MACHPHYS_UPDATE       1 /* ptr = MA of frame to modify entry for  */
#define MMU_PT_UPDATE_PRESERVE_AD 2 /* atomically: *ptr = val | (*ptr&(A|D)) */

/*
 * MMU EXTENDED OPERATIONS
 *
 * XEN_mmuext_op() accepts a list of mmuext_op structures.
 * A foreigndom (FD) can be specified (or DOMID_SELF for none).
 * Where the FD has some effect, it is described below.
 *
 * cmd: MMUEXT_(UN)PIN_*_TABLE
 * mfn: Machine frame number to be (un)pinned as a p.t. page.
 *      The frame must belong to the FD, if one is specified.
 *
 * cmd: MMUEXT_NEW_BASEPTR
 * mfn: Machine frame number of new page-table base to install in MMU.
 *
 * cmd: MMUEXT_NEW_USER_BASEPTR [x86/64 only]
 * mfn: Machine frame number of new page-table base to install in MMU
 *      when in user space.
 *
 * cmd: MMUEXT_TLB_FLUSH_LOCAL
 * No additional arguments. Flushes local TLB.
 *
 * cmd: MMUEXT_INVLPG_LOCAL
 * linear_addr: Linear address to be flushed from the local TLB.
 *
 * cmd: MMUEXT_TLB_FLUSH_MULTI
 * vcpumask: Pointer to bitmap of VCPUs to be flushed.
 *
 * cmd: MMUEXT_INVLPG_MULTI
 * linear_addr: Linear address to be flushed.
 * vcpumask: Pointer to bitmap of VCPUs to be flushed.
 *
 * cmd: MMUEXT_TLB_FLUSH_ALL
 * No additional arguments. Flushes all VCPUs' TLBs.
 *
 * cmd: MMUEXT_INVLPG_ALL
 * linear_addr: Linear address to be flushed from all VCPUs' TLBs.
 *
 * cmd: MMUEXT_FLUSH_CACHE
 * No additional arguments. Writes back and flushes cache contents.
 *
 * cmd: MMUEXT_SET_LDT
 * linear_addr: Linear address of LDT base (NB. must be page-aligned).
 * nr_ents: Number of entries in LDT.
 */
#define MMUEXT_PIN_L1_TABLE      0
#define MMUEXT_PIN_L2_TABLE      1
#define MMUEXT_PIN_L3_TABLE      2
#define MMUEXT_PIN_L4_TABLE      3
#define MMUEXT_UNPIN_TABLE       4
#define MMUEXT_NEW_BASEPTR       5
#define MMUEXT_TLB_FLUSH_LOCAL   6
#define MMUEXT_INVLPG_LOCAL      7
#define MMUEXT_TLB_FLUSH_MULTI   8
#define MMUEXT_INVLPG_MULTI      9
#define MMUEXT_TLB_FLUSH_ALL    10
#define MMUEXT_INVLPG_ALL       11
#define MMUEXT_FLUSH_CACHE      12
#define MMUEXT_SET_LDT          13
#define MMUEXT_NEW_USER_BASEPTR 15

struct mmuext_op {
   unsigned int cmd;
   union {
      /* [UN]PIN_TABLE, NEW_BASEPTR, NEW_USER_BASEPTR */
      unsigned long mfn;
      /* INVLPG_LOCAL, INVLPG_ALL, SET_LDT */
      unsigned long linear_addr;
   } arg1;
   union {
      /* SET_LDT */
      unsigned int nr_ents;
      /* TLB_FLUSH_MULTI, INVLPG_MULTI */
      void *vcpumask;
   } arg2;
};

/* These are passed as 'flags' to update_va_mapping. They can be ORed. */
/* When specifying UVMF_MULTI, also OR in a pointer to a CPU bitmap.   */
/* UVMF_LOCAL is merely UVMF_MULTI with a NULL bitmap pointer.         */
#define UVMF_NONE               (0UL<<0) /* No flushing at all.   */
#define UVMF_TLB_FLUSH          (1UL<<0) /* Flush entire TLB(s).  */
#define UVMF_INVLPG             (2UL<<0) /* Flush only one entry. */
#define UVMF_FLUSHTYPE_MASK     (3UL<<0)
#define UVMF_MULTI              (0UL<<2) /* Flush subset of TLBs. */
#define UVMF_LOCAL              (0UL<<2) /* Flush local TLB.      */
#define UVMF_ALL                (1UL<<2) /* Flush all TLBs.       */

/*
 * Commands to XEN_console_io().
 */
#define CONSOLEIO_write         0
#define CONSOLEIO_read          1

/*
 * Commands to XEN_vm_assist().
 */
#define VMASST_CMD_enable                0
#define VMASST_CMD_disable               1
#define VMASST_TYPE_4gb_segments         0
#define VMASST_TYPE_4gb_segments_notify  1
#define VMASST_TYPE_writable_pagetables  2
#define VMASST_TYPE_pae_extended_cr3     3
#define MAX_VMASST_TYPE 3

typedef uint16_t domid_t;

/* Domain ids >= DOMID_FIRST_RESERVED cannot be used for ordinary domains. */
#define DOMID_FIRST_RESERVED (0x7FF0U)

/* DOMID_SELF is used in certain contexts to refer to oneself. */
#define DOMID_SELF (0x7FF0U)

/*
 * DOMID_IO is used to restrict page-table updates to mapping I/O memory.
 * Although no Foreign Domain need be specified to map I/O pages, DOMID_IO
 * is useful to ensure that no mappings to the OS's own heap are accidentally
 * installed. (e.g., in Linux this could cause havoc as reference counts
 * aren't adjusted on the I/O-mapping code path).
 * This only makes sense in MMUEXT_SET_FOREIGNDOM, but in that context can
 * be specified by any calling domain.
 */
#define DOMID_IO   (0x7FF1U)

/*
 * DOMID_XEN is used to allow privileged domains to map restricted parts of
 * Xen's heap space (e.g., the machine_to_phys table).
 * This only makes sense in MMUEXT_SET_FOREIGNDOM, and is only permitted if
 * the caller is privileged.
 */
#define DOMID_XEN  (0x7FF2U)

/*
 * Send an array of these to XEN_mmu_update().
 * NB. The fields are natural pointer/address size for this architecture.
 */
struct mmu_update {
   uint64_t ptr;       /* Machine address of PTE. */
   uint64_t val;       /* New contents of PTE.    */
};

/*
 * Send an array of these to XEN_multicall().
 * NB. The fields are natural register size for this architecture.
 */
struct multicall_entry {
   unsigned long op;
   long result;
   unsigned long args[6];
};


/*
 * Send an array of these to HYPERVISOR_set_trap_table()
 * The privilege level specifies which modes may enter a trap via a software
 * interrupt. On x86/64, since rings 1 and 2 are unavailable, we allocate
 * privilege levels as follows:
 *  Level == 0: No one may enter
 *  Level == 1: Kernel may enter
 *  Level == 2: Kernel may enter
 *  Level == 3: Everyone may enter
 */
#define TI_GET_DPL(_ti)         ((_ti)->flags & 3)
#define TI_GET_IF(_ti)          ((_ti)->flags & 4)
#define TI_SET_DPL(_ti, _dpl)   ((_ti)->flags |= (_dpl))
#define TI_SET_IF(_ti, _if)     ((_ti)->flags |= ((!!(_if))<<2))

struct trap_info {
    uint8_t       vector;  /* exception vector                              */
    uint8_t       flags;   /* 0-3: privilege level; 4: clear event enable?  */
    uint16_t      cs;      /* code selector                                 */
    unsigned long address; /* code offset                                   */
};

struct arch_shared_info {
    unsigned long max_pfn;                  /* max pfn that appears in table */
    /* Frame containing list of mfns containing list of mfns containing p2m. */
    unsigned long pfn_to_mfn_frame_list_list;
    unsigned long nmi_reason;
};

/*
 * The hypercall asms have to meet several constraints:
 * - Work on 32- and 64-bit.
 *    The two architectures put their arguments in different sets of
 *    registers.
 *
 * - Work around asm syntax quirks
 *    It isn't possible to specify one of the rNN registers in a
 *    constraint, so we use explicit register variables to get the
 *    args into the right place.
 *
 * - Mark all registers as potentially clobbered
 *    Even unused parameters can be clobbered by the hypervisor, so we
 *    need to make sure gcc knows it.
 *
 * - Avoid compiler bugs.
 *    This is the tricky part.  Because x86_32 has such a constrained
 *    register set, gcc versions below 4.3 have trouble generating
 *    code when all the arg registers and memory are trashed by the
 *    asm.  There are syntactically simpler ways of achieving the
 *    semantics below, but they cause the compiler to crash.
 *
 *    The only combination I found which works is:
 *     - assign the __argX variables first
 *     - list all actually used parameters as "+r" (__argX)
 *     - clobber the rest
 *
 * The result certainly isn't pretty, and it really shows up cpp's
 * weakness as as macro language.  Sorry.  (But let's just give thanks
 * there aren't more than 5 arguments...)
 */

extern struct { char _entry[32]; } hypercall_page[];

#define __HYPERCALL             "call hypercall_page+%c[offset]"
#define __HYPERCALL_ENTRY(x)                                    \
   [offset] "i" (__XEN_##x * sizeof(hypercall_page[0]))

#ifdef CONFIG_X86_32
#define __HYPERCALL_RETREG      "eax"
#define __HYPERCALL_ARG1REG     "ebx"
#define __HYPERCALL_ARG2REG     "ecx"
#define __HYPERCALL_ARG3REG     "edx"
#define __HYPERCALL_ARG4REG     "esi"
#define __HYPERCALL_ARG5REG     "edi"
#else
#define __HYPERCALL_RETREG      "rax"
#define __HYPERCALL_ARG1REG     "rdi"
#define __HYPERCALL_ARG2REG     "rsi"
#define __HYPERCALL_ARG3REG     "rdx"
#define __HYPERCALL_ARG4REG     "r10"
#define __HYPERCALL_ARG5REG     "r8"
#endif

#define __HYPERCALL_DECLS                                               \
   register unsigned long __res  asm(__HYPERCALL_RETREG);               \
   register unsigned long __arg1 asm(__HYPERCALL_ARG1REG) = __arg1;     \
   register unsigned long __arg2 asm(__HYPERCALL_ARG2REG) = __arg2;     \
   register unsigned long __arg3 asm(__HYPERCALL_ARG3REG) = __arg3;     \
   register unsigned long __arg4 asm(__HYPERCALL_ARG4REG) = __arg4;     \
   register unsigned long __arg5 asm(__HYPERCALL_ARG5REG) = __arg5;

#define __HYPERCALL_0PARAM      "=r" (__res)
#define __HYPERCALL_1PARAM      __HYPERCALL_0PARAM, "+r" (__arg1)
#define __HYPERCALL_2PARAM      __HYPERCALL_1PARAM, "+r" (__arg2)
#define __HYPERCALL_3PARAM      __HYPERCALL_2PARAM, "+r" (__arg3)
#define __HYPERCALL_4PARAM      __HYPERCALL_3PARAM, "+r" (__arg4)
#define __HYPERCALL_5PARAM      __HYPERCALL_4PARAM, "+r" (__arg5)

#define __HYPERCALL_0ARG()
#define __HYPERCALL_1ARG(a1)                                            \
   __HYPERCALL_0ARG()              __arg1 = (unsigned long)(a1);
#define __HYPERCALL_2ARG(a1,a2)                                         \
   __HYPERCALL_1ARG(a1)            __arg2 = (unsigned long)(a2);
#define __HYPERCALL_3ARG(a1,a2,a3)                                      \
   __HYPERCALL_2ARG(a1,a2)         __arg3 = (unsigned long)(a3);
#define __HYPERCALL_4ARG(a1,a2,a3,a4)                                   \
   __HYPERCALL_3ARG(a1,a2,a3)      __arg4 = (unsigned long)(a4);
#define __HYPERCALL_5ARG(a1,a2,a3,a4,a5)                                \
   __HYPERCALL_4ARG(a1,a2,a3,a4)   __arg5 = (unsigned long)(a5);

#define __HYPERCALL_CLOBBER5    "memory"
#define __HYPERCALL_CLOBBER4    __HYPERCALL_CLOBBER5, __HYPERCALL_ARG5REG
#define __HYPERCALL_CLOBBER3    __HYPERCALL_CLOBBER4, __HYPERCALL_ARG4REG
#define __HYPERCALL_CLOBBER2    __HYPERCALL_CLOBBER3, __HYPERCALL_ARG3REG
#define __HYPERCALL_CLOBBER1    __HYPERCALL_CLOBBER2, __HYPERCALL_ARG2REG
#define __HYPERCALL_CLOBBER0    __HYPERCALL_CLOBBER1, __HYPERCALL_ARG1REG

#define _hypercall0(type, name)                                         \
   ({                                                                   \
      __HYPERCALL_DECLS;                                                \
      __HYPERCALL_0ARG();                                               \
      asm volatile (__HYPERCALL                                         \
                    : __HYPERCALL_0PARAM                                \
                    : __HYPERCALL_ENTRY(name)                           \
                    : __HYPERCALL_CLOBBER0);                            \
      (type)__res;                                                      \
   })

#define _hypercall1(type, name, a1)                                     \
   ({                                                                   \
      __HYPERCALL_DECLS;                                                \
      __HYPERCALL_1ARG(a1);                                             \
      asm volatile (__HYPERCALL                                         \
                    : __HYPERCALL_1PARAM                                \
                    : __HYPERCALL_ENTRY(name)                           \
                    : __HYPERCALL_CLOBBER1);                            \
      (type)__res;                                                      \
   })

#define _hypercall2(type, name, a1, a2)                                 \
   ({                                                                   \
      __HYPERCALL_DECLS;                                                \
      __HYPERCALL_2ARG(a1, a2);                                         \
      asm volatile (__HYPERCALL                                         \
                    : __HYPERCALL_2PARAM                                \
                    : __HYPERCALL_ENTRY(name)                           \
                    : __HYPERCALL_CLOBBER2);                            \
      (type)__res;                                                      \
   })

#define _hypercall3(type, name, a1, a2, a3)     \
   ({                                           \
      __HYPERCALL_DECLS;                        \
      __HYPERCALL_3ARG(a1, a2, a3);             \
      asm volatile (__HYPERCALL                 \
                    : __HYPERCALL_3PARAM        \
                    : __HYPERCALL_ENTRY(name)   \
                    : __HYPERCALL_CLOBBER3);    \
      (type)__res;                              \
   })

#define _hypercall4(type, name, a1, a2, a3, a4) \
   ({                                           \
      __HYPERCALL_DECLS;                        \
      __HYPERCALL_4ARG(a1, a2, a3, a4);         \
      asm volatile (__HYPERCALL                 \
                    : __HYPERCALL_4PARAM        \
                    : __HYPERCALL_ENTRY(name)   \
                    : __HYPERCALL_CLOBBER4);    \
      (type)__res;                              \
   })

#define _hypercall5(type, name, a1, a2, a3, a4, a5)     \
   ({                                                   \
      __HYPERCALL_DECLS;                                \
      __HYPERCALL_5ARG(a1, a2, a3, a4, a5);             \
      asm volatile (__HYPERCALL                         \
                    : __HYPERCALL_5PARAM                \
                    : __HYPERCALL_ENTRY(name)           \
                    : __HYPERCALL_CLOBBER5);            \
      (type)__res;                                      \
   })

static inline long
privcmd_call(unsigned call,
             unsigned long a1, unsigned long a2,
             unsigned long a3, unsigned long a4,
             unsigned long a5)
{
   __HYPERCALL_DECLS;
   __HYPERCALL_5ARG(a1, a2, a3, a4, a5);

   asm volatile("call *%[call]"
                : __HYPERCALL_5PARAM
                : [call] "a" (&hypercall_page[call])
                : __HYPERCALL_CLOBBER5);

   return (long)__res;
}

static inline int
XEN_set_trap_table(struct trap_info *table)
{
   return _hypercall1(int, set_trap_table, table);
}

static inline int
XEN_mmu_update(struct mmu_update *req, int count,
               int *success_count, domid_t domid)
{
   return _hypercall4(int, mmu_update, req, count, success_count, domid);
}

static inline int
XEN_mmuext_op(struct mmuext_op *op, int count,
              int *success_count, domid_t domid)
{
   return _hypercall4(int, mmuext_op, op, count, success_count, domid);
}

static inline int
XEN_set_gdt(unsigned long *frame_list, int entries)
{
   printf_log("plop Xen_set_gdt\n");
   return _hypercall2(int, set_gdt, frame_list, entries);
}

static inline int
XEN_stack_switch(unsigned long ss, unsigned long esp)
{
   return _hypercall2(int, stack_switch, ss, esp);
}

#ifdef CONFIG_X86_32
static inline int
XEN_set_callbacks(unsigned long event_selector,
                  unsigned long event_address,
                  unsigned long failsafe_selector,
                  unsigned long failsafe_address)
{
   return _hypercall4(int, set_callbacks,
                      event_selector, event_address,
                      failsafe_selector, failsafe_address);
}
#else  /* CONFIG_X86_64 */
static inline int
XEN_set_callbacks(unsigned long event_address,
                  unsigned long failsafe_address,
                  unsigned long syscall_address)
{
   return _hypercall3(int, set_callbacks,
                      event_address, failsafe_address,
                      syscall_address);
}
#endif  /* CONFIG_X86_{32,64} */

static inline int
XEN_callback_op(int cmd, void *arg)
{
   return _hypercall2(int, callback_op, cmd, arg);
}

static inline int
XEN_fpu_taskswitch(int set)
{
   return _hypercall1(int, fpu_taskswitch, set);
}

static inline int
XEN_sched_op(int cmd, void *arg)
{
   return _hypercall2(int, sched_op, cmd, arg);
}

static inline long
XEN_set_timer_op(uint64_t timeout)
{
   unsigned long timeout_hi = (unsigned long)(timeout>>32);
   unsigned long timeout_lo = (unsigned long)timeout;
   return _hypercall2(long, set_timer_op, timeout_lo, timeout_hi);
}

static inline int
XEN_set_debugreg(int reg, unsigned long value)
{
   return _hypercall2(int, set_debugreg, reg, value);
}

static inline unsigned long
XEN_get_debugreg(int reg)
{
   return _hypercall1(unsigned long, get_debugreg, reg);
}

static inline int
XEN_update_descriptor(unsigned long ma, unsigned long desc)
{
#ifdef CONFIG_X86_32
      return _hypercall2(int, update_descriptor, ma, desc);
#else
   return _hypercall4(int, update_descriptor, ma, ma>>32, desc, desc>>32);
#endif
}

static inline int
XEN_memory_op(unsigned int cmd, void *arg)
{
   return _hypercall2(int, memory_op, cmd, arg);
}

static inline int
XEN_multicall(void *call_list, int nr_calls)
{
   return _hypercall2(int, multicall, call_list, nr_calls);
}

static inline int
XEN_update_va_mapping(unsigned long va, xen_pte_t new_val,
                      unsigned long flags)
{
#ifdef CONFIG_X86_32
      return _hypercall3(int, update_va_mapping, va,
                         new_val.pte, flags);
#else
      return _hypercall4(int, update_va_mapping, va,
                         new_val.pte, new_val.pte >> 32, flags);
#endif
}

static inline int
XEN_event_channel_op(int cmd, void *arg)
{
   return _hypercall2(int, event_channel_op, cmd, arg);
}

static inline int
XEN_xen_version(int cmd, void *arg)
{
   return _hypercall2(int, xen_version, cmd, arg);
}

static inline int
XEN_console_io(int cmd, int count, char *str)
{
   return _hypercall3(int, console_io, cmd, count, str);
}

static inline int
XEN_physdev_op(int cmd, void *arg)
{
   return _hypercall2(int, physdev_op, cmd, arg);
}

static inline int
XEN_grant_table_op(unsigned int cmd, void *uop, unsigned int count)
{
   return _hypercall3(int, grant_table_op, cmd, uop, count);
}

static inline int
XEN_update_va_mapping_otherdomain(unsigned long va, xen_pte_t new_val,
                                  unsigned long flags, domid_t domid)
{
#ifdef CONFIG_X86_32
      return _hypercall4(int, update_va_mapping_otherdomain, va,
                         new_val.pte, flags, domid);
#else
      return _hypercall5(int, update_va_mapping_otherdomain, va,
                         new_val.pte, new_val.pte >> 32,
                         flags, domid);
#endif
}

static inline int
XEN_vm_assist(unsigned int cmd, unsigned int type)
{
   return _hypercall2(int, vm_assist, cmd, type);
}

static inline int
XEN_vcpu_op(int cmd, int vcpuid, void *extra_args)
{
   return _hypercall3(int, vcpu_op, cmd, vcpuid, extra_args);
}

#ifdef CONFIG_X86_64
static inline int
XEN_set_segment_base(int reg, unsigned long value)
{
   return _hypercall2(int, set_segment_base, reg, value);
}
#endif

static inline int
XEN_nmi_op(unsigned long op, unsigned long arg)
{
   return _hypercall2(int, nmi_op, op, arg);
}

static inline unsigned long
XEN_hvm_op(int op, void *arg)
{
   return _hypercall2(unsigned long, hvm_op, op, arg);
}

static inline int
XEN_tmem_op(void *op)
{
   return _hypercall1(int, tmem_op, op);
}

static inline void
MULTI_fpu_taskswitch(struct multicall_entry *mcl, int set)
{
   mcl->op = __XEN_fpu_taskswitch;
   mcl->args[0] = set;
}

static inline void
MULTI_update_va_mapping(struct multicall_entry *mcl, unsigned long va,
                        xen_pte_t new_val, unsigned long flags)
{
   mcl->op = __XEN_update_va_mapping;
   mcl->args[0] = va;
#ifdef CONFIG_X86_32
   mcl->args[1] = new_val.pte;
   mcl->args[2] = flags;
#else
   mcl->args[1] = new_val.pte;
   mcl->args[2] = new_val.pte >> 32;
   mcl->args[3] = flags;
#endif
}

static inline void
MULTI_grant_table_op(struct multicall_entry *mcl, unsigned int cmd,
                     void *uop, unsigned int count)
{
   mcl->op = __XEN_grant_table_op;
   mcl->args[0] = cmd;
   mcl->args[1] = (unsigned long)uop;
   mcl->args[2] = count;
}

static inline void
MULTI_update_va_mapping_otherdomain(struct multicall_entry *mcl, unsigned long va,
                                    xen_pte_t new_val, unsigned long flags,
                                    domid_t domid)
{
   mcl->op = __XEN_update_va_mapping_otherdomain;
   mcl->args[0] = va;
#ifdef CONFIG_X86_32
   mcl->args[1] = new_val.pte;
   mcl->args[2] = flags;
   mcl->args[3] = domid;
#else
   mcl->args[1] = new_val.pte;
   mcl->args[2] = new_val.pte >> 32;
   mcl->args[3] = flags;
   mcl->args[4] = domid;
#endif
}

static inline void
MULTI_update_descriptor(struct multicall_entry *mcl, unsigned long maddr,
                        unsigned long desc)
{
   mcl->op = __XEN_update_descriptor;
#ifdef CONFIG_X86_32
   mcl->args[0] = maddr;
   mcl->args[1] = desc;
#else
   mcl->args[0] = maddr;
   mcl->args[1] = maddr >> 32;
   mcl->args[2] = desc;
   mcl->args[3] = desc >> 32;
#endif
}

static inline void
MULTI_memory_op(struct multicall_entry *mcl, unsigned int cmd, void *arg)
{
   mcl->op = __XEN_memory_op;
   mcl->args[0] = cmd;
   mcl->args[1] = (unsigned long)arg;
}

static inline void
MULTI_mmu_update(struct multicall_entry *mcl, struct mmu_update *req,
                 int count, int *success_count, domid_t domid)
{
   mcl->op = __XEN_mmu_update;
   mcl->args[0] = (unsigned long)req;
   mcl->args[1] = count;
   mcl->args[2] = (unsigned long)success_count;
   mcl->args[3] = domid;
}

static inline void
MULTI_mmuext_op(struct multicall_entry *mcl, struct mmuext_op *op, int count,
                int *success_count, domid_t domid)
{
   mcl->op = __XEN_mmuext_op;
   mcl->args[0] = (unsigned long)op;
   mcl->args[1] = count;
   mcl->args[2] = (unsigned long)success_count;
   mcl->args[3] = domid;
}

static inline void
MULTI_set_gdt(struct multicall_entry *mcl, unsigned long *frames, int entries)
{
   mcl->op = __XEN_set_gdt;
   mcl->args[0] = (unsigned long)frames;
   mcl->args[1] = entries;
}

static inline void
MULTI_stack_switch(struct multicall_entry *mcl,
                   unsigned long ss, unsigned long esp)
{
   mcl->op = __XEN_stack_switch;
   mcl->args[0] = ss;
   mcl->args[1] = esp;
}

/*
** Functions
*/

/**
 * Init XEN hypercall page
 */
int xen_setup();


#endif
