#ifndef _HEAP_H
#define _HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _HeapBlockHeader {
  struct _HeapBlockHeader *prev, *next;
  unsigned int size;
} HeapBlockHeader;

typedef struct {
  HeapBlockHeader *heap_base;
  unsigned int heap_size;
} HeapSpace;

typedef struct {
  HeapSpace *heap_spaces;
  HeapBlockHeader *free_blocks, *used_blocks;
  unsigned int heap_space_count, heap_size, free_size, used_size;
} HeapInfo;

void HeapInfo_Init(HeapInfo *hi, HeapSpace *spaces, void *start, void *end);
int Heap_Init(HeapInfo *hi);
void *Heap_Alloc(HeapInfo *hi, unsigned int size);
void Heap_Free(HeapInfo *hi, void *p);
unsigned int Heap_GetSize(void *p);
void Heap_Test(HeapInfo *hi);
void Heap_Check(HeapInfo *hi);

#ifdef __cplusplus
}
#endif

#endif
