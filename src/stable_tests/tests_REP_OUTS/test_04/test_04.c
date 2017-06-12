/*
 * test_04.c
 *
 *  Created on: 9 juin 2016
 *      Author: anais
 */

#include <core/test.h>
#include <core/init.h>

#include <core/print.h>
#include <core/start.h>

#include <core/segmentation.h>
#include <core/interrupts.h>
#include <core/page.h>
#include <core/serial_driver.h>

// Global and external variables
extern mem_info_t mem_info;
extern isr_t interrupt_handlers[256];

static segment_descriptor_t         gdt[10]; // 6 (default size) + 4 (for testing)
static task_state_segment_t         tss;
static uint32_t                     user_stack[128];
static uint32_t                     *r3esp = &user_stack[127];
static gdt_reg_t __gdtr, gdtr;
static uint16_t  __cs, __ds, __ss, __tr;

static pgd_struct 					pGd;
static ptb_struct 					pTb;
static uint32_t __cr3, __cr4;

static interrupt_descriptor_t       idt[256];
static idt_reg_t __idtr, idtr;

static uint8_t* DS_addr;
static uint32_t 					flag;
static uint16_t 					leave_ret = 0xc3c9; // leave and ret

static void __attribute__((regparm(1))) local_gp_handler(context_t* ctx) {
	printf_log(" (#GP) ");
	ctx->eip = (uint32_t) &leave_ret;
	flag = 1;
}

static int init_test_4() {
	// Save
	get_gdtr( __gdtr );
	get_idtr( __idtr );
	get_tr( __tr );
	get_cr3( __cr3 );
	get_cr4( __cr4 );
	get_ds( __ds );
	get_ss( __ss );
	get_cs( __cs );

	// Init
	init_interrupts(idt, &idtr);
	interrupt_handlers[13] = local_gp_handler;
	init_work_mem();
	init_segmentation(gdt, &tss, r3esp, &gdtr);
	init_paging(&pGd, &pTb, __cr4);
	init_serial_uart(PORT_COM1);
	init_serial_uart(PORT_COM2);
	init_serial_uart(PORT_COM3);
	init_serial_uart(PORT_COM4);
	log("end_init\n");
	return 0;
}

void rep_outs_expand_down() {
	set_ds(get_kgdt_sel(6));
	asm volatile (  "mov $0x3f8, %%edx \n" /* serial port COM 1 */
					"mov $0x10011, %%esi \n" /* address of beginning of the buffer */
					"mov $0x20, %%ecx \n" /* number of byte to read in the buffer */
					"rep outsb (%%esi), %%dx \n" /* warning : here : esi and not si ! */
					:::);
}

static int test_4() {
	uint8_t *page_a, *page_b, *page_c, *page_d;
	int i;
	uint32_t ds_limit;
	uint32_t limit;

	// Error cases
	if (mem_info.next_free_pg == 0 && mem_info.mem_nb_free_pg < 3) {
		printf_log("Error : Not enough physical memory available.\n");
		return 1;
	}

	// Test
	// 2 physical pages for testing
	page_a = (uint8_t *) mem_info.next_free_pg;
	page_b = page_a + 2*PAGE_SIZE;
	page_c = page_b + PAGE_SIZE; // page normally mapped when the page fault occurs
	page_d = page_c + PAGE_SIZE; // page normally mapped when the page fault occurs
	mem_info.mem_nb_free_pg -= 3;
	mem_info.next_free_pg += 3*PAGE_SIZE;
	for(i = 0; i < PAGE_SIZE; i++) {
	   page_a[i] = 'A';
	   page_b[i] = 'B';
	   page_c[i] = 'C';
	   page_d[i] = 'D';
	}
	__pg_set_entry( &(pTb.ptb[0]), PG_RW|PG_USR, page_number((uint32_t)page_b) );	// Setup page table entries
	__pg_set_entry( &(pTb.ptb[1]), PG_RW|PG_USR, page_number((uint32_t)page_a) );

	// 1 data segment for testing
	get_gdtr(gdtr);
	gdtr.limit += 4*sizeof(segment_descriptor_t);
	DS_addr = (uint8_t *) 0x400ff0; // address at the end of the first physical page
	//DS_addr = 0x00;
	limit = 0x10010;
	//set_desc_addr_type_limit_1B(gdt[6], 4, 0, 0, 0); // data RO, Expand-down pvl=0

	(gdt[6]).s        = 1; /* no system segment */
	(gdt[6]).type     = 4; /* 0EWA = 0100 : Data segment, RO-Expand_down */
	(gdt[6]).dpl      = 0; /* ring 0 segment */
	(gdt[6]).db       = 0; /* b=0 : upper bound = 0xffff */
	(gdt[6]).p        = 1; /* segment present */
	(gdt[6]).reserved = 0;
	(gdt[6]).gran     = 0; /* max segment size : 1MB (=0xfffff) */
	(gdt[6]).base_1   = ((uint32_t)(DS_addr))&0xFFFF;
	(gdt[6]).base_2   = (((uint32_t)(DS_addr))>>16)&0xFF;
	(gdt[6]).base_3   = (((uint32_t)(DS_addr))>>24)&0xFF;
	(gdt[6]).limit_1  = ((uint32_t)(limit))&0xFFFF;
	(gdt[6]).limit_2  = (((uint32_t)(limit))>>16)&0xF;
	set_gdtr(gdtr);

	set_ds(get_kgdt_sel(6));
	asm volatile (  "mov %%ds, %%ebx\n"
					"lsl %%ebx, %%eax \n"
					"mov %%eax, %0"
					:"=m"(ds_limit)
					:
					:"memory" );

	set_ds( get_kgdt_sel(4) );
	printf_log("ds_limit : 0x%x\n", ds_limit);

	// REP OUTS instruction
	flag = 0;
	rep_outs_expand_down();

	set_ds( get_kgdt_sel(4) );
	if (flag == 0) {
		printf_log("NOK : no #GP (expand_down)\n");
	} else {
		printf_log("OK (expand_down)\n");
	}

	return 0;
}

static int restore_test_4() {
	// Restore
	set_ds(get_kgdt_sel(4));
	disable_paging();
	set_cr3( __cr3 );
	set_cr4( __cr4 );
	set_gdtr( __gdtr );
	set_idtr( __idtr );
	set_ds( __ds );
	set_ss( __ss );
	set_cs_m( __cs );

	return 0;
}

test_t test_04 = {
	.name = "test 04: 'rep outs' with an expand-down data segment",
	.desc = "test a reading (REP OUTS) of an expand-down data "
                "segment with a base address and a limit critically"
                "configured.",
	.init = init_test_4,
	.test = test_4,
	.fini = restore_test_4,
};


DECLARE_TEST(test_04)



