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
#define FIXED_UPDATE_RATE_SECONDS 0.0666 //15 times per second

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


#define GAME_UPDATE_RENDER(name) void name(Backbuffer* backbuffer, Keyboard keyboard, PlatformProcs procs, Memory* permanentMemory, Memory* temporaryMemory, f64 dt)
typedef GAME_UPDATE_RENDER(GameUpdateRender);
GAME_UPDATE_RENDER(game_update_render);

#define DEBUG_FONT_DRAW(name) void name(Backbuffer* backbuffer, const char* str, f32 penX, f32 penY, u8 r, u8 g, u8 b)
typedef DEBUG_FONT_DRAW(DebugFontDraw);
DEBUG_FONT_DRAW(debug_font_draw);


void fill_backbuffer(Backbuffer backbuffer);
void draw_rectangle(Backbuffer* backbuffer, i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b, u8 a);
void clear_background(Backbuffer* backbuffer, u8 r, u8 g, u8 b);


typedef enum GameScene{
  SCENE_LEVEL,
  SCENE_MENU,
  __SCENE_COUNT
}GameScene;

typedef enum TileKind{
  TILE_NORMAL,
  TILE_WALL,
  TILE_NORMAL_CORRRECT,
  __TILE_COUNT
}TileKind;

typedef struct Tile{
  i32 tileKindEnum;
  bool32 hasBox;
}Tile;

#define TILE_COUNT_WIDTH 16
#define TILE_COUNT_HEIGHT 8

#define TILE_WIDTH_PIXELS (HORIZONTAL_RESOLUTION / 16)
#define TILE_HEIGHT_PIXELS (VERTICAL_RESOLUTION / 9)

typedef struct GameState{
  f64 fixedUpdateCounter;
  bool32 initialized;
  i32 gameSceneEnum;
  i32 playerX;
  i32 playerY;
  i32 playerAnim;
  f32 totalSeconds;
  Tile tiles[TILE_COUNT_WIDTH * TILE_COUNT_HEIGHT];
  bool32 increasing;
}GameState;

typedef struct Direction{
  i32 x;
  i32 y;
}Direction;


#endif
