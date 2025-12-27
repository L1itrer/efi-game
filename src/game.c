#include "game.h"

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
#define TILE_COUNT_HEIGHT 9

#define TILE_WIDTH_PIXELS (HORIZONTAL_RESOLUTION / 16)
#define TILE_HEIGHT_PIXELS (VERTICAL_RESOLUTION / 9)

typedef struct GameState{
  bool32 initialized;
  i32 gameSceneEnum;
  i32 playerX;
  i32 playerY;
  Tile tiles[TILE_COUNT_WIDTH * TILE_COUNT_HEIGHT];
}GameState;

typedef struct Direction{
  i32 x;
  i32 y;
}Direction;

internal const Direction directions[] = {
  (Direction){.x =  0, .y = -1},
  (Direction){.x = -1, .y =  0},
  (Direction){.x =  1, .y =  0},
  (Direction){.x =  0, .y =  1},
};

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
  GameState* gameState = (GameState*)permanentMemory->data;
  if (!gameState->initialized)
  {
    // init
    gameState->playerX = 2;
    gameState->playerY = 3;
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
