#include "game.h"

#include "../meta/roboto.h"
#include <stdint.h>
#include <stdarg.h>

#ifndef ARENA_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#include "arena.h"
#endif

#ifndef STB_SPRINTF_IMPLEMENTATION
#  define STB_SPRINTF_IMPLEMENTATION
#include "thirdparty/stb/stb_sprintf.h"
#endif

internal const Direction directions[] = {
  (Direction){.x =  0, .y = -1},
  (Direction){.x = -1, .y =  0},
  (Direction){.x =  1, .y =  0},
  (Direction){.x =  0, .y =  1},
};

internal PlatformProcs Gprocs = {0};
internal Arena Gtemp = {0};


void *memcpy(void *dest, const void *src, size_t n)
{
  for (size_t i = 0; i < n; i++)
  {
    ((char*)dest)[i] = ((char*)src)[i];
  }
  return dest;
}

char* tprintf(const char* fmt, ...)
{
  i32 count = 0;
  va_list va;
  va_start(va, fmt);
  char base[1024];
  // HACK: stb_sprintf.h has custom callbacks maybe use that
  count = stbsp_vsnprintf(base, 1024, fmt, va);
  void* ptr = arena_alloc(&Gtemp, count);
  memcpy(ptr, base, count);
  return ptr;
}

void game_update_level(GameState* state, Keyboard* keys)
{
  for (i32 dir = 0;dir < 4;++dir)
  {
    if (keys->key[dir])
    {
      state->playerX += directions[dir].x;
      state->playerY += directions[dir].y;
      break;
    }
  }
}

void game_update(GameState* state, Keyboard* keyboard)
{
  switch (state->gameSceneEnum)
  {
    case SCENE_LEVEL:
      {
        game_update_level(state, keyboard);
        break;
      }
    case SCENE_MENU:
      {
        // TODO:
        break;
      }
  }
}

void tile_draw(Backbuffer* backbuffer, i32 x, i32 y, u8 r, u8 g, u8 b, u8 a)
{
  i32 pixelX = x * TILE_WIDTH_PIXELS;
  i32 pixelY = y * TILE_HEIGHT_PIXELS;
  draw_rectangle(backbuffer, pixelX, pixelY, TILE_WIDTH_PIXELS, TILE_HEIGHT_PIXELS, r, g, b, a);
}

size_t strlen(const char* str)
{
  char* ptr = (char*)str;
  size_t len = 0;
  while(*ptr != 0)
  {
    len += 1;
    ptr += 1;
  }
  return len;
}

DEBUG_FONT_DRAW(debug_font_draw)
{
  size_t len = strlen(str);
  for (size_t i = 0;i < len;++i)
  {
    CharData cdata = roboto_cdata[str[i] - roboto_START_CHAR];
    for (i32 dy = cdata.y0;dy < cdata.y1;++dy)
    {
      i32 y = (i32)penY + dy - cdata.y0 + cdata.yoff;
      if (y >= 0 && y < (i32)backbuffer->lineCount)
      {
        for (i32 dx = cdata.x0;dx < cdata.x1;++dx)
        {
          i32 x = (i32)penX + dx - cdata.x0 + cdata.xoff;
          u32 intensity = roboto_pixels[dy * roboto_WIDTH + dx];
          i32 index = y * backbuffer->pixelsPerLine + x;
          if (x < (i32)backbuffer->pixelsPerLine && x >= 0)
          {
            u32* pixel = (u32*)backbuffer->buffer + index;
            u32 currB = ((*pixel >> backbuffer->blueShift) & 0x000000ff);
            u32 currR = ((*pixel >> backbuffer->redShift) & 0x000000ff);
            u32 currG = ((*pixel >> backbuffer->greenShift) & 0x000000ff);
            *pixel = 0;
            *pixel |= ((currB * (255 - intensity) + b * intensity)/255) << backbuffer->blueShift;
            *pixel |= ((currG * (255 - intensity) + g * intensity)/255) << backbuffer->greenShift;
            *pixel |= ((currR * (255 - intensity) + r * intensity)/255) << backbuffer->redShift;
          }
      }
      }
    }
    penX += cdata.xadvance;
  }
}

void game_draw(Backbuffer* backbuffer, GameState* state)
{
  clear_background(backbuffer, 0, 0, 0);
  for (i32 y = 0;y < TILE_COUNT_HEIGHT;++y)
  {
    for (i32 x = 0;x < TILE_COUNT_WIDTH;++x)
    {
      i32 pixelX = x * TILE_WIDTH_PIXELS;
      i32 pixelY = y * TILE_HEIGHT_PIXELS;
      i32 cord = x + y * TILE_COUNT_WIDTH;
      switch (state->tiles[cord].tileKindEnum)
      {
        case TILE_NORMAL:
          {
            //tile_draw(backbuffer, 1, 1, 0x18, 0x18, 0x18, 0xff);
            draw_rectangle(backbuffer, pixelX+1, pixelY+1, TILE_WIDTH_PIXELS-1, TILE_HEIGHT_PIXELS-1, 0, 0x88, 0x88, 0xff);
            break;
          }
        case TILE_WALL:
          {
            tile_draw(backbuffer, x, y, 0xff, 0, 0, 0xff);
            break;
          }
      }
    }
  }
  draw_rectangle(backbuffer, state->playerX * TILE_WIDTH_PIXELS+1, state->playerY * TILE_HEIGHT_PIXELS+1, TILE_WIDTH_PIXELS-1, TILE_HEIGHT_PIXELS-1, 0, 0xee, 0xee, 0xff);
}


// ENTRY POINT
GAME_UPDATE_RENDER(game_update_render)
{
  UNUSED(temporaryMemory);
  UNUSED(dt);
  if (sizeof(GameState) > permanentMemory->size)
  {
    // TODO: panic
    procs.debug_printf("Exceeded the permanent memory limit!\n");
    return;
  }
  Gprocs = procs;
  Gtemp = (Arena){
    .buffer = temporaryMemory->data,
    .reserved = temporaryMemory->size,
    .commited = 0,
  };
  GameState* gameState = (GameState*)permanentMemory->data;
  if (!gameState->initialized)
  {
    // init
    gameState->playerX = 2;
    gameState->playerY = 3;
    gameState->totalSeconds = 0;
    for (i32 y = 0;y < TILE_COUNT_HEIGHT;++y)
    {
      for (i32 x = 0;x < TILE_COUNT_WIDTH;++x)
      {
        i32 cord = x + y * TILE_COUNT_WIDTH;
        if (y == 0 || y == TILE_COUNT_HEIGHT-1)
        {
          gameState->tiles[cord] = (Tile){.tileKindEnum = TILE_WALL};
        }
        else
        {
          gameState->tiles[cord] = (Tile){.tileKindEnum = TILE_NORMAL};
        }
      }
    }

    gameState->initialized = TRUE;
  }
  game_update(gameState, &keyboard);
  game_draw(backbuffer, gameState);
  gameState->totalSeconds += dt;
  debug_font_draw(backbuffer, tprintf("Seconds %.2f", gameState->totalSeconds), 300.0f, 100.0f, 0x0, 0, 0);
}

void draw_rectangle(Backbuffer* backbuffer, i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b, u8 a)
{
  u8* line = (u8*)backbuffer->buffer + (y * backbuffer->pixelsPerLine * backbuffer->bytesPerPixel);
  for (i32 ry = 0;ry < h;++ry)
  {
    if (y+ry <= (i32)backbuffer->lineCount && y+ry > 0)
    {
      u8* pixelByte = line + (x * backbuffer->bytesPerPixel);
      for (i32 rx = 0;rx < w;++rx)
      {
        if (x+rx <= (i32)backbuffer->pixelsPerLine && x+rx > 0)
        {
          u32* pixel = (u32*)pixelByte;
          *pixel = 0;
          *pixel |= ((u32)b << backbuffer->blueShift);
          *pixel |= ((u32)g << backbuffer->greenShift);
          *pixel |= ((u32)r << backbuffer->redShift);
          *pixel |= ((u32)a << backbuffer->alphaShift);
        }
        pixelByte += backbuffer->bytesPerPixel;
      }
    }
    line += backbuffer->pitch;
  }
}

void clear_background(Backbuffer* backbuffer, u8 r, u8 g, u8 b)
{
  u8* line = backbuffer->buffer;
  for (u32 y = 0;y < backbuffer->lineCount;++y)
  {
    u8* pixelByte = line;
    for (u32 x = 0;x < backbuffer->pixelsPerLine;++x)
    {
      // NOTE: still assuming 4 bytes per pixel
      u32* pixel = (u32*)pixelByte;
      // fill the color
      *pixel |= ((u32)b << backbuffer->blueShift);
      *pixel |= ((u32)g << backbuffer->greenShift);
      *pixel |= ((u32)r << backbuffer->redShift);
      pixelByte += backbuffer->bytesPerPixel;
    }
    line += backbuffer->pitch;
  }
}
