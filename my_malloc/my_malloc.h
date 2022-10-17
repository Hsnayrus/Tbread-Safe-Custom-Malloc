#include <stdio.h>
#include <unistd.h>

typedef struct My_malloc {
  struct My_malloc * next;
  struct My_malloc * prev;
  size_t blockSize;
} mmalloc;

void * reusebf_malloc(size_t noBytes);
void * splitBlocks(void * ptrToPurify, size_t noBytes);
void * bf_malloc(size_t noBytes);
void tryMerge(void * ptrVoid);
/* size_t tryMerge(mmalloc * ptrToMerge); */

void * ts_malloc_lock(size_t noBytes);
void ts_free_lock(void * ptr);
void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void * ptr);

void bf_free(void * ptrToFree);
void printLLFront();
void printLLBack();
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

/* //This function is the substitute of malloc. Probably FF */
/* void * bf_malloc(size_t noBytes); */
/* //This function was purely made for testing purposes. */
/* void printLLFront(mmalloc * headPtr); */
/* //This function is the function used to free a memory region */
/* void bf_free(void * ptrToDelete); */
/* //This function is also for testing */
/* void printLLBack(mmalloc * tailPtr); */
/* //helper function for adding to free linked linst */
/* void addSortedFree(mmalloc * ptrToAdd); */
/* //Merges adjacent free regions */
/* size_t mergeFreeList(mmalloc * newNode); */
/* unsigned long get_data_segment_size(); */
/* unsigned long get_data_segment_free_space_size(); */
/* void * reusebf_malloc(size_t noBytes); */
/* void addSortedFree(mmalloc * ptrToAdd); */
/* void printLLFront(mmalloc * headPtr); */
/* void printLLBack(mmalloc * tailPtr); */
/* void purify(mmalloc * result); */
/* void addToMalloc(mmalloc * newNode); */
/* void * ff_malloc(size_t noBytes); */
/* void ff_free(void * newPtr); */
