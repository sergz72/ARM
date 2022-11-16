#ifndef _HEAP_H
#define _HEAP_H

struct _HeapInfo;

typedef struct _HeapBlockHeader {
  struct _HeapBlockHeader *prev, *next;
  unsigned int size;
} HeapBlockHeader;

typedef struct {
  HeapBlockHeader *heap_base;
  unsigned int heap_size;
} HeapSpace;

typedef struct _HeapInfo {
  HeapSpace *heap_spaces;
  HeapBlockHeader *free_blocks, *used_blocks;
  unsigned int heap_space_count, heap_size, free_size, used_size;
} HeapInfo;

void Heap_Init(HeapInfo *hi);
void *Heap_Alloc(HeapInfo *hi, unsigned int size);
void Heap_Free(HeapInfo *hi, void *p);
unsigned int Heap_GetSize(void *p);
#define Heap_GetFree(hi) (hi)->free_size

#ifdef HEAP_TEST
void Heap_Test(HeapInfo *hi);
#endif

#ifdef HEAP_CHECK
void Heap_Check(HeapInfo *hi);
#endif

#endif
