/*
 * page.c
 *
 *  Created on: 18 mai 2016
 *      Author: anais
 */

#include <core/page.h>
#include <core/print.h>

int paging_is_enabled() {
	uint32_t cr0;
		asm volatile ( "mov %%cr0, %%eax \n"
				       "mov %%eax, %0"
				       :
				       :"m"(cr0)
				       :"eax","memory");
		if( (cr0 & 0x80000000) == 0x80000000 ) {
			return 0; //paging enabled
		}
		return 1;
}


int allocate_missing_page(uint32_t addr, mem_info_t* mem_info) { // TODO WARNING : what about the attributes ?
	uint32_t pde_p, pde_ps, ptb_addr, page;
	pde_t *pgd;

	/* Error cases */
	if(mem_info == NULL) {
		printf("allocate_missing_page error : Null Parameter.\n");
		return 1;
	}
	if(paging_is_enabled() == 1) {
		printf("allocate_missing_page error : Paging disabled.\n");
		return 1;
	}
	if (mem_info->next_free_pg == 0  || mem_info->mem_nb_free_pg < 1) {
		printf("allocate_missing_page error : No more physical page of 4kB available.\n");
		return 1;
	}
	if (mem_info->mem_nb_free_pg < PTB_SIZE/PAGE_SIZE) {
		printf("allocate_missing_page error : No more physical page of 1MB available.\n");
		return 1;
	}

	/* Allocate a page and map it in PGD & PTB */
	get_cr3(pgd);
	pde_p = pde(pgd, addr)->p;
	pde_ps = pde(pgd, addr)->ps;
	if ((pde_p == 1) && (pde_ps == 1)) {
		printf("allocate_missing_page error : Must never print that.\n");
		return 1;
	} else if (pde_p == 0) {
		//printf("allocate_missing_page info : Allocate a new ptb.\n");
		ptb_addr = mem_info->next_free_pg;		// Allocate a new ptb
		mem_info->next_free_pg += PTB_SIZE;		// Update mem_info
		mem_info->mem_nb_free_pg -= PTB_SIZE/PAGE_SIZE;
		__pg_set_entry( &(pgd[page_directory_index(addr)]), PG_RW|PG_USR, page_number((uint32_t)ptb_addr)); 		// Update PGD entry
	}
	//printf("allocate_missing_page info : Allocate a new page.\n");
	page = mem_info->next_free_pg; 				// Allocate a new page
	mem_info->next_free_pg += PAGE_SIZE; 		// Update mem_info
	mem_info->mem_nb_free_pg -= 1;
	__pg_set_entry( &(ptb(pde(pgd,addr))[page_table_index(addr)]), PG_RW|PG_USR, page_number((uint32_t)page) );	// Update PTB entry
	return 0;
}
