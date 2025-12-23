#include "game.h"


void game_update_render(Backbuffer backbuffer, Keyboard keyboard, PlatformProcs procs)
{
  for (int i = 0; i < __KEY_COUNT;++i)
  {
    if (keyboard.key[i]) procs.debug_printf("%d pressed\n", i);
  }
  fill_backbuffer(backbuffer);
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
      *pixel |= (128 << backbuffer.blueShift);
      *pixel |= (128 << backbuffer.greenShift);
      *pixel |= (255 << backbuffer.redShift);
      pixelByte += backbuffer.bytesPerPixel;
    }
    line += backbuffer.pitch;
  }
}
