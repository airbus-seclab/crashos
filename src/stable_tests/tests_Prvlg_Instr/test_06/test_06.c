// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
/*
 * test_06.c
 *
 *  Created on: 13 juin 2016
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

extern isr_t interrupt_handlers[256];

static interrupt_descriptor_t                   idt[256];
static idt_reg_t __idtr, idtr;

static pgd_struct 								pGd;
static ptb_struct 								pTb;
static uint32_t  __cr3, __cr4;

static segment_descriptor_t                     gdt[10];
static task_state_segment_t                     tss;
static uint32_t                                 *mem_test;
static uint32_t                                 user_stack[128];
static uint32_t                                 *r3esp = &user_stack[127];
static uint32_t                                 r0esp;
static gdt_reg_t __gdtr, gdtr;
static uint16_t  __cs, __ds, __ss, __tr;

uint8_t* DS_addr;

typedef void __attribute__((regparm(0))) (*function)();
#define NB_TEST 16 /** WARNING : modify the size if tests are added !!! **/
function							            test_array[NB_TEST];
uint32_t 										flag;
uint16_t 										gp_return = 0xc3c9; // leave and ret

/** Local General Protection Fault handler
 * Just return into the calling function (=test_06()) and update the flag
 * @param ctx interrupt context
 */
static void __attribute__((regparm(1))) local_gp_handler(context_t* ctx) {
	ctx->eip = (uint32_t) &gp_return;
	flag = 1;
}

void __attribute__((regparm(0))) test_lgdt() {
   	printf_log("test_lgdt    : ");
   	asm volatile (  "lgdt %0 \n"::"m"(mem_test):);
	flag = 0;
}
void __attribute__((regparm(0))) test_lldt() {
   	printf_log("test_lldt    : ");
   	asm volatile (  "lldt %0 \n"::"m"(mem_test):);
}
void __attribute__((regparm(0))) test_ltr() {
   	printf_log("test_ltr     : ");
   	asm volatile (  "ltr %0 \n"::"m"(mem_test):);
}
void __attribute__((regparm(0))) test_lidt() {
   	printf_log("test_lidt    : ");
   	asm volatile (  "lidt %0 \n"::"m"(mem_test):);
}
void __attribute__((regparm(0))) test_mov_cr3() {
   	printf_log("test_mov_cr3 : ");
   	asm volatile (  "mov %%cr3, %%eax \n":::);
}
void __attribute__((regparm(0))) test_lmsw() {
   	printf_log("test_lmsw    : ");
   	asm volatile (  "lmsw %0 \n"::"m"(mem_test):);
}
void __attribute__((regparm(0))) test_clts() {
   	printf_log("test_clts    : ");
   	asm volatile (  "clts \n":::);
}
void __attribute__((regparm(0))) test_mov_drx() {
   	printf_log("test_mov_drx : ");
   	asm volatile (  "mov %%dr0, %%eax \n":::);
}
void __attribute__((regparm(0))) test_invd() {
   	printf_log("test_invd    : ");
   	asm volatile (  "invd \n":::);
}
void __attribute__((regparm(0))) test_wbinvd() {
   	printf_log("test_wbinvd  : ");
   	asm volatile (  "wbinvd \n":::);
}
void __attribute__((regparm(0))) test_invlpg() {
   	printf_log("test_invlpg  : ");
   	asm volatile (  "invlpg %0 \n"::"m"(mem_test):);
}
void __attribute__((regparm(0))) test_hlt() {
   	printf_log("test_hlt     : ");
   	asm volatile (  "hlt \n":::);
}
void __attribute__((regparm(0))) test_rdmsr() {
   	printf_log("test_rdmsr   : ");
   	asm volatile (  "rdmsr \n":::);
}
void __attribute__((regparm(0))) test_wrmsr() {
   	printf_log("test_wrmsr   : ");
   	asm volatile (  "wrmsr \n":::);
}
void __attribute__((regparm(0))) test_rdpmc() {
   	printf_log("test_rdpmc   : ");
   	asm volatile (  "rdpmc \n":::);
}
void __attribute__((regparm(0))) test_rdtsc() {
   	printf_log("test_rdtsc   : ");
   	// "When the TSD flag is clear, the RDTSC instruction can be executed at any privilege level;
   	// when the flag is set, the instruction can only be executed at privilege level 0." (intel doc)
   	asm volatile ("rdtsc \n":::);
}

static void enter_r0() { //idt[0] routine
   asm volatile ( "mov %0, %%esp"::"m"(r0esp):"memory" );
}

static void enter_r3() {
     // Launch all privileged instructions in ring 3
	int i;
	for (i=0; i<NB_TEST; i++) {
		flag = 0;
		(test_array[i])();
		if (flag == 1) {
			printf_log(" OK\n");
		} else {
			printf_log(" NOK (no #GP occured)\n");
		}
	}
	// Enter ring 0
   asm volatile ( "pop %ebx ; int $32" );
}

static void leave_r0() {
   asm volatile (
      "mov %%esp, %0\n"
      "mov %1, %%esp\n"
      "iret"
      :"=m"(r0esp)
      :"m"(r3esp)
      :"memory" );
}

static int init_test_06() {
	//save
	get_gdtr( __gdtr );
	get_idtr( __idtr );
	get_tr( __tr );
	get_cr3( __cr3 );
	get_cr4( __cr4 );
	get_ds( __ds );
	get_ss( __ss );
	get_cs( __cs );

	//init
	init_interrupts(idt, &idtr);
	interrupt_handlers[13] = local_gp_handler;
	init_work_mem();
	init_segmentation(gdt, &tss, r3esp, &gdtr);
	init_paging(&pGd, &pTb, __cr4);

	//init test list
	test_array[0]  = test_lgdt;
	test_array[1]  = test_lldt;
	test_array[2]  = test_ltr;
	test_array[3]  = test_lidt;
	test_array[4]  = test_mov_cr3;
	test_array[5]  = test_lmsw;
	test_array[6]  = test_clts;
	test_array[7]  = test_mov_drx;
	test_array[8]  = test_invd;
	test_array[9]  = test_wbinvd;
	test_array[10] = test_invlpg;
	test_array[11] = test_hlt;
	test_array[12] = test_rdmsr;
	test_array[13] = test_wrmsr;
	test_array[14] = test_rdpmc;
	test_array[15] = test_rdtsc;
   	asm volatile (  "mov %%cr4, %%eax \n" /* set TSD flag */
   					"or $4, %%eax     \n"
   					"mov %%eax, %%cr4 \n":::);
	return 0;
}

static int test_test_06()
{
    //Set enter_r0 in IDT
    set_interrupt_gate( idt[32], (uint32_t)enter_r0 );
    set_idtr( idtr );

    // Enter r3
   *r3esp-- = (uint32_t)get_ugdt_sel(4);
   *r3esp-- = (uint32_t)&user_stack[64];
   *r3esp-- = 0;
   *r3esp-- = (uint32_t)get_ugdt_sel(3);
   *r3esp   = (uint32_t)enter_r3;
   leave_r0();
   return 0;
}

static int restore_test_06() {
   // Restore
   disable_paging();
   set_cr3( __cr3 );
   set_cr4( __cr4 );
   set_gdtr( __gdtr );
   set_idtr( __idtr );
   if( __tr ) set_tr( __tr );
   set_ds( __ds );
   set_ss( __ss );
   set_cs_m( __cs );
   return 0;
}

test_t test_06 = {
	.name = "Launch all privileged instructions in ring 3",
	.desc = " ",
	.init = init_test_06,
	.test = test_test_06,
	.fini = restore_test_06,
};


DECLARE_TEST(test_06)

