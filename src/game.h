#ifndef GAME_H
#define GAME_H
#include <stdint.h>
#include <stddef.h>

// Basic types

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef u8 char8;
typedef u16 char16;

typedef i8 bool8;
typedef i32 bool32;

#define TRUE 1
#define FALSE 0

typedef uintptr_t usize;
typedef ptrdiff_t isize;

typedef float f32;
typedef double f64;

#define global static 
#define internal static 
#define local_persist static 

typedef struct Backbuffer {
  void* buffer;
  u32 pixelsPerLine;
  u32 lineCount;
  u32 pitch; // how many pixels to advance to new line
  u32 bytesPerPixel;
  u32 redShift;
  u32 blueShift;
  u32 greenShift;
  u32 alphaShift;
}Backbuffer;


void fill_backbuffer(Backbuffer backbuffer);

#endif
