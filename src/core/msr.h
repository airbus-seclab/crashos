#ifndef __MSR_H__
#define __MSR_H__

#include <core/types.h>


/**
** Used for MSR reading and writing
*/
typedef struct raw_msr_entry
{
   uint32_t   eax;
   uint32_t   edx;

} __attribute__((packed)) raw_msr_entry_t;

/**
** Read/Write standard MSR (ecx=index, edx,eax=value)
*/
#define read_msr32(index,val)               asm volatile ( "rdmsr" :"=a"((val)):"c"((index)) )
#define write_msr32(index,val)              asm volatile ( "wrmsr" ::"c"((index)),"a"((val)) )

#define read_msr64(index,h_val,l_val)       asm volatile ( "rdmsr" :"=d"((h_val)),"=a"((l_val)):"c"((index)) )
#define write_msr64(index,h_val,l_val)      asm volatile ( "wrmsr" ::"c"((index)),"d"((h_val)),"a"((l_val)) )

/**
** IA32_APIC_BASE_MSR
*/
typedef struct ia32_apic_base_msr_fields
{
   uint_t    rsrvd0:8;
   uint_t    bsp:1;
   uint_t    rsrvd1:2;
   uint_t    enable:1;
   uint_t    paddr:24;
   uint_t    rsrvd2:28;

} __attribute__((packed)) ia32_apic_base_msr_fields_t;

typedef union ia32_apic_base_msr
{
   raw_msr_entry_t;
   raw64_t;
   ia32_apic_base_msr_fields_t;

} __attribute__((packed)) ia32_apic_base_msr_t;

#define read_msr_ia32_apic_base(val)        read_msr64(0x1b,(val).edx,(val).eax)
#define write_msr_ia32_apic_base(val)       write_msr64(0x1b,(val).edx,(val).eax)

#define apic_set_enable(_x_)					\
   ({								\
      ia32_apic_base_msr_t apic;				\
      read_msr_ia32_apic_base(apic);				\
      apic.enable = (_x_);					\
      write_msr_ia32_apic_base(apic);				\
   })

#define enable_apic()    apic_set_enable(1)
#define disable_apic()   apic_set_enable(0)

/**
** IA32_SYSENTER_CS, IA32_SYSENTER_EIP, IA32_SYSENTER_ESP
*/
#define IA32_SYSENTER_CS_MSR                0x174
#define IA32_SYSENTER_ESP_MSR               0x175
#define IA32_SYSENTER_EIP_MSR               0x176

#define read_msr_ia32_sysenter_cs(val)      read_msr32(IA32_SYSENTER_CS_MSR,val.raw)
#define read_msr_ia32_sysenter_esp(val)     read_msr64(IA32_SYSENTER_ESP_MSR,(val).high,(val).low)
#define read_msr_ia32_sysenter_eip(val)     read_msr64(IA32_SYSENTER_EIP_MSR,(val).high,(val).low)

/**
** IA32_DEBUGCTL_MSR
*/
typedef union ia32_debugctl_msr
{
   raw_msr_entry_t;
   raw64_t;

   struct
   {
      uint_t    lbr:1;    /* last branch int/excp */
      uint_t    btf:1;    /* single step on branch */
      uint_t    r1:4;     /* reserved (0) */
      uint_t    tr:1;     /* trace msg enable */
      uint_t    bts:1;    /* branch trace store */
      uint_t    btint:1;  /* branch trace interrupt */
      uint_t    r2:23;    /* reserved (0) */

   } __attribute__((packed));

} __attribute__((packed)) ia32_debugctl_msr_t;

#define read_msr_ia32_debugctl(val)      read_msr64(0x1d9,(val).edx,(val).eax)


/**
** MSR Feature Control
*/
#define IA32_FEATURE_CONTROL_MSR_LOCK_BIT   0
#define IA32_FEATURE_CONTROL_MSR_VMX_BIT    2

#define IA32_FEATURE_CONTROL_MSR_LOCK       (1<<IA32_FEATURE_CONTROL_MSR_LOCK_BIT)
#define IA32_FEATURE_CONTROL_MSR_VMX        (1<<IA32_FEATURE_CONTROL_MSR_VMX_BIT)

typedef union ia32_feature_control_msr
{
   raw_msr_entry_t;
   raw32_t;

   struct
   {
      uint_t    msr_lock:1;
      uint_t    unk1:1;
      uint_t    msr_vmx:1;
      uint_t    unk2:29;
      
   } __attribute__((packed));

} __attribute__((packed)) feat_ctl_msr_t;

#define read_msr_feature_ctrl(feat)       read_msr32(0x3a,(feat).raw)
#define write_msr_feature_ctrl(feat)      write_msr32(0x3a,(feat).raw)

/**
** PAT register
*/
typedef union pat_register
{
   struct
   {
      uint_t    pa0:3;
      uint_t    r0:5;
      uint_t    pa1:3;
      uint_t    r1:5;
      uint_t    pa2:3;
      uint_t    r2:5;
      uint_t    pa3:3;
      uint_t    r3:5;
      uint_t    pa4:3;
      uint_t    r4:5;
      uint_t    pa5:3;
      uint_t    r5:5;
      uint_t    pa6:3;
      uint_t    r6:5;
      uint_t    pa7:3;
      uint_t    r7:5;

   } __attribute__((packed));

   raw64_t;

} __attribute__((packed)) pat_t;

/**
** Performance Monitoring Counters
*/
#define IA32_PMC_0          0xc1
#define IA32_PERFEVTSEL_0   0x186

typedef union ia32_perfevtsel_msr
{
   struct
   {
      uint8_t  evt;
      uint8_t  umask;
      uint_t   usr:1;
      uint_t   os:1;
      uint_t   e:1;
      uint_t   pc:1;
      uint_t   intr:1;
      uint_t   rsv:1;
      uint_t   en:1;
      uint_t   inv:1;
      uint8_t  cmask;
   };

   raw64_t;

} __attribute__((packed)) ia32_perfsel_t;


#endif
