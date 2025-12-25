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

#define UNUSED(var) (void)(var)

#define Kilobytes(n) (n*1024LL)
#define Megabytes(n) (Kilobytes(n)*1024LL)
#define Gigabytes(n) (Megabytes(n)*1024LL)

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


#define HORIZONTAL_RESOLUTION (1920 / 2)
#define VERTICAL_RESOLUTION (1080 / 2)

#define TARGET_US_PER_FRAME 30000

#define PERMANENT_MEMORY_SIZE Kilobytes(64LL);
#define TEMPORARY_MEMORY_SIZE Megabytes(12LL);


#define KEYS\
  KEY_DEF(KEY_UP, 1, 0) \
  KEY_DEF(KEY_LEFT, 4, 0) \
  KEY_DEF(KEY_RIGHT, 3, 0) \
  KEY_DEF(KEY_DOWN, 2, 0) \
  KEY_DEF(KEY_CHAR_U, 0, 'u') \
  KEY_DEF(KEY_CHAR_Z, 0, 'z') \
  KEY_DEF(KEY_CHAR_Q, 0, 'q') \
  KEY_DEF(KEY_ENTER, 0, '\r') \

typedef enum KeyKind{
  #define KEY_DEF(enum, s, c) enum,
  KEYS
  #undef KEY_DEF
  __KEY_COUNT
}KeyKind;

u16 keyScanCodes[] = {
  #define KEY_DEF(e, scan, c) scan,
  KEYS
  #undef KEY_DEF
};

u16 keyPrintable[] = {
  #define KEY_DEF(e, s, ch) ch,
  KEYS
  #undef KEY_DEF
};

typedef struct Keyboard{
  bool8 key[__KEY_COUNT];
}Keyboard;

typedef struct PlatformProcs{
  i32 (*debug_printf)(const char*, ...);
}PlatformProcs;

typedef struct Memory{
  void* data;
  usize size;
}Memory;


#define GAME_UPDATE_RENDER(name) void name(Backbuffer* backbuffer, Keyboard keyboard, PlatformProcs procs, Memory* permanentMemory, Memory* temporaryMemory, f32 dt)
typedef GAME_UPDATE_RENDER(GameUpdateRender);
GAME_UPDATE_RENDER(game_update_render);


void fill_backbuffer(Backbuffer backbuffer);
void draw_rectangle(Backbuffer* backbuffer, i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b, u8 a);
void clear_background(Backbuffer* backbuffer, u8 r, u8 g, u8 b);

#endif
