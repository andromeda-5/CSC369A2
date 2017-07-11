#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand;
/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	int evict_frame;

	//wait until we have a page to kick out
	while(1) { 
		pgtbl_entry_t *pte = coremap[clock_hand].pte;
		if (pte->frame & PG_REF) {
			pte->frame &= ~PG_REF;
			clock_hand++;
			if (clock_hand == memsize){
				clock_hand = 0;
			}
		} else {
			evict_frame = clock_hand;
			break; 
		}
	}
	return evict_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	p->frame |= PG_REF;
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	clock_hand = 0;
}
