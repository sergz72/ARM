#include "board.h"
#include <heap.h>
#include <stdlib.h>

void HeapInfo_Init(HeapInfo *hi, HeapSpace *spaces, void *start, void *end)
{
  spaces->heap_base = start;
  spaces->heap_size = end - start;
  hi->heap_spaces = spaces;
  hi->heap_space_count = 1;
}

int Heap_Init(HeapInfo *hi)
{
  unsigned int i;
  HeapSpace *hs;

  if (!hi)
    return 1;

  hs = hi->heap_spaces;

  if (!hi->heap_space_count || !hs || !hs->heap_base)
    return 2;

  hi->free_blocks = hs->heap_base;
  hi->used_blocks = NULL;
  hi->used_size = hi->heap_size = hi->free_size = 0;
  for (i = 0; i < hi->heap_space_count; i++)
  {
    if (!hs->heap_base || !hs->heap_size)
      return 3;
    hi->heap_size += hs->heap_size;
    hi->free_size += hs->heap_size - sizeof(HeapBlockHeader);
    if (i)
    {
      hs->heap_base->prev = hi->heap_spaces[i - 1].heap_base;
      hi->heap_spaces[i - 1].heap_base->next = hs->heap_base;
    }
    else
      hs->heap_base->prev = NULL;
    hs->heap_base->next = NULL;
    hs->heap_base->size = hs->heap_size / sizeof(HeapBlockHeader) - 1;
    hs++;
  }

  return 0;
}

void *Heap_Alloc(HeapInfo *hi, unsigned int size)
{
  HeapBlockHeader *h, *h1;
  
  if (size == 0)
    return (void*)0xFFFFFFF0;

  size /= sizeof(HeapBlockHeader);
  size++;

  h = hi->free_blocks;

  while (h)
  {
    if (h->size >= size)
    {
      if (h->size - size <= 1)
      {
        if (!h->next && !h->prev)
          hi->free_blocks = NULL;
        else
        {
          if (h->next)
            h->next->prev = h->prev;
          if (h->prev)
            h->prev->next = h->next;
          else
            hi->free_blocks = h->next;
        }

        size = h->size * sizeof(HeapBlockHeader);
        hi->free_size -= size;
      }
      else
      {
        // create a new used block
        h->size -= size+1;
        h1 = h + h->size + 1;
        h1->size = size;
        h = h1;

        size *= sizeof(HeapBlockHeader);
        hi->free_size -= size + sizeof(HeapBlockHeader);
      }

      if (hi->used_blocks)
        hi->used_blocks->prev = h;
      h->prev = NULL;
      h->next = hi->used_blocks;
      hi->used_blocks = h;

#ifdef HEAP_CHECK
      Heap_Check(hi);
#endif

      hi->used_size += size;
      return (void*)(hi->used_blocks + 1);
    }

    h = h->next;
  }

  return NULL;
}

unsigned int Heap_GetSize(void *p)
{
  HeapBlockHeader *hb = (HeapBlockHeader*)p;
  return (--hb)->size * sizeof(HeapBlockHeader);
}

static int Heap_Merge(HeapInfo *hi, HeapBlockHeader *hb)
{
  if (hb->next && hb->next == hb + hb->size + 1)
  {
    hb->size += hb->next->size + 1;
    hi->free_size += sizeof(HeapBlockHeader);
    if (hb->next->next)
    {
      hb->next->next->prev = hb;
      hb->next = hb->next->next;
      Heap_Merge(hi, hb);
    }
    else
      hb->next = NULL;

    return 1;
  }

  return 0;
}

static int Heap_AddressWithinHeap(HeapInfo *hi, HeapBlockHeader *address)
{
  unsigned int i;
  HeapSpace *hs = hi->heap_spaces;

  for (i = 0; i < hi->heap_space_count; i++)
  {
    if (address >= hs->heap_base && address <= (HeapBlockHeader*)((char*)hs->heap_base + hs->heap_size - sizeof(HeapBlockHeader)))
      return 1;
    hs++;
  }

  return 0;
}

void Heap_Free(HeapInfo *hi, void *p)
{
#ifdef HEAP_CHECK
  int ok = 0;
#endif
  HeapBlockHeader *hb = (HeapBlockHeader*)p, *hb2;
  hb--;

#ifdef HEAP_CHECK
  if (((unsigned int)hb & 0xFFFF) == 0x1cb0)
  {
    PRINT_Log("1cb0\n");
  }

  hb2 = hi->used_blocks;
  while (hb2)
  {
    if (hb2 == hb)
    {
      ok = 1;
      break;
    }
    hb2 = hb2->next;
  }
  if (!ok)
    PRINT_Log("Fatal error\n");
#endif

  if (!Heap_AddressWithinHeap(hi, hb))
    return;

  if (hb->next)
    hb->next->prev = hb->prev;
  if (hb->prev)
    hb->prev->next = hb->next;
  else
    hi->used_blocks = hb->next;

  hi->free_size += hb->size * sizeof(HeapBlockHeader);

  if (hi->free_blocks)
  {
    hb2 = hi->free_blocks;
    while (hb2 < hb && hb2->next)
      hb2 = hb2->next;
    if (hb2 >= hb)
    {
      hb->next = hb2;
      hb->prev = hb2->prev;
      if (hb2->prev)
        hb2->prev->next = hb;
      else
        hi->free_blocks = hb;
      hb2->prev = hb;
    }
    else
    {
      hb->next = hb2->next;
      hb->prev = hb2;
      if (hb2->next)
        hb2->next->prev = hb;
      hb2->next = hb;
    }

    if (hb->prev)
    {
      if (!Heap_Merge(hi, hb->prev))
        Heap_Merge(hi, hb);
    }
    else
      Heap_Merge(hi, hb);
  }
  else
  {
    hb->prev = hb->next = NULL;
    hi->free_blocks = hb;
  }

#ifdef HEAP_CHECK
  Heap_Check();
#endif
}

#ifdef HEAP_CHECK

void Heap_Check(HeapInfo *hi)
{
  HeapBlockHeader *hb = hi->free_blocks;

//  PRINT_Log("Starting test...\n");

  while (hb)
  {
    if (hb->next && (hb->next == hb->prev))
    {
        PRINT_Log("FATAL ERROR2!!!\n");
        return;
    }

    if (hb->next && hb->next < hb + hb->size + 1)
    {
        PRINT_Log("FATAL ERROR!!!\n");
        return;
    }
    hb = hb->next;
  }

//  PRINT_Log("Test done\n");
}

#endif

#ifdef HEAP_TEST

#define SIZE 8096

void *pointers[SIZE];

void Heap_Test(HeapInfo *hi)
{
  unsigned int i = SIZE, allocated = 0, n, free, j, oom_count = 0;
  void **p = pointers, *pp;
  char *cp;

  while (i--)
    *p++ = NULL;

  PRINT_Log("Starting test...\n");

  free = Heap_GetFree(hi);

  i = 0;
  while (i < SIZE)
  {
    do
    {
      n = rand() % 511;
    } while (n >= hi->heap_size || n == 0);
    pp = Heap_Alloc(hi, 0, n);
    if (pp)
    {
      pointers[i++] = pp;
      cp = (char*)pp;
      for (j = 0; j < n; j++)
        cp[j] = 0;
      allocated += Heap_GetSize(pp);
//      PRINT_Log("i = %d, %d bytes allocated.\n", i, allocated);
      if (allocated > hi->heap_size)
      {
        PRINT_Log("FATAL ERROR!!! allocated = %d\n", allocated);
        return;
      }
    }
    else
    {
      oom_count++;
//      PRINT_Log("Out of memory, %d bytes requested\n", n);
    }

    n = rand() % (SIZE - 1);
    if (n < SIZE && pointers[n])
    {
      allocated -= Heap_GetSize(pointers[n]);
      if (allocated > hi->heap_size)
      {
        PRINT_Log("FATAL ERROR!!! allocated = %d\n", allocated);
        return;
      }
      Heap_Free(hi, pointers[n]);
      pointers[n] = NULL;
//      PRINT_Log("i = %d, %d bytes allocated\n", i, allocated);
    }
  }

  PRINT_Log("Part 1 done...\n");

  for (i = 0; i < SIZE; i++)
  {
    if (pointers[i])
    {
//      if (((unsigned int)pointers[i] & 0xFF00) == 0x8500)
//        PRINT_Log("aaa\n");
      Heap_Free(hi, pointers[i]);
    }
  }

  n = Heap_GetFree(hi);

  PRINT_Log("Initial free: %d, final free: %d, out of memory count = %d.\nDone\n", free, n, oom_count);
}

#endif
