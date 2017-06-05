// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/start.h>
#include <core/setup.h>
#include <core/test.h>
#include <core/serial_driver.h>
#include <core/video.h>
#include <core/print.h>
#include <core/utils.h>

mbi_t *__mbi;
mem_info_t mem_info;
uint32_t __multiboot_header__ multiboot_header_magic = MBH_MAGIC;
uint32_t __multiboot_header__ multiboot_header_flags = MBH_FLAGS;
int32_t __multiboot_header__ multiboot_header_chksm = -(MBH_MAGIC+MBH_FLAGS);

extern uint32_t __tests_start__;
extern uint32_t __tests_end__;
extern uint32_t __tests_init_start__;
extern uint32_t __tests_init_end__;
extern uint32_t __tests_restore_start__;
extern uint32_t __tests_restore_end__;

void __attribute__((regparm(1))) start(mbi_t *mbi)
{
        //sleep();
        __mbi = mbi;
        setup();

        //asm volatile ("1: jmp 1b"); //jump eip : boucle infinie

        tests();
        printf("\n-= o00oO =- Crash OS halted.");
}


void tests() 
{
        loc_t cursor = {.linear = (uint32_t)&__tests_start__};
        loc_t stop = {.linear = (uint32_t)&__tests_end__};

	int err;
	
	while (cursor.linear < stop.linear)
	{
		test_t *test = (test_t *)*cursor.u32;
		printf("-======< [%s] >======-\n", test->name);
		if (test->init) {
			printf(" + Running init ...");
			err = test->init();
			if (err != 0) {
				printf("KO (error=%i). Aborting.\n", err);
				return;
			}
			printf("ok.\n");
		}
		if (test->test) {
			printf(" + Running test\n");
			err = test->test();
			if (err) {
				printf(" + FAILED (error=%i) <==========\n", err);
			}
			else {
				printf(" + passed\n");
			}
		}
		if (test->fini) {
			printf(" + Running fini ...");
			err = test->fini();
			if (err) {
				printf("KO (error=%i). Aborting.\n", err);
				return;
			}
			printf("ok.\n");
		}
		cursor.u32++;

		
	}
}
