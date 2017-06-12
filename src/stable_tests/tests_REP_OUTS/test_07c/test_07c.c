// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
 * test_07.c
 *
 *  Created on: 14 juin 2016
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

extern mem_info_t mem_info;
extern isr_t interrupt_handlers[256];

static interrupt_descriptor_t                   idt[1];
static idt_reg_t __idtr, idtr;

static pgd_struct 								pGd;
static ptb_struct 								pTb;
static uint32_t  __cr3, __cr4;

static segment_descriptor_t                     gdt[10];
static task_state_segment_t                     tss;
static uint32_t                                 user_stack[128];
static uint32_t                                 *r3esp = &user_stack[127];
static uint32_t                                 r0esp;
static gdt_reg_t __gdtr, gdtr;
static uint16_t  __cs, __ds, __ss, __tr;

static uint8_t* DS_addr;
static uint32_t 								flag;
static uint16_t 								leave_ret = 0xc3c9; // leave and ret

static void __attribute__((regparm(1))) local_pf_handler(context_t* ctx) {
	int p, w_r, u_s, rsvd, i_d;

	printf_log(" (#PF) ");
	printf_log("eip 0x%x ", ctx->eip);
	/* Error code interpretation */
	p    = (ctx->error_code   ) & 1;
	w_r  = (ctx->error_code>>1) & 1;
	u_s  = (ctx->error_code>>2) & 1;
	rsvd = (ctx->error_code>>3) & 1;
	i_d  = (ctx->error_code>>4) & 1;
	if(p == 0) {
		printf_log("p=0 ");
	} else {
		printf_log("p=1 ");
	}
	if(w_r == 0) {
		printf_log("w_r=0 ");
	} else {
		printf_log("w_r=1 ");
	}
	if(u_s == 0) {
		printf_log("u_s=0 ");
	} else {
		printf_log("u_s=1 ");
	}
	if (rsvd == 1) {
		printf_log("rsvd=1 ");
	}
	if (i_d == 1) {
		printf_log("i_d=1 ");
	}
	ctx->eip = (uint32_t) &leave_ret;
	flag = 1;
}

static void enter_r0() {
   asm volatile ( "mov %0, %%esp"::"m"(r0esp):"memory" );
}

static void rep_outs_bad_rights_ptb() {
	// REP OUTS instruction
	set_ds(get_ugdt_sel(6));
	asm volatile (  "mov $0x3f8, %%edx \n" /* serial port COM 1 */
					"mov $0x0, %%esi   \n" /* address of buffer */
					"mov $0x20, %%ecx  \n" /* number of byte to read in the buffer */
					"rep outsb (%%si), %%dx  \n"
					:::);
}

static void enter_r3() {
	flag = 0;
	rep_outs_bad_rights_ptb();

	set_ds( get_kgdt_sel(4) );
	if (flag == 0) {
		printf_log("NOK (bad rights - no #PF)\n");
	} else {
		printf_log("OK\n");
	}


	// Enter ring 0
   asm volatile ( "pop %ebx ; int $32" );
}

static void leave_r0()
{
   asm volatile (
      "mov %%esp, %0\n"
      "mov %1, %%esp\n"
      "iret"
      :"=m"(r0esp)
      :"m"(r3esp)
      :"memory" );
}

static int init_test_07c() {
	//save
	get_gdtr( __gdtr );
	get_idtr( __idtr );
	get_tr( __tr );
	get_cr3( __cr3 );
	get_cr4( __cr4 );
	get_ds( __ds );
	get_ss( __ss );
	get_cs( __cs );

	//setup sg
	init_interrupts(idt, &idtr);
	interrupt_handlers[14] = local_pf_handler;
	init_work_mem();
	init_segmentation(gdt, &tss, r3esp, &gdtr);
	init_paging(&pGd, &pTb, __cr4);
	__pg_set_entry( &(pGd.pgd[1]), PG_RW, page_number((uint32_t)pTb.ptb) );
	init_serial_uart(PORT_COM1);
	init_serial_uart(PORT_COM2);
	init_serial_uart(PORT_COM3);
	init_serial_uart(PORT_COM4);

	return 0;
}

static int test_07c()
{
	// Error cases
	if (mem_info.next_free_pg == 0 && mem_info.mem_nb_free_pg < 3) {
		printf_log("Error : Not enough physical memory available.\n");
		return 1;
	}

	// Test
	// 2 physical pages for testing
	uint8_t *page_a, *page_b;
	page_a = (uint8_t *) mem_info.next_free_pg;
	page_b = page_a + PAGE_SIZE;
	mem_info.mem_nb_free_pg -= 3;
	mem_info.next_free_pg += 3*PAGE_SIZE;
	int i;
	for(i = 0; i < PAGE_SIZE; i++) {
	   page_a[i] = 'A';
	   page_b[i] = 'B';
	}
	__pg_set_entry( &(pTb.ptb[0]), PG_RW|PG_USR, page_number((uint32_t)page_b) );	// Setup page table entries
	__pg_set_entry( &(pTb.ptb[1]), PG_RW|PG_USR, page_number((uint32_t)page_a) );

	// 1 data segment for testing
	get_gdtr(gdtr);
	gdtr.limit += 4*sizeof(segment_descriptor_t);
	DS_addr = (uint8_t *) 0x400ff0; // address at the end of the first page
	set_desc_addr_type_limit(gdt[6], 0, 3, DS_addr, (1*PAGE_SIZE-1)>>12); // data RO pvl=3
	set_gdtr(gdtr);

    //set enter_r0 in IDT
    set_interrupt_gate( idt[32], (uint32_t)enter_r0 );
    set_idtr( idtr );
    //enter r3
   *r3esp-- = (uint32_t)get_ugdt_sel(4);
   *r3esp-- = (uint32_t)&user_stack[64];
   *r3esp-- = 0;
   *r3esp-- = (uint32_t)get_ugdt_sel(3);
   *r3esp   = (uint32_t)enter_r3;
   leave_r0();

   return 0;
}

static int restore_test_07c() {
	// Restore
	set_ds(get_kgdt_sel(4));
	disable_paging();
	set_cr3( __cr3 );
	set_cr4( __cr4 );
	set_gdtr( __gdtr );
	set_idtr( __idtr );
	//if( __tr ) set_tr( __tr );
	set_ds( __ds );
	set_ss( __ss );
	set_cs_m( __cs );

	return 0;
}

test_t test_7c = {
	.name = "test 07c: 'rep outs' with an expand-down data segment",
	.desc = "test a reading (REP OUTS) of an expand-down data "
            "segment with a base address and a limit critically"
            "configured.",
	.init = init_test_07c,
	.test = test_07c,
	.fini = restore_test_07c,
};


DECLARE_TEST(test_7c)




