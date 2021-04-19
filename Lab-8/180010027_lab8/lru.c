// Libraries include
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define HIGH 20
#define FREQ 80
#define SAMPLES 20

// PTE Structure
typedef struct pte {
 int id;
 int present;
 struct pte *next;
 struct pte *prev;
} pte;


void init(int *sequence, int refs, int pages) {

int high = (int)(pages*((float)HIGH/100));

 for(int i = 0; i < refs; i++) {
  if(rand()%100 < FREQ) {
   sequence[i] = rand() % high;
   } else {
    sequence[i] = high + rand()%(pages - high);
  }
 }
}

int main(int argc, char *argv[]) {
  
 int refs = 100000;
 int pages = 100;
 
 pte *table = (pte *)malloc(pages*sizeof(pte));
 
 int *sequence = (int*)malloc(refs*sizeof(int));
 
 init(sequence, refs, pages);
 
 printf("# This is a benchmark of random replacement\n");
 printf("# %d page references\n", refs);
 printf("# %d pages \n", pages);
 printf("#\n#\n#frames\tratio\n");
 
 int frames;
 
 int incr = pages/SAMPLES;
 
 for(frames = incr; frames <= pages; frames += incr) {
 
 clear_page_table(table, pages);
 
 int hits = simulate(sequence, table, refs, frames, pages);
 
 float ratio = (float)hits/refs;
 
 printf("%d\t%.2f\n", frames, ratio);
 
 } 
 
 printf("\n");
 
 return 0;

}

void clear_page_table(pte *page_table, int pages) {
for(int i = 0; i < pages; i++) {
 page_table[i].present = 0;
 }
}

int simulate(int *seq, pte *table, int refs, int frms, int pgs) {

int hits = 0;
int allocated = 0;

pte *first = NULL;
pte *last = NULL;

for(int i = 0; i < refs; i++) {
 int next = seq[i];
 pte *entry = &table[next];

	if(entry->present == 1) {
 		hits++;
 
  // Pointers cases of NULL
 	/* unlink entry and place last */ 
 	 if(entry->next != NULL) {
    if(entry->prev == NULL) {
     	first = entry->next;
      entry->next->prev = NULL;
  	}
    if(entry->prev != NULL){
      entry->prev->next = entry->next;
      entry->next->prev = entry->prev;
    }
   	entry->next = NULL;
    last->next = entry;
    entry->prev = last;
   	last = entry;
  }
 
 } else {
  	if(allocated < frms) {
   	allocated++;
   
   	entry->present = 1;
   	entry->prev = last;
   	entry->next = NULL;
   
    // Pointers cases of NULL
   	/* place entry last */
   	 if(last == NULL) {
   	  first = entry;
   	  last = entry;
   	  entry->prev = NULL;
   	  entry->next = NULL;
   	}
     if(last != NULL) {
      entry->next = NULL;
      last->next = entry;
      entry->prev = last;
      last = entry;
     }
  } else {
   pte *evict; 
   
   evict = first;
   first = evict->next;
   first->prev = NULL;
   
   evict->present = 0;
   entry->present = 1;
   
   // Pointers cases of NULL
   /* place entry last*/
   entry->next = NULL;
   last->next = entry;
   entry->prev = last;
   last = entry;
   
  }
 }
}
return hits;

}

