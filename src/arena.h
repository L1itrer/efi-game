#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// NOTE: "inspired" by https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

typedef struct Arena{
  unsigned char* buffer;
  size_t reserved;
  size_t commited;
}Arena;


#ifndef DEFAULT_ALIGNMENT
#  define DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif

void* arena_alloc(Arena* arena, size_t size);
void* arena_alloc_align(Arena* arena, size_t size, size_t alignment);

#define ArenaPushStruct(a, s) arena_alloc(arena, sizeof(s))
#define ArenaPushArray(a, t, c) arena_alloc(arena, sizeof(t) * c)



#endif

#ifdef ARENA_IMPLEMENTATION

bool arena_is_power_of_two(uintptr_t n)
{
  return (n & (n-1)) == 0;
}

uintptr_t arena_align_forward(uintptr_t ptr, size_t alignment)
{
  uintptr_t newPtr, a, modulo;
  //assert(arena_is_power_of_two(alignment));

  newPtr = ptr;
  a = (uintptr_t)alignment;
  modulo = newPtr & (a-1);
  
  if (modulo != 0)
  {
    newPtr += a - modulo;
  }
  return newPtr;
}

void* arena_alloc_align(Arena* arena, size_t size, size_t alignment)
{
  uintptr_t currPtr = (uintptr_t)arena->buffer + (uintptr_t)arena->commited;
  uintptr_t offset = arena_align_forward(currPtr, alignment);
  offset -= (uintptr_t)arena->buffer; // calculate relative offset

  // do we still have enough space?
  if (offset + size <= arena->reserved)
  {
    void* ptr = &arena->buffer[offset];
    arena->commited = offset + size;

    // could zero the memory here?

    return ptr;
  }
  // return null if out of memory
  return NULL;
}


void* arena_alloc(Arena* arena, size_t size)
{
  return arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
}


#endif
