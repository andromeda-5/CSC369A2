#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

unsigned long timestamp;
/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {

	//examine all the frames and kick out the one that's been used least recently
	//start at the beginning
	int frame = 0;
	pgtbl_entry_t *least_frame = coremap[frame].pte;

	int i;
	for (i = 1; i<memsize; i++) {
		pgtbl_entry_t *current_frame = coremap[i].pte;

		if (current_frame->timestamp < least_frame->timestamp) {
			least_frame = current_frame;
			frame = i;
		}

	}
	
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	p->timestamp = timestamp;
	timestamp++;
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	timestamp = 0;
}
