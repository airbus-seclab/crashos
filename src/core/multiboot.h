#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#define MBH_MAGIC		0x1BADB002
#define MBH_BOOTLOADER_MAGIC	0x2BADB002
#define MBH_FLAGS		0x00000003

#define __multiboot_header__    __attribute__ ((section(".multiboot_hdr"),aligned(4)))

#define MBI_FLAG_MEM_BIT       0
#define MBI_FLAG_BOOT_BIT      1
#define MBI_FLAG_CMDLINE_BIT   2
#define MBI_FLAG_MODS_BIT      3
#define MBI_FLAG_MMAP_BIT      6

#define MBI_FLAG_MEM          (1<<MBI_FLAG_MEM_BIT)
#define MBI_FLAG_BOOT         (1<<MBI_FLAG_BOOT_BIT)
#define MBI_FLAG_CMDLINE      (1<<MBI_FLAG_CMDLINE_BIT)
#define MBI_FLAG_MODS         (1<<MBI_FLAG_MODS_BIT)
#define MBI_FLAG_MMAP         (1<<MBI_FLAG_MMAP_BIT)

/**
** Structures from GRUB sources
*/

/** The Multiboot header. */
typedef struct multiboot_header
{
   unsigned long magic;
   unsigned long flags;
   unsigned long checksum;
   unsigned long header_addr;
   unsigned long load_addr;
   unsigned long load_end_addr;
   unsigned long bss_end_addr;
   unsigned long entry_addr;
} multiboot_header_t;

/** The symbol table for a.out. */
typedef struct aout_symbol_table
{
   unsigned long tabsize;
   unsigned long strsize;
   unsigned long addr;
   unsigned long reserved;
} aout_symbol_table_t;

/** The section header table for ELF. */
typedef struct elf_section_header_table
{
   unsigned long num;
   unsigned long size;
   unsigned long addr;
   unsigned long shndx;
} elf_section_header_table_t;

/** The Multiboot information. */
typedef struct multiboot_info
{
   unsigned long flags;
   unsigned long mem_lower;
   unsigned long mem_upper;
   unsigned long boot_device;
   unsigned long cmdline;
   unsigned long mods_count;
   unsigned long mods_addr;
   union
   {
      aout_symbol_table_t        aout_sym;
      elf_section_header_table_t elf_sec;
   } u;
   unsigned long mmap_length;
   unsigned long mmap_addr;
   unsigned long drives_length;
   unsigned long drives_addr;
   unsigned long config_table;
   unsigned long boot_loader_name;
   unsigned long apm_table;
   unsigned long vbe_control_info;
   unsigned long vbe_mode_info;
   unsigned long vbe_mode;
   unsigned long vbe_interface_seg;
   unsigned long vbe_interface_off;
   unsigned long vbe_interface_len;
} mbi_t;

/** The module structure. */
typedef struct module
{
   unsigned long mod_start;
   unsigned long mod_end;
   unsigned long string;
   unsigned long reserved;
} module_t;

/** The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct memory_map
{
   unsigned long size;
   unsigned long base_addr_low;
   unsigned long base_addr_high;
   unsigned long length_low;
   unsigned long length_high;
   unsigned long type;
} memory_map_t;

#endif

