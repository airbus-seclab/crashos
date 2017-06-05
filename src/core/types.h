// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#ifndef __TYPES_H__
#define __TYPES_H__

#define __align_inf(addr,val)  ((addr)&(~((val)-1)))
#define __align_sup(addr,val)  __align_inf(addr+val,val)
#define __aligned(addr,val)    (!((addr)&((val)-1)))

#define int_align_inf(addr)  __align_inf(addr,4)
#define int_align_sup(addr)  __align_sup(addr,4)
#define int_aligned(addr)    __aligned(addr,4)

#define get_bit(val,pos) (val>>pos) & 1
#define clear_bit(val,pos) val & ~(1<<pos)
#define set_bit(val,pos) val | (1<<pos)

#define NULL                 ((void*)0)

typedef unsigned char            uint8_t;
typedef unsigned short int       uint16_t;
typedef unsigned long int        uint32_t;
typedef unsigned long long int   uint64_t;

typedef char                     int8_t;
typedef short int                int16_t;
typedef long int                 int32_t;
typedef long long int            int64_t;

typedef unsigned int             uint_t;

typedef enum { FALSE=0, TRUE } bool;

typedef unsigned int size_t;

#define range(_val_,_start_,_end_) (((_val_)>=(_start_))&&((_val_)<=(_end_)))

/**
** Offset of a field from a structure
*/
#define offsetof(type,field)     ((uint32_t)(&(((type*)0)->field)))

#define __unused__		  __attribute__ ((unused))

/**
** Decomposition of 32 bis
** in 16 and 8 bits
*/
typedef union raw_32_bits_entry
{
   uint32_t   raw;
   int32_t    sraw;

   /* H/L word access */
   struct
   {
      union
      {
	 /* H/L byte access */
	 struct
	 {
	    uint8_t blow;
	    uint8_t bhigh;
	 };

	 uint16_t wlow;
      };

      uint16_t whigh;
   };

} __attribute__((packed)) raw32_t;

/**
** Decomposition of 64 bits
** in 32, 16 and 8 bits
*/
typedef union raw_64_bits_entry
{
   uint64_t     raw;
   int64_t      sraw;

   /* H/L dword access */
   struct
   {
      union
      {
	 /* H/L word access */
	 struct
	 {
	    union
	    {
	       /* H/L byte access */
	       struct
	       {
		  uint8_t blow;
		  uint8_t bhigh;
	       };

	       uint16_t wlow;
	    };

	    uint16_t whigh;
	 };

	 uint32_t  low;
      };

      uint32_t  high;
   };

} __attribute__((packed)) raw64_t;

/**
** Simple far pointer structs
*/
typedef struct far_pointer
{
   raw32_t  offset;
   uint16_t segment;

} __attribute__((packed)) fptr_t;

typedef struct far_ptr16
{
   uint16_t offset;
   uint16_t segment;

} __attribute__((packed)) fptr16_t;

typedef struct far_ptr32
{
   uint32_t offset;
   uint16_t segment;

} __attribute__((packed)) fptr32_t;


/**
** code_16 handler
*/
typedef int (*code_16_t)();

/**
** No cast on access
*/
typedef union location
{
   uint32_t  linear;

   void      *addr;

   uint8_t   *u8;
   uint16_t  *u16;
   uint32_t  *u32;

   fptr16_t  *f16;
   fptr32_t  *f32;

} __attribute__((packed)) loc_t;

/**
** Simple buffer struct
*/
typedef struct buffer
{
   uint8_t   *data;
   uint32_t  sz;

} __attribute__((packed)) buffer_t;

/**
 * Physical memory information
 */
typedef struct mem_info_t {
	uint32_t mem_start;			/** The address of the physical memory start*/
	uint32_t mem_end;			/** The address of the physical memory end */
	uint32_t mem_size;			/** The size of the physical memory available*/
	uint32_t mem_nb_free_pg; 	/** The number of free 4KB-pages*/
	uint32_t mem_nb_free_pg4M;	/** The number of free 4MB-pages */
	uint32_t next_free_pg;	 	/** The address of the next free 4KB-page*/
	uint32_t next_free_pg4M; 	/** The address of the next free 4MB-page*/
} mem_info_t;



#endif



