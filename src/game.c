#include "game.h"

typedef struct GameState{
  bool32 initialized;
  i32 playerX;
  i32 playerY;
}GameState;


void game_update_render(Backbuffer backbuffer, Keyboard keyboard, PlatformProcs procs, Memory* permanentMemory, Memory* temporaryMemory)
{
  GameState* gameState = (GameState*)permanentMemory->data;
  if (!gameState->initialized)
  {
    // init
    gameState->playerX = 2;
    gameState->playerY = 3;
    gameState->initialized = TRUE;
  }
  if (keyboard.key[KEY_RIGHT])
  {
    gameState->playerX += 1;
  }
  if (keyboard.key[KEY_LEFT])
  {
    gameState->playerX -= 1;
  }
  if (keyboard.key[KEY_UP])
  {
    gameState->playerY -= 1;
  }
  if (keyboard.key[KEY_DOWN])
  {
    gameState->playerY += 1;
  }
  fill_backbuffer(backbuffer);
  draw_rectangle(&backbuffer, gameState->playerX * 100, gameState->playerY * 100, 100, 100, 0xff, 0, 0, 0xff);
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

void fill_backbuffer(Backbuffer backbuffer)
{
  u8* line = backbuffer.buffer;
  for (u32 y = 0;y < backbuffer.lineCount;++y)
  {
    u8* pixelByte = line;
    for (u32 x = 0;x < backbuffer.pixelsPerLine;++x)
    {
      // NOTE: still assuming 4 bytes per pixel
      u32* pixel = (u32*)pixelByte;
      // fill the color
      *pixel |= (0x18 << backbuffer.blueShift);
      *pixel |= (0x18 << backbuffer.greenShift);
      *pixel |= (0x18 << backbuffer.redShift);
      pixelByte += backbuffer.bytesPerPixel;
    }
    line += backbuffer.pitch;
  }
}
