// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/test.h>
#include <core/init.h>
#include <core/segmentation.h>
#include <core/interrupts.h>
#include <core/page.h>
#include <core/print.h>
#include <core/start.h>
#include <core/utils.h>

// Global and external variables
#define IO_PORT_MIN 0x0000
#define IO_PORT_MAX 0xffff

extern mem_info_t mem_info;

static segment_descriptor_t         gdt[10]; // 6 (default size) + 4 (for testing)
static task_state_segment_t         tss;
static uint32_t                     user_stack[128];
static uint32_t                     *r3esp = &user_stack[127];
static gdt_reg_t __gdtr, gdtr;
static uint16_t  __cs, __ds, __ss, __tr;

static uint32_t __cr3, __cr4;

static interrupt_descriptor_t       idt[256];
static idt_reg_t __idtr, idtr;


static uint8_t *page_a;

static int init_test_15() {
	int i;

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
	init_work_mem();
	init_segmentation(gdt, &tss, r3esp, &gdtr);


	// 1 physical page for testing
	page_a = (uint8_t *) 0x300000;
	mem_info.mem_nb_free_pg -= 1;
	mem_info.next_free_pg += 1*PAGE_SIZE;
	for(i = 0; i < PAGE_SIZE; i++) {
	   page_a[i] = 'B';
	}


	return 0;
}


static int test_test_15() {
	int i;
	uint32_t port;
	// Test
	// REP OUTS instruction
	for(i=IO_PORT_MIN; i<IO_PORT_MAX; i++) {

		if(i==0x2fb			// Vmw : nothing (but uart com2 stopped)					Ram : unsupported uart operation ! 	VBox : nothing
		|| i==0x3fd			// Vmw : nothing											Ram : unsupported uart operation !	VBox : nothing
		|| i==0x102c		// Vmw : NOT_REACHED bora/devices/chipset/piix4PM.c:1982	Ram : no control (vmware crash)		VBox : nothing
		|| i==0x102d		// Vmw : NOT_REACHED bora/devices/chipset/piix4PM.c:1982	Ram : no control (vmware crash)		VBox : nothing
		|| i==0x102e		// Vmw : NOT_REACHED bora/devices/chipset/piix4PM.c:1982	Ram : no control (vmware crash)		VBox : nothing
		|| i==0x2000		// Vmw : NOT_IMPLEMENTED bora/devices/e1000/e1000.c:2752	Ram : no control (vmware crash)		VBox : nothing
		|| i==0x2040 		// Vmw : NOT_IMPLEMENTED bora/devices/e1000/e1000.c:2752	Ram : no control (vmware crash)		VBox : nothing
		){
		} else {
		log("0x%x",i);
		port = i;
		asm volatile (  "mov %0, %%edx \n"
						"mov $0x300000, %%esi   \n" /* address of buffer */
						"mov $0x1, %%ecx  \n" /* number of byte to read in the buffer */
						"rep outsb (%%esi), %%dx  \n"
						::"m"(port):);
		log(".");
		}
	}
	printf_log("\nrep outsb done\n");


	return 0;
}

static int restore_test_15() {
	// Restore
	return 0;
}

test_t test_15 = {
	.name = "Test 15 : REP OUTS fuzzer - list of fails - value 'B'",
	.desc = "- privilege level : ring 0"
	"- instruction     : REP OUTS"
	"- port range      : [0x0000 - 0xffff]"
	"- buffer addr     : 0x300000 (no paging)"
	"- buffer size     : 1"
	"- buffer content  : 'B'" ,
	.init = init_test_15,
	.test = test_test_15,
	.fini = restore_test_15,
};

DECLARE_TEST(test_15)


