#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

#define MAXLINE 256

extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile;
extern pgdir_entry_t *pgdir;
char buffer[MAXLINE];
FILE *f;


/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int victim = 0;
	int furthest_reference = 0;
	char type;
	addr_t vaddr;
	//for each page in physical memory
	int i;
	for (i=0; i<memsize; i++) {
		int lines = 0;
		//read the lines
		while (fgets(buffer, MAXLINE, f)) {
			lines++;
			sscanf(buffer, "%c %lx", &type, &vaddr);
			pgtbl_entry_t *page = (pgtbl_entry_t *)(pgdir[PGDIR_INDEX(vaddr)].pde & PAGE_MASK);
			pgtbl_entry_t *frame = &page[PGTBL_INDEX(vaddr)];
			if (frame == coremap[i].pte) {
				if (furthest_frame < lines) {
					furthest_furthest = lines;
					victim = i;
					break;
				}
			}
		}
	}
	if (victim  == 0) {
		victim = memsize - 1;
	}
	return victim;	
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	//read tracefile
	if ((f = fopen(tracefile, "r")) == NULL) {
		perror("Could not open tracefile");
		exit(1);
	}


}

