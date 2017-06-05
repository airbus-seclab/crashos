// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
 * init.h
 *
 *  Created on: 11 mai 2016
 *      Author: anais
 */

#ifndef __INIT_H_
#define __INIT_H_

#include <core/types.h>
#include <core/segmentation.h>
#include <core/interrupts.h>
#include <core/page.h>
#include <core/serial_driver.h>
#include <core/multiboot.h>
#include <core/print.h>
#include <core/start.h>

/**
 * initialize the mem_info struct to define the available work memory\n
 * @param mbi : Multiboot Information
 */
/*
 * 0           1MB				2MB                             4MB													RAM_SIZE\n
 * |------------|-----------------|------------------------------|-----------------------------------------------------|\n
 *  ___________________________________________________________________________________________________________________\n
 * |      |     |                 |              |                                                           |		   |\n
 * | BIOS | ... |         <--pile | kernel image |                        WORK MEMORY                        |   ...   |\n
 * |______|_____|_________________|______________|___________________________________________________________|_________|\n
\n
 * |---------------------------------------------|-----------------------------------------------------------|\n
 *										  	mem_start													  mem_end\n
 *												 <-------------------------mem_size-------------------------->\n
 *												 |\n
 *									next_free_pg = next_free_pg4M\n
 **/
void init_work_mem();

/**
 *  * initialize the GDT (Global Descriptor Table) with the following entries and update gdtr and segment selectors\n
 * @param gdt : GDT to fill
 * @param tss : TSS to update
 * @param stack_ptr : stack pointer for tss->esp0
 * @param gdtr : GDT register to update
 * @return 0 if success
 */
/*
 *	GDTR------>  ________GDT_________
 *			+ 0	|        null        |\n
 *	CS----> + 8	| 	  CS ring 0		 |\n
 *	SS----> +16	| 	  DS ring 0		 |<--------------------+\n
 *	 		+24 | 	  CS ring 3		 |                     |\n
 *	DS----> +32	| 	  DS ring 3		 |	     ____TSS_____  |\n
 *	TR----> +40	|        TSS         |----->|    ss0	 |-+\n
 *	 		  	|____________________|		|	 esp0	 |-----> ____stack_ptr____\n
 * 											|    ...     |      |       ...       |\n
 * 											|____________|		|_________________|\n
 * 											*/
//int init_segmentation(segment_descriptor_t gdt[6], uint32_t nr_entry, task_state_segment_t *tss, uint32_t *stack_ptr, gdt_reg_t *gdtr);
int init_segmentation(segment_descriptor_t gdt[6], task_state_segment_t *tss, uint32_t *stack_ptr, gdt_reg_t *gdtr);

/**
 * initialize the PGD (Page Directory) with the following entries, update CR3 and CR4, and enable paging (update CR0)\n
 * @param pGd : PGD struct to fill
 * @param pTb : Page Table Struct
 * @param __cr4 : current CR4
 * @return 0 if success
 */
/* 	CR3------------> ____________PGD___________\n
 * 					|             0            |\n
 * 					|  (identity mapping 4MB)  |\n
 * 					|--------------------------|\n
 * 	current_ptb---->|            PTB           |-------------------> ________PTB________\n
 * 					|--------------------------| next_free_pte----->|        ...        |--------> ???\n
 * 	next_free_pde-->|            ...           |         			|-------------------|\n
 * 		    			                                 			|        ...        |\n
 * 		    	    		                             			|-------------------|\n
 * */
int init_paging(pgd_struct *pGd, ptb_struct *pTb, uint32_t __cr4);

/**
 * initialize the IDT (Interrupt Descriptor Table) with the following entries (32 first entries for exceptions)\n
 * @param idt : IDT to fill
 * @param idtr : IDT Register to update
 * @return 0 if success
 */
/* IDTR ---->____________________IDT____________________\n
 *          | 0  #DE : Divide Error Exception         N |\n
 *          | 1  #DB : Debug Exception                N |\n
 *          | 2  NMI Interrupt                        N |\n
 * 			| 3  #BP : Breakpoint Exception           N |\n
 * 			| 4  #OF : Overflow Exception             N |\n
 * 			| 5  #BR : Bound Range exceeded exception N |\n
 * 			| 6  #UD : Invalid Opcode Exception       N |\n
 * 			| 7  #NM : Device Not Available Exception N |\n
 * (ERR=0)  | 8  #DF : Double Fault Exception         Y |--specific handler\n
 * 			| 9  Coprocessor Segment Overrun          N |\n
 * 			| 10 #TS : Invalid TSS Exception          Y |\n
 * 			| 11 #NP : Segment Not Present            Y |\n
 * 			| 12 #SS : Stack Fault Exception          Y |\n
 * 			| 13 #GP : General Protection Exception   Y |--specific handler\n
 * 			| 14 #PF : Page Fault Exception           Y |--specific handler\n
 * 			| 15 Intel Reserved entry                   |\n
 * 			| 16 #MF : x87 FPU Floating-Point Error   N |\n
 * 			| 17 #AC : Alignment Check Exception      Y |\n
 * 			| 18 #MC : Machine-Check Exception        N |\n
 * 			| 19 #XF : SIMD Floating-Point Exception  N |\n
 * 			| 					...                     |\n
 * 			|___________________________________________|\n
 * 			|										    |\n
 * 			| 32                                        |\n
 * 			|                                           |\n
 * 			| 		User Defined Interrupts             |\n
 * 			|										    |\n
 * 			| 255                                       |\n
 * 			|___________________________________________|\n
 *\n*/
int init_interrupts(interrupt_descriptor_t idt[256], idt_reg_t *idtr);

#endif /* __INIT_H_ */
