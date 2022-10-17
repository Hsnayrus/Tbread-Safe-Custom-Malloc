#include "my_malloc.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

size_t allocatedBytes = 0;
size_t freeBytes = 0;
size_t maxFreeBlock = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
mmalloc * fHead = NULL;
mmalloc * fTail = NULL;

unsigned long get_data_segment_size() {
  return allocatedBytes;
}

unsigned long get_data_segment_free_space_size() {
  return freeBytes;
}

void * ts_malloc_nolock(size_t noBytes) {
  return ts_malloc_lock(noBytes);
}

void ts_free_nolock(void * ptr) {
  ts_free_lock(ptr);
}

/*
This function determines whether a block from the freeList can be used or not.
It assumes that the pointer in the free linked list is one with metadata attached to it.
If it can then it returns the original pointer with the metadata informati
on
the metadata size is to be calculate in the bf_malloc function.
 */

void * reusebf_malloc(size_t noBytes) {
  size_t valMmalloc = sizeof(mmalloc);
  mmalloc * temp = fHead;
  mmalloc * result = NULL;
  size_t value = 0;
  while (temp != NULL) {
    if ((temp->blockSize - valMmalloc) > noBytes) {
      value = temp->blockSize;
      if ((result == NULL) || (value < result->blockSize)) {
        result = temp;
      }
    }
    temp = temp->next;
  }
  void * resultVoid = (void *)result;
  return resultVoid;
}

/*
  This method basically clears ensures proper removal of this pointer 
  and splits the memory block if necessary
*/
void * splitBlocks(void * pTPVoid, size_t noBytes) {
  mmalloc * ptrToPurify = (mmalloc *)pTPVoid;
  mmalloc * temp = NULL;
  void * tempVoid = NULL;
  if ((ptrToPurify->blockSize - (2 * sizeof(mmalloc)) > (noBytes))) {
    tempVoid = (void *)(pTPVoid + (ptrToPurify->blockSize - noBytes - sizeof(mmalloc)));
    temp = (mmalloc *)tempVoid;
    temp->next = NULL;
    temp->prev = NULL;
    temp->blockSize = sizeof(mmalloc) + noBytes;
    ptrToPurify->blockSize -= (temp->blockSize);
  }
  return tempVoid;
}

void removePointer(mmalloc * ptr) {
  if (fHead == ptr) {
    fHead = ptr->next;
  }
  else {
    if (ptr->prev != NULL) {
      ptr->prev->next == ptr->next;
    }
    if (ptr->next != NULL) {
      ptr->next->prev = ptr->prev;
    }
  }
  ptr->next = NULL;
  ptr->prev = NULL;
}

void * ts_malloc_lock(size_t noBytes) {
  pthread_mutex_lock(&lock);
  void * result = bf_malloc(noBytes);
  pthread_mutex_unlock(&lock);
  return result;
}

void ts_free_lock(void * ptr) {
  pthread_mutex_lock(&lock);
  bf_free(ptr);
  pthread_mutex_unlock(&lock);
}

/*
This function returns either a new block if the noBytes requested
is less than the freeBlocks
Else it creates a new block by sbrk system call
The data address after incrementing it with sizeof(mmalloc) is returned
*/

void * bf_malloc(size_t noBytes) {
  if (noBytes < maxFreeBlock) {
    void * result = reusebf_malloc(noBytes);
    mmalloc * resultM = (mmalloc *)result;
    void * actualResult = splitBlocks(resultM, noBytes);
    if (actualResult != NULL) {
      mmalloc * aCRM = (mmalloc *)actualResult;
      freeBytes -= aCRM->blockSize;
      allocatedBytes += aCRM->blockSize;
      aCRM->next = NULL;
      aCRM->prev = NULL;
      return (void *)(actualResult + sizeof(mmalloc));
    }
  }
  mmalloc * newNode = (mmalloc *)(sbrk(noBytes + sizeof(mmalloc)));
  newNode->blockSize = noBytes + sizeof(mmalloc);
  allocatedBytes += noBytes;
  void * newNodeVoid = (void *)newNode;
  return (newNodeVoid + sizeof(mmalloc));
}
/*
  This function is supposed to merge the pointer that is to
  be added to the linked list with any adjacent pointers that 
  are already present in the free linked list.
  This function assumes that the parameter contains the metadata
  in the block
*/

void tryMerge(void * ptrVoid) {
  mmalloc * ptrM = (mmalloc *)ptrVoid;
  if (ptrM->next != NULL) {
    void * tempNextVoid = (void *)(ptrM->next);
    if (ptrVoid + ptrM->blockSize == tempNextVoid) {
      ptrM->blockSize += (ptrM->next)->blockSize;
      ptrM->next = ptrM->next->next;
    }
  }
}

/*
  This function is responsible for adding the paramter to the free 
  linked list.
  It assumes that the parameter's address starts after the area 
  occupied in this block by the metadata
*/

void bf_free(void * ptrVoidWD) {
  /* void * ptrVoid = (void *)(ptrVoidWD - sizeof(mmalloc)); */
  /* mmalloc * ptr = (mmalloc *)ptrVoid; */
  /* void * headVoid = (void *)fHead; */
  /* void * tailVoid = (void *)fTail; */
  /* freeBytes += ptr->blockSize; */

  /* if (fHead == NULL) { */
  /*   assert(fTail == NULL); */
  /*   fHead = ptr; */
  /*   fTail = ptr; */
  /*   ptr->prev = NULL; */
  /*   ptr->next = NULL; */
  /*   tryMerge(ptrVoid); */
  /*   printf("returning1\n"); */
  /*   return; */
  /* } */
  /* if (ptrVoid < headVoid) { */
  /*   ptr->prev = NULL; */
  /*   fHead->prev = ptr; */
  /*   ptr->next = fHead; */
  /*   fHead = ptr; */
  /*   tryMerge(ptrVoid); */
  /*   printf("returning2\n"); */
  /*   return; */
  /* } */
  /* if (ptrVoid > tailVoid) { */
  /*   ptr->next = NULL; */
  /*   ptr->prev = fTail; */
  /*   fTail->next = ptr; */
  /*   fTail = ptr; */
  /*   tryMerge(ptrVoid); */
  /*   printf("returnin3g\n"); */
  /*   return; */
  /* } */
  /* printf("Coming here"); */
  /* mmalloc * temp = fHead->next; */
  /* while ((((void *)temp) < ptrVoid) && (temp->next != NULL)) { */
  /*   temp = temp->next; */
  /* } */

  /* ptr->next = temp->next; */
  /* temp->next->prev = ptr; */
  /* temp->next = ptr; */
  /* ptr->prev = temp; */
  /* printf("returning4\n"); */
  /* tryMerge(ptrVoid); */
  void * ptrVoid = ptrVoidWD - sizeof(mmalloc);
  mmalloc * temp = fHead;
  mmalloc * ptrToFree = (mmalloc *)ptrVoid;
  if (fHead == NULL) {
    fHead = ptrToFree;
    ptrToFree->next = NULL;
    ptrToFree->prev = NULL;
  }

  else if (ptrToFree < fHead) {
    ptrToFree->next = fHead;
    fHead = ptrToFree;
    ptrToFree->next->prev = ptrToFree;
    ptrToFree->prev = NULL;
  }

  else {
    while (temp->next != NULL && temp->next < ptrToFree) {
      temp = temp->next;
    }
    ptrToFree->next = temp->next;
    if (temp->next != NULL) {
      ptrToFree->next->prev = ptrToFree;
    }
    temp->next = ptrToFree;
    ptrToFree->prev = temp;
  }
  if (ptrToFree->blockSize > maxFreeBlock) {
    maxFreeBlock = ptrToFree->blockSize;
  }
  freeBytes += ptrToFree->blockSize;
  tryMerge(ptrVoid);
}
/* //Decrement size so that it contains metadata */
/* } */

/*
  This function was purely made for testing purposes.
  It will print the blocksizes and addresses of the 
  free linked list starting from the head pointer.
*/

void printLLFront() {
  mmalloc * temp = fHead;
  printf("Head = %p\n", fHead);
  while (temp != NULL) {
    printf("**********%p, %lu**********\n", temp, temp->blockSize);
    temp = temp->next;
  }
}

/*
  This function was purely made for testing purposes.
  It will print the blocksizes and addresses of the 
  free linked list starting from the tail pointer.
*/

void printLLBack() {
  mmalloc * temp = fTail;
  printf("Tail = %p\n", fTail);
  while (temp != NULL) {
    printf("**********%p, %lu**********\n", temp, temp->blockSize);
    temp = temp->prev;
  }
}

int main() {
  int ** newPtr = (int **)bf_malloc(10 * sizeof(int *));
  for (int i = 0; i < 10; i++) {
    newPtr[i] = (int *)bf_malloc(400);
  }
  printf("Will free 024689\n");
  bf_free(newPtr[1]);
  bf_free(newPtr[0]);
  bf_free(newPtr[5]);
  bf_free(newPtr[4]);
  bf_free(newPtr[7]);
  bf_free(newPtr[9]);

  printLLFront();
  printLLBack();
  return 0;

  newPtr[0] = (int *)bf_malloc(400);
  printLLBack();
  printLLFront();
  bf_free(newPtr[0]);
  printLLFront();
  printLLBack();
  printf("%lu\n", maxFreeBlock);
}
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */

/*   /\* newPtr[3] = (int *)bf_malloc(700); *\/ */
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */

/*   /\* bf_free(newPtr[3]); *\/ */
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */

/*   /\* newPtr[3] = (int *)bf_malloc(700); *\/ */
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */

/*   /\* newPtr[0] = (int *)bf_malloc(99); *\/ */
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */

/*   /\* bf_free(newPtr[3]); *\/ */
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */

/*   /\* bf_free(newPtr[0]); *\/ */
/*   /\* printf("\n\t\tAnotherone\n"); *\/ */
/*   /\* printLLFront(); *\/ */
/* } */
