// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
 * init.c
 *
 *  Created on: 11 mai 2016
 *      Author: anais
 */
#include <core/init.h>

extern mem_info_t mem_info;
extern uint32_t __kernel_end__;
extern mbi_t *__mbi;
extern uint32_t idt_switch;

void init_work_mem() {
	mem_info.mem_start = __align_sup((int)&__kernel_end__, 4096);
	mem_info.mem_end = __align_inf((mem_info.mem_start + (__mbi->mem_upper)*1024 - 1000000), 4096);
	mem_info.mem_size = mem_info.mem_end - mem_info.mem_start;
	mem_info.mem_nb_free_pg = (mem_info.mem_end - mem_info.mem_start)/4096;
	mem_info.mem_nb_free_pg4M = (mem_info.mem_end - mem_info.mem_start)/4194304;
	if(mem_info.mem_nb_free_pg > 0) {
		mem_info.next_free_pg = mem_info.mem_start;
	} else {
		mem_info.next_free_pg = 0;
	}
	if(mem_info.next_free_pg4M > 0) {
		mem_info.next_free_pg4M = mem_info.mem_start;
	} else {
		mem_info.next_free_pg4M = 0;
	}

}

// int init_segmentation(segment_descriptor_t *gdt, uint32_t nr_entry, task_state_segment_t *tss, uint32_t *stack_ptr, gdt_reg_t *gdtr) {
// 	if (gdt ==  NULL || tss == NULL || stack_ptr == NULL || gdtr == NULL) {
// 		printf("Init segmentation : Null parameter.\n");
// 		return 1;
// 	}

// 	if (nr_entry < 6) {
// 		printf("gdt too small\n");
// 		return 1;
// 	}

// 	// Fill GDT
// 	set_null_desc( gdt[0] );
// 	set_code_desc( gdt[1], 0 );
// 	set_data_desc( gdt[2], 0 );
// 	set_code_desc( gdt[3], 3 );
// 	set_data_desc( gdt[4], 3 );
// 	set_tss_desc( gdt[5], (uint32_t)tss );

// 	// Set TSS
// 	tss->esp0 = (uint32_t)stack_ptr;
// 	tss->ss0 = get_kgdt_sel(2);

// 	// Update GDTR
// 	gdtr->base_addr = (uint32_t)gdt;
// 	gdtr->limit = nr_entry*sizeof(segment_descriptor_t)-1;
// 	set_gdtr( *gdtr );

// 	// Set selectors
// 	set_tr( get_kgdt_sel(5) ); 	 // tss in task register
// 	set_cs_i( get_kgdt_sel(1) ); // code ring 0
// 	set_ds( get_ugdt_sel(4) );   // data ring 3
// 	set_ss( get_kgdt_sel(2) );	 // data ring 0

// 	return 0;
// }
int init_segmentation(segment_descriptor_t *gdt, task_state_segment_t *tss, uint32_t *stack_ptr, gdt_reg_t *gdtr) {
	if (gdt ==  NULL || tss == NULL || stack_ptr == NULL || gdtr == NULL) {
		printf("Init segmentation : Null parameter.\n");
		return 1;
	}


	// Fill GDT
	set_null_desc( gdt[0] );
	set_code_desc( gdt[1], 0 );
	set_data_desc( gdt[2], 0 );
	set_code_desc( gdt[3], 3 );
	set_data_desc( gdt[4], 3 );
	set_tss_desc( gdt[5], (uint32_t)tss );

	// Set TSS
	tss->esp0 = (uint32_t)stack_ptr;
	tss->ss0 = get_kgdt_sel(2);

	// Update GDTR
	gdtr->base_addr = (uint32_t)gdt;
	gdtr->limit = 10*sizeof(segment_descriptor_t)-1;
	set_gdtr( *gdtr );

	// Set selectors
	set_tr( get_kgdt_sel(5) ); 	 // tss in task register
	set_cs_i( get_kgdt_sel(1) ); // code ring 0
	set_ds( get_ugdt_sel(4) );   // data ring 3
	set_ss( get_kgdt_sel(2) );	 // data ring 0

	return 0;
}


int init_paging(pgd_struct *pGd, ptb_struct *pTb, uint32_t __cr4) {
	if (pGd == NULL) {
		printf("Init paging : Null parameter.\n");
		return 1;
	}

	// Fill PGD
	__pg_set_4MB_entry( &(pGd->pgd[0]), PG_RW|PG_USR, 0 ); //for identity mapping
	__pg_set_entry( &(pGd->pgd[1]), PG_RW|PG_USR, page_number((uint32_t)pTb->ptb) );
	pGd->next_free_pde = 2;
	pGd->current_ptb = 1;
	pTb->next_free_pte = 0;

	// Update CR4 and CR3
	set_cr4( __cr4|CR4_PSE ); // CR4_PageSizeExtension = 1000
	set_cr3( &(pGd->pgd[0]) );

	// Enable paging
	enable_paging();
	return 0;
}



int init_interrupts(interrupt_descriptor_t idt[256], idt_reg_t *idtr) {
	int i;
	uint32_t wrapper_addr = (uint32_t) &idt_switch;

	/* Initialize the handler array and fill IDT */
	init_handler_array();
	for (i=0; i<20; i++) { // WARNING : size = size of idt_switch
		set_interrupt_gate( idt[i], (uint32_t)((wrapper_addr + i*16)) );
	}
	//set_interrupt_gate( idt[14], (uint32_t)&idt_switch ); /* used with simple_handler.s */

	/* Update IDTR */
	idtr->base_addr = (uint32_t) idt;
	idtr->limit = 256*sizeof(interrupt_descriptor_t)-1;
	set_idtr( *idtr );

	return 0;
}

