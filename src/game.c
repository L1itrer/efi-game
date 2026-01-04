#include "game.h"

#include "../meta/roboto.h"
#include "../meta/roboto_small.h"
#include <stdint.h>
#include <stdarg.h>

#ifndef ARENA_IMPLEMENTATION
#  define ARENA_IMPLEMENTATION
#  include "arena.h"
#endif

#ifndef STB_SPRINTF_IMPLEMENTATION
#  define STB_SPRINTF_IMPLEMENTATION
#  include "thirdparty/stb/stb_sprintf.h"
#endif

#include "levels.c"

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

bool32 xy_within_bounds(i32 x, i32 y)
{
  return x >= 0 && x < TILE_COUNT_WIDTH && y >= 0 && y < TILE_COUNT_HEIGHT;
}

void player_move(GameState* state, Direction direction)
{
  GameLevel* level = &state->level;
  i32 requestedX = level->playerX + direction.x;
  i32 requestedY = level->playerY + direction.y;
  // if is in bounds
  if (xy_within_bounds(requestedX, requestedY))
  {
    Tile requestedTile = level->tiles[requestedY * TILE_COUNT_WIDTH + requestedX];
    if (requestedTile.tileKindEnum != TILE_WALL)
    {
      if (!requestedTile.hasBox)
      {
        // allowed to move
        level->playerX += direction.x;
        level->playerY += direction.y;
      }
      else
      {
        i32 nextX1 = requestedX+direction.x;
        i32 nextY1 = requestedY+direction.y;
        if (xy_within_bounds(nextX1, nextY1))
        {
          Tile* boxToMove1 = &level->tiles[(nextY1) * TILE_COUNT_WIDTH + (nextX1)];
          if (boxToMove1->tileKindEnum != TILE_WALL && !boxToMove1->hasBox)
          {
            // allowed to push the box
            level->tiles[requestedY * TILE_COUNT_WIDTH + requestedX].hasBox = FALSE;
            boxToMove1->hasBox = TRUE;
            level->playerX += direction.x;
            level->playerY += direction.y;
          }
        }
      }
    }
  }
}

GAME_SWITCH_SCENE(game_switch_select)
{
  state->quitWarningLevel = 0;
  state->currSelection = 0;
  state->currLevelIndex = 0;
  state->gameSceneEnum = SCENE_LEVEL_SELECT;
  state->gameWon = FALSE;
}

void game_update_level(GameState* state, Keyboard* keys)
{
  if (state->gameWon)
  {
    if (keys->key[KEY_CHAR_Q])
    {
      game_switch_select(state);
    }
    return;
  }

  if (keys->key[KEY_CHAR_Q])
  {
    state->quitWarningLevel += 1;
  }
  if (state->quitWarningLevel >= 2)
  {
    game_switch_select(state);
    return;
  }

  bool32 moved = FALSE;
  for (i32 dir = 0;dir < 4;++dir)
  {
    if (keys->key[dir])
    {
      player_move(state, directions[dir]);
      moved = TRUE;
      break;
    }
  }
  if (moved)
  {
    state->quitWarningLevel = 0;
  }
  i32 correctCounter = 0;

  for (i32 y = 0;y < TILE_COUNT_HEIGHT;++y)
  {
    for (i32 x = 0;x < TILE_COUNT_WIDTH;++x)
    {
      Tile currTile = state->level.tiles[y * TILE_COUNT_WIDTH + x];
      if (currTile.tileKindEnum == TILE_CORR)
      {
        if (currTile.hasBox) correctCounter += 1;
      }
    }
  }
  state->level.currCorrectBoxes = correctCounter;
  state->gameWon = (correctCounter == state->level.requiredBoxesCount);
}


void game_switch_level(GameState* state, u32 levelIdx)
{
  state->currSelection = 0;
  state->gameSceneEnum = SCENE_LEVEL;
  state->gameWon = FALSE;
  state->level = Glevels_data[levelIdx];
  state->currLevelIndex = (i32)levelIdx;
}

void game_update_level_select(GameState* state, Keyboard* keyboard)
{
  i32 count = Arrlen(Glevels_data);
  if (keyboard->key[KEY_UP]) 
  {
    state->currSelection = ((state->currSelection-1)+count) % count;
    return;
  }
  if (keyboard->key[KEY_DOWN])
  {
    state->currSelection = ((state->currSelection+1)+count) % count;
    return;
  }
  if (keyboard->key[KEY_ENTER] || keyboard->key[KEY_CHAR_Z])
  {
    game_switch_level(state, state->currSelection);
    return;
  }
  if (keyboard->key[KEY_CHAR_Q])
  {
    state->gameSceneEnum = SCENE_MENU;
  }
}

void game_menu_quit(GameState* state)
{
  state->quitWarningLevel += 1;
  if (state->quitWarningLevel >= 2) Gprocs.quit();
}


#define MENU_OPTIONS \
MENU_DEF("Level select", game_switch_select) \
MENU_DEF("Credits", game_switch_select) \
MENU_DEF("Quit", game_menu_quit) \

const char* GmenuOptionNames[] = {
  #define MENU_DEF(n, p) n,
  MENU_OPTIONS
  #undef MENU_DEF
};

GameSwitchScene* GmenuActions[] = {
  #define MENU_DEF(n, p) p,
  MENU_OPTIONS
  #undef MENU_DEF
};

void game_update_menu(GameState* state, Keyboard* keyboard)
{
  i32 count = Arrlen(GmenuOptionNames);
  if (keyboard->key[KEY_UP]) 
  {
    state->currSelection = ((state->currSelection-1)+count) % count;
    state->quitWarningLevel = 0;
    return;
  }
  if (keyboard->key[KEY_DOWN])
  {
    state->currSelection = ((state->currSelection+1)+count) % count;
    state->quitWarningLevel = 0;
    return;
  }
  if (keyboard->key[KEY_ENTER] || keyboard->key[KEY_CHAR_Z])
  {
    GmenuActions[state->currSelection](state);
    return;
  }
}

void game_update(GameState* state, Keyboard* keyboard, f64 dt)
{
  state->fixedUpdateCounter += dt;
  state->totalSeconds += dt;
  switch (state->gameSceneEnum)
  {
    case SCENE_LEVEL:
      {
        game_update_level(state, keyboard);
        break;
      }
    case SCENE_LEVEL_SELECT:
      {
        game_update_level_select(state, keyboard);
        break;
      }
    case SCENE_MENU:
      {
        game_update_menu(state, keyboard);
        break;
      }
  }
}

void tile_draw(Backbuffer* backbuffer, i32 x, i32 y, Color color)
{
  i32 pixelX = x * TILE_WIDTH_PIXELS;
  i32 pixelY = y * TILE_HEIGHT_PIXELS;
  draw_rectangle(backbuffer, pixelX, pixelY, TILE_WIDTH_PIXELS, TILE_HEIGHT_PIXELS, color);
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

void draw_text(Font* font, Backbuffer* backbuffer, const char* str, f32 penX, f32 penY, Color color)
{
  size_t len = strlen(str);
  for (size_t i = 0;i < len;++i)
  {
    CharData cdata = font->cdata[str[i] - font->startChar];
    for (i32 dy = cdata.y0;dy < cdata.y1;++dy)
    {
      i32 y = (i32)penY + dy - cdata.y0 + cdata.yoff;
      if (y >= 0 && y < (i32)backbuffer->lineCount)
      {
        for (i32 dx = cdata.x0;dx < cdata.x1;++dx)
        {
          i32 x = (i32)penX + dx - cdata.x0 + cdata.xoff;
          u32 intensity = font->pixels[dy * font->width + dx];
          i32 index = y * backbuffer->pixelsPerLine + x;
          if (x < (i32)backbuffer->pixelsPerLine && x >= 0)
          {
            u32* pixel = (u32*)backbuffer->buffer + index;
            u32 currB = ((*pixel >> backbuffer->blueShift) & 0x000000ff);
            u32 currR = ((*pixel >> backbuffer->redShift) & 0x000000ff);
            u32 currG = ((*pixel >> backbuffer->greenShift) & 0x000000ff);
            *pixel = 0;
            *pixel |= ((currB * (255 - intensity) + color.b * intensity)/255) << backbuffer->blueShift;
            *pixel |= ((currG * (255 - intensity) + color.g * intensity)/255) << backbuffer->greenShift;
            *pixel |= ((currR * (255 - intensity) + color.r * intensity)/255) << backbuffer->redShift;
          }
      }
      }
    }
    penX += cdata.xadvance;
  }
}

DEBUG_FONT_DRAW(debug_font_draw)
{
  draw_text(&GrobotoFont, backbuffer, str, penX, penY, color);
}

void draw_box(Backbuffer* backbuffer, i32 x, i32 y)
{
  draw_rectangle(
    backbuffer,
    x + BOX_DIFF,
    y + BOX_DIFF,
    BOX_WIDTH_PIXELS,
    BOX_HIEGHT_PIXELDS,
    COLOR_BOX
  );
}

internal void draw_rectangle_lines(Backbuffer* backbuffer, i32 x, i32 y, i32 w, i32 h, Color color, i32 thickness)
{
  i32 wlen = w - thickness;
  i32 hlen = h - thickness;
  // upper line
  draw_rectangle(
    backbuffer,
    x, y,
    wlen,
    thickness,
    color
  );
  // right line
  draw_rectangle(
    backbuffer,
    x + wlen, y,
    thickness,
    hlen,
    color
  );
  // bottom line
  draw_rectangle(
    backbuffer,
    x + thickness, y + hlen,
    wlen, thickness,
    color
  );
  // left line
  draw_rectangle(
    backbuffer,
    x, y + thickness,
    thickness, hlen,
    color
  );
}


internal void game_draw_level(Backbuffer* backbuffer, GameState* state)
{
  clear_background(backbuffer, 0x18, 0x18, 0x18);
  for (i32 y = 0;y < TILE_COUNT_HEIGHT;++y)
  {
    for (i32 x = 0;x < TILE_COUNT_WIDTH;++x)
    {
      i32 pixelX = x * TILE_WIDTH_PIXELS;
      i32 pixelY = y * TILE_HEIGHT_PIXELS;
      i32 cord = x + y * TILE_COUNT_WIDTH;
      Tile tile = state->level.tiles[cord];
      switch (tile.tileKindEnum)
      {
        case TILE_NORM:
          {
            // draw the tile
            draw_rectangle(
              backbuffer,
              pixelX+1,
              pixelY+1,
              TILE_WIDTH_PIXELS-1,
              TILE_HEIGHT_PIXELS-1,
              COLOR_TILE
            );
            // draw box
            if (tile.hasBox)
            {
              draw_box(backbuffer, pixelX, pixelY);
            }
            break;
          }
        case TILE_WALL:
          {
            tile_draw(backbuffer, x, y, COLOR_WALL);
            break;
          }
        case TILE_CORR:
          {
            if (tile.hasBox)
            {
              // draw green tile
              Color color = COLOR_GREEN;
              color.g += state->playerAnim;
              color.r += state->playerAnim;
              color.g += state->playerAnim;
              draw_rectangle(
                backbuffer,
                pixelX+1,
                pixelY+1,
                TILE_WIDTH_PIXELS-1,
                TILE_HEIGHT_PIXELS-1,
                color
              );
              draw_box(backbuffer, pixelX, pixelY);
            }
            else
            {
              draw_rectangle(
                backbuffer,
                pixelX+1,
                pixelY+1,
                TILE_WIDTH_PIXELS-1,
                TILE_HEIGHT_PIXELS-1,
                COLOR_RED
              );
            }
            break;
          }
      }
    }
  }
  // player drawing
  i32 playerOffset = 2 + state->playerAnim;
  draw_rectangle(
    backbuffer,
    state->level.playerX * TILE_WIDTH_PIXELS+playerOffset,
    state->level.playerY * TILE_HEIGHT_PIXELS+playerOffset,
    TILE_WIDTH_PIXELS-playerOffset*2, TILE_HEIGHT_PIXELS-playerOffset*2,
    COLOR_PLAYER
  );

  f32 drawableHeight = (f32)((f32)(TILE_COUNT_HEIGHT+1) * (f32)TILE_HEIGHT_PIXELS - (TILE_HEIGHT_PIXELS/2));

  // draw current boxes
  Color color;
  if (state->gameWon) color = COLOR_GREEN;
  else color = COLOR_PURE_WHITE;
  debug_font_draw(
    backbuffer,
    tprintf("correct boxes %d/%d",
            state->level.currCorrectBoxes,
            state->level.requiredBoxesCount
            ),
    600.0f,
    drawableHeight,
    color
  );
  draw_text(
    &Groboto_smallFont,
    backbuffer,
    tprintf(
      "[Arrow Keys] - Move   [u] - undo"
    ),
    50.0f, drawableHeight,
    COLOR_WHITE
  );
  const char* msg;
  bool32 warned = state->quitWarningLevel > 0;
  if (warned)
  {
    msg = "(one more time to confirm)";
  }
  else
  {
    msg = "[q] - quit";
  }
  draw_text(
    &Groboto_smallFont,
    backbuffer,
    msg,
    50.0f, drawableHeight+20.0f,
    warned ? COLOR_RED : COLOR_WHITE
  );

  // draw lines around the screen
  draw_rectangle_lines(
    backbuffer,
    0, 0,
    HORIZONTAL_RESOLUTION,
    VERTICAL_RESOLUTION,
    COLOR_GREEN, 1
  );
}


void game_draw_select(Backbuffer* backbuffer, GameState* state)
{
  clear_background(backbuffer, 0x18, 0x18, 0x18);
  Font* font = &GrobotoFont;
  f32 yStart = 100.0f;
  i32 count = Arrlen(Glevels_data);
  for (i32 i = 0;i < count;++i)
  {
    Color c;
    if ((u32)i == state->currSelection)
    {
      c = COLOR_YELLOW;
      i32 factor = state->playerAnim * 3;
      c.b -= factor;
      c.r -= factor;
      c.g -= factor;
    }
    else c = COLOR_WHITE;

    draw_text(
      font,
      backbuffer,
      Glevels_data[i].name,
      50.0f,
      yStart + (f32)i*50.0f,
      c
    );
  }
}

void game_draw_menu(Backbuffer* backbuffer, GameState* state)
{
  clear_background(backbuffer, 0x18, 0x18, 0x18);
  Font* font = &GrobotoFont;
  f32 yStart = 400.0f;
  f32 xStart = 200.0f;
  u32 count = Arrlen(GmenuOptionNames);
  for (u32 i = 0;i < count;++i)
  {
    Color c;
    if ((u32)i == state->currSelection)
    {
      c = COLOR_YELLOW;
      i32 factor = state->playerAnim * 3;
      c.b -= factor;
      c.r -= factor;
      c.g -= factor;
    }
    else c = COLOR_WHITE;

    // last index should always be quit
    if (i == (count-1) && state->quitWarningLevel > 0)
    {
      draw_text(
        font,
        backbuffer,
        "(one more time to confirm)",
        xStart,
        yStart + (f32)i*50.0f,
        COLOR_RED
      );
    }
    else
    {
      draw_text(
        font,
        backbuffer,
        GmenuOptionNames[i],
        xStart,
        yStart + (f32)i*50.0f,
        c
      );
    }
  }
  draw_rectangle_lines(
    backbuffer,
    50, 70,
    100, 200,
    COLOR_PURE_WHITE,
    1
  );
}

void game_draw(Backbuffer* backbuffer, GameState* state)
{
  switch (state->gameSceneEnum)
  {
    case SCENE_LEVEL:
      {
        game_draw_level(backbuffer, state);
        break;
      }
    case SCENE_LEVEL_SELECT:
      {
        game_draw_select(backbuffer, state);
        break;
      }
    case SCENE_MENU:
      {
        game_draw_menu(backbuffer, state);
        break;
      }
  }
}


void fixed_update(GameState* state)
{
  if (!state->increasing)
  {
    state->playerAnim = state->playerAnim + 1;
  }
  else
  {
    state->playerAnim = state->playerAnim - 1;
  }
  if (state->playerAnim >= 15) state->increasing = !state->increasing;
  else if (state->playerAnim <= 0) state->increasing = !state->increasing;
}

void game_init(GameState* state)
{
  state->totalSeconds = 0;
  state->level = Glevels_data[1];
  state->currLevelIndex = 1;
  state->gameSceneEnum = SCENE_MENU;
  state->initialized = TRUE;
}

// ENTRY POINT
GAME_UPDATE_RENDER(game_update_render)
{
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
    game_init(gameState);
  }
  game_update(gameState, &keyboard, dt);
  if (gameState->fixedUpdateCounter >= FIXED_UPDATE_RATE_SECONDS)
  {
    gameState->fixedUpdateCounter -= FIXED_UPDATE_RATE_SECONDS;
    fixed_update(gameState);
  }
  game_draw(backbuffer, gameState);
}

void draw_rectangle(Backbuffer* backbuffer, i32 x, i32 y, i32 w, i32 h, Color color)
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
          *pixel |= ((u32)color.b << backbuffer->blueShift);
          *pixel |= ((u32)color.g << backbuffer->greenShift);
          *pixel |= ((u32)color.r << backbuffer->redShift);
          *pixel |= ((u32)color.a << backbuffer->alphaShift);
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
