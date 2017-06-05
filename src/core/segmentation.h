#ifndef __SEGMEM_H__
#define __SEGMEM_H__

#include <core/types.h>

#define GDT_TYPE 0
#define LDT_TYPE 1

#define K_PVL 0
#define U_PVL 3

#define farjump(_fptr) ({asm volatile ("ljmp  *%0"::"m"(_fptr):"memory")})

typedef union segment_selector
{
   struct
   {
      uint_t         rpl:2;         /** bits 0-1  requested privilege level */
      uint_t         ti:1;          /** bit  2    table indicator gdt(0) or ldt(1) */
      uint_t         index:13;      /** bits 3-15 index in descriptor table */
   };

   uint16_t raw;

}__attribute__((packed)) segment_selector_t;

typedef union segment_descriptor
{
   struct
   {
      uint16_t       limit_1;              /** bits 00-15 of the segment limit */
      uint16_t       base_1;               /** bits 00-15 of the base address */
      uint8_t        base_2;               /** bits 16-23 of the base address */
      uint_t         type:4;               /** segment type */
      uint_t         s:1;                  /** descriptor type */
      uint_t         dpl:2;                /** descriptor privilege level */
      uint_t         p:1;                  /** segment present flag */
      uint_t         limit_2:4;            /** bits 16-19 of the segment limit */
      uint_t         avl:1;                /** available for fun and profit */
      uint_t         reserved:1;           /** reserved bit */
      uint_t         db:1;                 /** default length, depend on seg type */
      uint_t         gran:1;               /** granularity */
      uint8_t        base_3;               /** bits 24-31 of the base address */
   };

   uint64_t          raw;

}__attribute__((packed,aligned(8))) segment_descriptor_t;

typedef struct global_descriptor_table_register
{
   uint16_t       limit;                 /** gdt limit = size - 1 */
   uint32_t       base_addr;             /** gdt addr */

}__attribute__((packed)) gdt_reg_t;

typedef struct local_descriptor_table_register
{
   segment_selector_t  sel;            /** segment selector */
   uint16_t            limit;          /** segment limit */
   uint32_t            base_addr;      /** ldt addr */

}__attribute((packed)) ldt_reg_t;

typedef struct task_register
{
   uint16_t              limit;
   uint32_t              base_addr;
   segment_selector_t    selector;

}__attribute__((packed)) task_reg_t;

#define set_tr(val)         asm volatile ( "ltr   %%ax"::"a"(val) )
#define get_tr(aLocation)   asm volatile ( "str   %0"::"m"(aLocation):"memory" )

#define set_gdtr(val)       asm volatile ( "lgdt  %0"::"m"(val):"memory" )
#define get_gdtr(aLocation) asm volatile ( "sgdtl %0"::"m"(aLocation):"memory" )

#define get_ds(sel)         asm volatile ( "movw %%ds, %%ax":"=a"(sel) )
#define get_es(sel)         asm volatile ( "movw %%es, %%ax":"=a"(sel) )
#define get_ss(sel)         asm volatile ( "movw %%ss, %%ax":"=a"(sel) )
#define get_cs(sel)         asm volatile ( "movw %%cs, %%ax":"=a"(sel) )

#define set_ds(sel)         asm volatile ( "movw %%ax, %%ds"::"a"(sel) )
#define set_es(sel)         asm volatile ( "movw %%ax, %%es"::"a"(sel) )
#define set_fs(sel)         asm volatile ( "movw %%ax, %%fs"::"a"(sel) )
#define set_gs(sel)         asm volatile ( "movw %%ax, %%gs"::"a"(sel) )
#define set_ss(sel)         asm volatile ( "movw %%ax, %%ss"::"a"(sel) )
#define set_cs_i(sel)       asm volatile ( "ljmp %0, $1f ; 1:"::"i"(sel) )

#define set_cs_m(sel)				\
   ({						\
      fptr32_t loc;				\
      loc.segment = sel;			\
      asm volatile (				\
	 "movl $1f, %0 ; ljmp *%1 ; 1:"		\
	 ::"m"(loc.offset),"m"(loc):"memory" );	\
   })

typedef struct task_state_segment
{
   uint16_t       back_link;             /** previous TSS */
   uint16_t       r1;                    /** reserved, 0 */

   uint32_t       esp0;                  /** stack pointer for pvl 0 */
   uint16_t       ss0;                   /** stack segment for pvl 0 */
   uint16_t       r2;                    /** reserved, 0 */

   uint32_t       esp1;                  /** stack pointer for pvl 1 */
   uint16_t       ss1;                   /** stack segment for pvl 1 */
   uint16_t       r3;                    /** reserved, 0 */

   uint32_t       esp2;                  /** stack pointer for pvl 2 */
   uint16_t       ss2;                   /** stack segment for pvl 2 */
   uint16_t       r4;                    /** reserved, 0 */

   uint32_t       cr3;                   /** pdbr of the task */
   uint32_t       eip;                   /** entry point */
   uint32_t       eflags;                /** processor state flags */

   uint32_t       eax;                   /** general registers */
   uint32_t       ecx;
   uint32_t       edx;
   uint32_t       ebx;
   uint32_t       esp;
   uint32_t       ebp;
   uint32_t       esi;
   uint32_t       edi;

   uint16_t       es;
   uint16_t       r5;                    /** reserved, 0 */

   uint16_t       cs;
   uint16_t       r6;                    /** reserved, 0 */

   uint16_t       ss;
   uint16_t       r7;                    /** reserved, 0 */

   uint16_t       ds;
   uint16_t       r8;                    /** reserved, 0 */

   uint16_t       fs;
   uint16_t       r9;                    /** reserved, 0 */

   uint16_t       gs;
   uint16_t       r10;                   /** reserved, 0 */

   uint16_t       ldt;                   /** selector of the LDT of the task */
   uint16_t       r11;                   /** reserved, 0 */

   uint_t         t:1;                   /** raise debug exception if 1 when enters the task */
   uint16_t       r12:15;                /** reserved, 0 */

   uint16_t       io_map_addr;           /** offset from TSS of the I/O bitmap */

   uint8_t        int_bitmap[32];        /** Software interrupt redirection bitmap */
   uint8_t        io_bitmap[8192];       /** I/O bitmap */

   uint8_t        boundary;              /** must be set to 0xff */

}__attribute__((packed)) task_state_segment_t;


#define tss_allow(_tss_,_map_,_idx_)      ((_tss_)->_map_[(_idx_)/8] &= ~(1<<((_idx_)%8)))
#define tss_deny(_tss_,_map_,_idx_)       ((_tss_)->_map_[(_idx_)/8] |=  (1<<((_idx_)%8)))
#define tss_is_denied(_tss_,_map_,_idx_)  (((_tss_)->_map_[(_idx_)/8]&(1<<((_idx_)%8)))?1:0)

#define tss_allow_int(_tss_,_idx_)        tss_allow(_tss_,int_bitmap,_idx_)
#define tss_deny_int(_tss_,_idx_)         tss_deny(_tss_,int_bitmap,_idx_)
#define tss_is_denied_int(_tss_,_idx_)    tss_is_denied(_tss_,int_bitmap,_idx_)

#define tss_allow_io(_tss_,_idx_)         tss_allow(_tss_,io_bitmap,_idx_)
#define tss_deny_io(_tss_,_idx_)          tss_deny(_tss_,io_bitmap,_idx_)
#define tss_is_denied_io(_tss_,_idx_)     tss_is_denied_io(_tss_,io_bitmap,_idx_)

/*
** Builders
*/

/**
** 16 bits Segment selector
** bits 0-1  requested privilege level
** bit  2    gdt or ldt
** bits 3-15 index in descriptor table
**
** CS must be set with a ljmp
** ljmp takes immediate value as 1st operand
** inline-asm "i" constraint needs a number
** so we use a macro to set selectors values
*/
#define get_selector(index,ti,rpl)   (((index)<<3)|((ti)<<2)|((rpl)))

#define get_gdt_sel(index,pvl)       get_selector(index,GDT_TYPE,pvl)
#define get_ldt_sel(index,pvl)       get_selector(index,LDT_TYPE,pvl)

  #define get_kgdt_sel(index)          get_gdt_sel(index,K_PVL)
#define get_ugdt_sel(index)          get_gdt_sel(index,U_PVL)

/**
 * Prepare an empty segment descriptor
 */
#define set_null_desc(entry)	       \
   ({				       \
      *((uint32_t*)(&(entry)))   = 0;  \
      *((uint32_t*)(&(entry))+1) = 0;  \
   })

/**
 * Prepare code descriptor in "entry" with privilege "pvl"
 *
 * non-conforming
 * 32 bits addressing
 * non system
 * 4GB
 */
#define set_code_desc(entry,pvl)                                           \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
        (entry).base_1 = 0;             /* CODE goes from 0 to 4GB */      \
        (entry).base_2 = 0;                                                \
        (entry).base_3 = 0;                                                \
        (entry).limit_1 = 0xFFFF;       /* 2^20*gran = 4GB */              \
        (entry).limit_2 = 0xF;                                             \
})

/**
 * Prepare code descriptor in "entry" with privilege "pvl"
 *
 * conforming
 * 32 bits addressing
 * non system
 * 4GB
 */
#define set_code_desc_conforming(entry,pvl)                                \
({      (entry).type = 12;              /* 32-bit call gate */             \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
        (entry).base_1 = 0;             /* CODE goes from 0 to 4GB */      \
        (entry).base_2 = 0;                                                \
        (entry).base_3 = 0;                                                \
        (entry).limit_1 = 0xFFFF;       /* 2^20*gran = 4GB */              \
        (entry).limit_2 = 0xF;                                             \
})

/**
 * Prepare data descriptor in "entry" with privilege "pvl"
 *
 * In flat mode the stack can be anywhere in linear addr space
 * so it is in data :)
 */
#define set_data_desc(entry,pvl)                                           \
({      (entry).type = 3;               /* R/W/A */                        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits stack pointer */        \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
        (entry).base_1 = 0;             /* DATA goes from 0 to 4GB */      \
        (entry).base_2 = 0;                                                \
        (entry).base_3 = 0;                                                \
        (entry).limit_1 = 0xFFFF;                                          \
        (entry).limit_2 = 0xF;                                             \
})

/**
 * Prepare code descriptor in "entry" with privilege "pvl"
 * with base addr "addr"
 * non-conforming
 * 32 bits addressing
 * non system
 * 4GB
 */
#define set_code_desc_addr(entry,pvl,addr)                                 \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                          \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                      \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                      \
        (entry).limit_1 = 0xFFFF;       /* 2^20*gran = 4GB */              \
        (entry).limit_2 = 0xF;                                             \
})

/**
 * Prepare data descriptor in "entry" with privilege "pvl"
 * with base addr "addr"
 * In flat mode the stack can be anywhere in linear addr space
 * so it is in data :)
 */
#define set_data_desc_addr(entry,pvl,addr)                                 \
({      (entry).type = 3;               /* R/W/A */                        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits stack pointer */        \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                          \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                      \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                      \
        (entry).limit_1 = 0xFFFF;                                          \
        (entry).limit_2 = 0xF;                                             \
})


/**
 * Prepare a TSS descriptor in "entry" for the TSS at "addr"
 *
 * type: 1001b = 9d  not busy
 * type: 1011b = 11d busy
 */
#define set_tss_desc(entry,addr)                                                                        \
({      (entry).db = 0;                                                                                 \
        (entry).p = 1;                                                                                  \
        (entry).reserved = 0;                                                                           \
        (entry).gran = 0;                                 /* 1 byte increment for limit */              \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;       /* 16 low bits of 32 bits linear addr */      \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;   /* 8 middle bits of 32 bits linear addr */    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;   /* 8 high bits of 32 bits linear addr */      \
        (entry).dpl = 0;                                  /* kernel only does task switch */            \
        (entry).type = 9;                                 /* not busy */                                \
        (entry).s = 0;                                    /* system descriptor */                       \
        (entry).limit_1 = sizeof(task_state_segment_t);   /* TSS size */                                \
        (entry).limit_2 = 0;                                                                            \
})

#define set_tss_desc_r3(entry,addr)                                                                     \
({      (entry).db = 0;                                                                                 \
        (entry).p = 1;                                                                                  \
        (entry).reserved = 0;                                                                           \
        (entry).gran = 0;                                 /* 1 byte increment for limit */              \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;       /* 16 low bits of 32 bits linear addr */      \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;   /* 8 middle bits of 32 bits linear addr */    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;   /* 8 high bits of 32 bits linear addr */      \
        (entry).dpl = 3;                                  /* kernel only does task switch */            \
        (entry).type = 9;                                 /* not busy */                                \
        (entry).s = 0;                                    /* system descriptor */                       \
        (entry).limit_1 = sizeof(task_state_segment_t);   /* TSS size */                                \
        (entry).limit_2 = 0;                                                                            \
})

/******************************************************** AG ********************************************************************/
#define set_code_desc_1MB(entry,pvl,addr)                                  \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 0;               							       \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                 	   \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = 0xFFFF;                                          \
        (entry).limit_2 = 0xF;                                             \
})

#define set_code_desc_16(entry,pvl,addr)                                   \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 0;                 /* 16 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 0;               							       \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                 	   \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = 0xFFFF;                                          \
        (entry).limit_2 = 0xF;                                             \
})

#define set_code_desc_16_4GB(entry,pvl,addr)                               \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 0;                 /* 16 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               							       \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                 	   \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = 0xFFFF;                                          \
        (entry).limit_2 = 0xF;                                             \
})

#define set_data_desc_16(entry,pvl,addr)                                   \
({      (entry).type = 3;                                                  \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 0;                 /* 16 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 0;               							       \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                 	   \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = 0xFFFF;        /*limit : 1MB*/                   \
        (entry).limit_2 = 0x0;                                             \
})

#define set_tss_desc_16(entry,addr)                                                                     \
({      (entry).db = 0;                                                                                 \
        (entry).p = 1;                                                                                  \
        (entry).reserved = 0;                                                                           \
        (entry).gran = 0;                                 /* 1 byte increment for limit */              \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;       /* 16 low bits of 32 bits linear addr */      \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;   /* 8 middle bits of 32 bits linear addr */    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;   /* 8 high bits of 32 bits linear addr */      \
        (entry).dpl = 0;                                  /* kernel only does task switch */            \
        (entry).type = 9;                                 /* not busy */                                \
        (entry).s = 0;                                    /* system descriptor */                       \
        (entry).limit_1 = 0xffff;                                                                       \
        (entry).limit_2 = 0x0;                                                                          \
})

#define set_code_desc_addr(entry,pvl,addr)                                 \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
		(entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
		(entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
		(entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = 0xFFFF;       /* 2^20*gran = 4GB */              \
        (entry).limit_2 = 0xF;                                             \
})

#define set_code_desc_addr_limit(entry,pvl,addr, limit)                    \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
		(entry).dpl = pvl;              /* privilege level */              \
		(entry).db = 1;                 /* 32 bits addresses */            \
		(entry).s = 1;                  /* non system descriptor */        \
		(entry).p = 1;                  /* segment is present in mem */    \
		(entry).reserved = 0;                                              \
		(entry).gran = 1;               /* limit unit is 4KB */            \
		(entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
		(entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
		(entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
		(entry).limit_1 = limit & 0xFFFF;                                  \
		(entry).limit_2 = (limit>>13) & 0xF;                               \
})

#define set_data_desc_addr_limit(entry,pvl,addr, limit)                    \
({      (entry).type = 3;               /* R/W/A */                        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 1;                 /* 32 bits stack pointer */        \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 1;               /* limit unit is 4KB */            \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = limit & 0xFFFF;                                  \
        (entry).limit_2 = (limit>>13) & 0xF;                               \
})

#define set_desc_addr_type_limit(entry, t, pvl, addr, limit)               \
({      (entry).type = t;                                                  \
		(entry).dpl = pvl;              /* privilege level */              \
		(entry).db = 1;                 /* 32 bits addresses */            \
		(entry).s = 1;                  /* non system descriptor */        \
		(entry).p = 1;                  /* segment is present in mem */    \
		(entry).reserved = 0;                                              \
		(entry).gran = 1;               /* limit unit is 4KB */            \
		(entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
		(entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
		(entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
		(entry).limit_1 = limit & 0xFFFF;                                  \
		(entry).limit_2 = (limit>>13) & 0xF;                               \
})

#define set_desc_addr_type_limit_1B(entry, t, pvl, addr, limit)            \
({      (entry).type = t;               				                   \
		(entry).dpl = pvl;              /* privilege level */              \
		(entry).db = 1;                 /* 32 bits addresses */            \
		(entry).s = 1;                  /* non system descriptor */        \
		(entry).p = 1;                  /* segment is present in mem */    \
		(entry).reserved = 0;                                              \
		(entry).gran = 0;               /* limit unit is 1B */             \
		(entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
		(entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
		(entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = ((uint32_t)(limit))&0xFFFF;                      \
        (entry).limit_2 = (((uint32_t)(limit))>>16)&0xF;                   \
})
/********************************************************************************************************************************/
/**
 * Prepare 16 bits code descriptor in "entry" with privilege "pvl"
 * with base addr "addr" and "limit" limit
 */
#define set_code16_desc_addr_limit(entry,pvl,addr,limit)                   \
({      (entry).type = 11;              /* Exe/A, non-conforming */        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 0;                 /* 32 bits addresses */            \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 0;               /* limit unit is 1B */             \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = ((uint32_t)(limit))&0xFFFF;                      \
        (entry).limit_2 = (((uint32_t)(limit))>>16)&0xF;                   \
})


/**
 * Prepare 16 bits data descriptor in "entry" with privilege "pvl"
 * with base addr "addr" and "limit" limit
 */
#define set_data16_desc_addr_limit(entry,pvl,addr,limit)                   \
({      (entry).type = 3;               /* R/W/A */                        \
        (entry).dpl = pvl;              /* privilege level */              \
        (entry).db = 0;                 /* 16 bits segment */              \
        (entry).s = 1;                  /* non system descriptor */        \
        (entry).p = 1;                  /* segment is present in mem */    \
        (entry).reserved = 0;                                              \
        (entry).gran = 0;               /* limit unit is 1B */             \
        (entry).base_1 = ((uint32_t)(addr))&0xFFFF;                        \
        (entry).base_2 = (((uint32_t)(addr))>>16)&0xFF;                    \
        (entry).base_3 = (((uint32_t)(addr))>>24)&0xFF;                    \
        (entry).limit_1 = ((uint32_t)(limit))&0xFFFF;                      \
        (entry).limit_2 = (((uint32_t)(limit))>>16)&0xF;                   \
})

#endif
