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

addr_t *addresses;
addr_t *frames;

int references_so_far;
int num_lines;

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int i, j;
	int eject = 0;

	int max_distances[memsize];

	for (i=0; i<memsize; i++) {
		    
		max_distances[i] = num_lines - references_so_far + 1;
		for (j=0; j<num_lines - references_so_far; j++) {
			if (frames[i] == addresses[references_so_far + j]) {
				if (j < max_distances[i]) {
					max_distances[i] = j;
					break;
				}			
			}
		}

		if (j == num_lines) {
			eject = i;
			return eject;
		}
	}

	for (i=0; i<memsize; i++){
		if (max_distances[i] > max_distances[eject]) {
			eject = i;
		}
	}

	frames[eject] = 0;

	return eject;

}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {

	if (!frames[p->frame >> PAGE_SHIFT]) {
		frames[p->frame >> PAGE_SHIFT] = addresses[references_so_far];
	}

	references_so_far++;

	return;

}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	//read tracefile
	FILE *f;
	if ((f = fopen(tracefile, "r")) == NULL) {
		perror("Could not open tracefile");
		exit(1);
	}

	//count number of lines in tracefile
	char buffer[MAXLINE];
	num_lines = 0;
	while (fgets(buffer, MAXLINE, f)) {
		num_lines++;
	}

	//this is to store all the addresses in the tracefile
	addresses = malloc(num_lines * sizeof(addr_t));

	rewind(f);

	char type;
	addr_t vaddr;
	int i = 0;

	//now store in the addresses array
	while (fgets(buffer, MAXLINE, f)) {
		sscanf(buffer, "%c %lx", &type, &vaddr);
		addresses[i] = vaddr;
		i++;
	}

	//make a copy of all the frames
	frames = malloc(sizeof(addr_t) * memsize);
	for (i=0; i<memsize; i++) {
		frames[i] = 0;
	}

	references_so_far = 0;

	fclose(f);

}

