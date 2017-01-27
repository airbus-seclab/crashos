#ifndef __PAGE_H__
#define __PAGE_H__

#include <core/types.h>

/***************************************************************
                DEFAULT PHYSICAL MEMORY PC LAYOUT
***************************************************************/

/** 1KB IVT */
#define IVT_START             0x00000000
#define IVT_END               0x00000400
/** 512B BIOS data area */
#define BIOS_DATA_START       0x00000400
#define BIOS_DATA_END         0x00000600
/** conventional memory */
#define LOW_MEM_START         0x00000600
#define LOW_MEM_END           0x0009fc00
/** BIOS extended data area */
#define BIOS_EXT_DATA_START   0x0009fc00
#define BIOS_EXT_DATA_END     0x000a0000
/** 128KB of VGA memory */
#define VGA_START             0x000a0000
#define VGA_END               0x000c0000
/** 192KB VGA BIOS rom */
#define VGA_BIOS_START        0x000c0000
#define VGA_BIOS_END          0x000f0000
/** 64KB BIOS rom */
#define BIOS_START            0x000f0000
#define BIOS_START_OFFSET     0x0000fff0
#define BIOS_END              0x00100000
/** Extended memory */
#define EXT_MEM_START         (1<<20)
/** DMA limit */
#define DMA_END               (16<<20)
/** BIOS bootloader @ */
#define BIOS_BOOTLOADER       0x00007c00

/** bios macros */
#define mem_range(_x_addr_x_,_x_start_x_,_x_end_x_) \
   (((_x_addr_x_)>=(_x_start_x_)) && ((_x_addr_x_)<(_x_end_x_)))

#define is_bios_mem(__AdDr__) \
   (mem_range((__AdDr__),IVT_START,BIOS_DATA_END) || \
    mem_range((__AdDr__),BIOS_EXT_DATA_START,EXT_MEM_START))

/**
** rmode max addr is 0xffff<<4 + 0xffff == 0x10ffef == RM_WRAP_LIMIT
**
** - if a20 is on, we allow access to such an address
** - if a20 is off, we emulate wrap-around by mirroring
**   pages from [ 1MB ; 1MB+64KB [ to [ 0 ; 64KB [
**
*/
#define RM_LIMIT             EXT_MEM_START
#define RM_WRAP_LIMIT        (RM_LIMIT+(64<<10))
#define RM_STACK_BOTTOM      ((LOW_MEM_END & 0xffff0000) - 2)
#define RM_BASE_SS           ((RM_STACK_BOTTOM & 0xffff0000)>>4)
#define RM_BASE_SP           (RM_STACK_BOTTOM & 0xffff)


/**************************************************************
                 VIRTUAL MEMORY PAGES INFO
**************************************************************/

#define PAGE_SIZE      4096
#define PAGE_BITSHIFT  12
#define PAGE_4M_SHIFT  22

#define PTB_SIZE       4194304
#define PTB_BITSHIFT   22

#define PDE_PER_PD     1024
#define PTE_PER_PT     1024

#define MAX_VADDR      0xffffffff

#define __clear_page(addr)					   \
   ({								   \
      uint32_t *___D = (uint32_t*)(addr);			   \
      while( (uint32_t)___D < ((uint32_t)(addr))+PAGE_SIZE )	   \
	 *___D++ = 0;						   \
   })

#define __copy_page(__dst,__src)					\
   ({									\
      uint32_t *__D = (uint32_t*)(__dst);				\
      uint32_t *__S=(uint32_t*)(__src);					\
      while( (uint32_t)__D < ((uint32_t)(__dst))+PAGE_SIZE )		\
	 *__D++ = *__S++;						\
   })

#define page_directory_index(addr)   ((addr)>>PTB_BITSHIFT)
#define page_table_index(addr)       (((addr)>>PAGE_BITSHIFT)&0x3ff)
#define page_index(addr)             ((addr)&0x00000fff)
#define page_index_4MB(addr)         ((addr)&0x003fffff)
#define page_number(addr)            ((addr)>>PAGE_BITSHIFT)
#define page_4M_number(addr)         ((addr)>>PAGE_4M_SHIFT)

#define page_align_inf(addr)         __align_inf(addr,PAGE_SIZE)
#define page_align_sup(addr)         __align_sup(addr,PAGE_SIZE)
#define page_aligned(addr)           __aligned(addr,PAGE_SIZE)

#define page_align_inf_4MB(addr)     __align_inf(addr,PTB_SIZE)
#define page_align_sup_4MB(addr)     __align_sup(addr,PTB_SIZE)
#define page_aligned_4MB(addr)       __aligned(addr,PTB_SIZE)

#define page_nr_from_sz(_sz_)        (((_sz_)>>PAGE_BITSHIFT)+(((_sz_)%PAGE_SIZE)?1:0))
#define ptb_nr_from_sz(_sz_)         (((_sz_)>>PTB_BITSHIFT)+(((_sz_)%PTB_SIZE)?1:0))
#define ptb_nr_from_pg(_pg_)         (((_pg_)/PTE_PER_PT)+(((_pg_)%PTE_PER_PT)?1:0))

#define __page_aligned__             __attribute__((aligned(PAGE_SIZE)))

#define pde(pGd,aDDr)                ((pde_t*)(&pGd[page_directory_index(aDDr)]))
#define ptb(pDe)                     ((pte_t*)(((pDe)->addr)<<PAGE_BITSHIFT))
#define pte(pTb,aDDr)                ((pte_t*)(pTb)+page_table_index(aDDr))
#define page(pTe)                    (((pTe)->addr)<<PAGE_BITSHIFT)
#define page_4MB(pDe)                ((((pde_4MB_t*)(pDe))->addr)<<PTB_BITSHIFT)



/**
** Physical addr range
** where vmm is located
*/
#define vmm_paddr_range_page(_addr_)					\
   mem_range( _addr_,							\
	      page_align_inf((uint32_t)info),				\
	      page_align_inf(info->hrd.ram.size)			\
      )

#define vmm_paddr_range_large_page(_addr_)				\
   mem_range( _addr_,							\
	      page_align_inf_4MB((uint32_t)info),			\
	      page_align_inf_4MB(info->hrd.ram.size)			\
      )


typedef union control_register_3
{
   struct
   {
      uint_t     r1:3;     /* reserved */
      uint_t     pwt:1;    /* write through (1), write back (0) */
      uint_t     pcd:1;    /* page dir could be cached (0) */
      uint_t     r2:7;     /* reserved */
      uint_t     addr:20;  /* physical base addr of the page directory (align 4KB) */
   };

   uint32_t  raw;

} __attribute__((packed)) cr3_reg_t;

/**
** Page directory and page table entries structures
*/
typedef union page_directory_entry
{
   struct
   {
      uint_t     p:1;      /* page table present (1) flag */
      uint_t     rw:1;     /* read/write flag (0 read, 1 r/w) */
      uint_t     lvl:1;    /* user (1) or supervisor (0) privilege level */
      uint_t     pwt:1;    /* page write through caching (1) or write back (0) */
      uint_t     pcd:1;    /* page cache disabled (0 to enable caching) */
      uint_t     acc:1;    /* set to 0 when page (table) is loaded, CPU will set the bit when accessed */
      uint_t     r:1;      /* reserved 0 */
      uint_t     ps:1;     /* page size, 0 for 4KB */
      uint_t     g:1;      /* global page, ignored */
      uint_t     avl:3;    /* available for programmer */
      uint32_t   addr:20;  /* page table physical base addr, align 4KB, 20 most significant bits */

   } __attribute__((packed));

   raw32_t;

} __attribute__((packed)) pde_t;

typedef union page_directory_entry_4MB
{
   struct
   {
      uint_t     p:1;      /* page table present (1) flag */
      uint_t     rw:1;     /* read/write flag (0 read, 1 r/w) */
      uint_t     lvl:1;    /* user (1) or supervisor (0) privilege level */
      uint_t     pwt:1;    /* page write through caching (1) or write back (0) */
      uint_t     pcd:1;    /* page cache disabled (0 to enable caching) */
      uint_t     acc:1;    /* set to 0 when page (table) is loaded, CPU will set the bit when accessed */
      uint_t     d:1;      /* dirty set to 1 by CPU when page is written. In charge of OS to clear it */
      uint_t     ps:1;     /* page size, 0 for 4KB */
      uint_t     g:1;      /* global page, ignored */
      uint_t     avl:3;    /* available for programmer */
      uint_t     pat:1;    /* pat */
      uint_t     rsv:9;    /* reserved */
      uint32_t   addr:10;  /* page physical base addr, align 4MB, 10 most significant bits */

   } __attribute__((packed));

   raw32_t;

} __attribute__((packed)) pde_4MB_t;


typedef union page_table_entry
{
   struct
   {
      uint_t     p:1;      /* page present (1) flag */
      uint_t     rw:1;     /* read/write flag (0 read, 1 r/w) */
      uint_t     lvl:1;    /* user (1) or supervisor (0) privilege level */
      uint_t     pwt:1;    /* page write through(1)/back(0) caching */
      uint_t     pcd:1;    /* page cache disabled (0 to enable caching) */
      uint_t     acc:1;    /* accessed (same as above) */
      uint_t     d:1;      /* dirty set to 1 by CPU when page is written. In charge of OS to clear it */
      uint_t     pat:1;    /* pat */
      uint_t     g:1;      /* global page, for prevent TLB flush on frequently accessed pages */
      uint_t     avl:3;    /* available for programmer */
      uint32_t   addr:20;  /* page physical base addr (align 4KB) */

   } __attribute__((packed));

   raw32_t;

} __attribute__((packed)) pte_t;




#define PG_P_BIT       0
#define PG_RW_BIT      1
#define PG_LVL_BIT     2
#define PG_PWT_BIT     3
#define PG_PCD_BIT     4
#define PG_ACC_BIT     5
#define PG_DRT_BIT     6
#define PG_PS_BIT      7
#define PG_PAT_BIT     7
#define PG_G_BIT       8
#define PG_4MB_PAT_BIT 12

#define PG_KRN         0
#define PG_USR         (1<<PG_LVL_BIT)

#define PG_RO          0
#define PG_RW          (1<<PG_RW_BIT)

#define PG_P           (1<<PG_P_BIT)
#define PG_PCD         (1<<PG_PCD_BIT)
#define PG_PWT         (1<<PG_PWT_BIT)
#define PG_GLOBAL      (1<<PG_G_BIT)
#define PG_4MB         (1<<PG_PS_BIT)
#define PG_G           (1<<PG_G_BIT)

//pfn = Page Frame Number
#define __pg_set_entry(_e_,_attr_,_pfn_)		\
   (_e_)->raw = ((_pfn_)<<PAGE_BITSHIFT)|(_attr_)|PG_P

#define __pg_set_4MB_entry(_e_,_attr_,_pfn_)			\
   (_e_)->raw = ((_pfn_)<<PTB_BITSHIFT)|(_attr_)|PG_4MB|PG_P

#define set_cr3(val) asm volatile ( "mov %%eax, %%cr3"::"a"(val) )
#define get_cr3(val) asm volatile ( "mov %%cr3, %%eax":"=a"(val) )

#define set_cr0(val) asm volatile ( "mov %%eax, %%cr0"::"a"(val) )
#define get_cr0(val) asm volatile ( "mov %%cr0, %%eax":"=a"(val) )
#define disable_protected_mode() asm volatile (	\
	"mov %%cr0, %%eax       \n"					\
	"and $0xfffffffe, %%eax \n"					\
	"mov %%eax, %%cr0":::"eax" )

#define enable_protected_mode() asm volatile (	\
	"mov %%cr0, %%eax       \n"					\
	"or $0x00000001, %%eax  \n"					\
	"mov %%eax, %%cr0":::"eax" )

#define enable_paging() asm volatile (	    	\
	"mov %%cr0, %%eax       \n"					\
	"or $0x80000000, %%eax  \n"					\
	"mov %%eax, %%cr0":::"eax" )

#define disable_paging() asm volatile (	        \
	"mov %%cr0, %%eax       \n"					\
	"and $0x7fffffff, %%eax \n"					\
	"mov %%eax, %%cr0":::"eax" )

#define CR4_PSE             (1<<4)
#define CR4_PGE             (1<<7)
#define get_cr4(val)  asm volatile ( "mov %%cr4, %%eax":"=a"(val) )
#define set_cr4(val)  asm volatile ( "mov %%eax, %%cr4"::"a"(val) )

#define get_cr0(val)  asm volatile ( "mov %%cr0, %%eax":"=a"(val) )
#define set_cr0(val)  asm volatile ( "mov %%eax, %%cr0"::"a"(val) )

/** (AG)
 * PGD and PTB structures
 */
typedef struct ptb_struct {
	pte_t   __attribute__((aligned(PAGE_SIZE))) ptb[PTE_PER_PT];
	uint16_t next_free_pte;
} ptb_struct;

typedef struct pgd_struct {
	pde_t   __attribute__((aligned(PAGE_SIZE))) pgd[PDE_PER_PD];
	uint16_t next_free_pde;
	uint16_t current_ptb;
} pgd_struct;

/**
 * Allocate a new page if no physical page is mapped for the required virtual address - for PF handler
 * @param v_addr virtual address to map
 * @param mem_info Physical Memory Informations to update
 * @return 0 if success, 1 if failure
 */
int allocate_missing_page(uint32_t addr, mem_info_t* mem_info);


#endif
