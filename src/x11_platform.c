#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "game.h"


#define x86_INTRINSICS_IMPLEMENTATION
#include "x86_intrinsics.h"


global const char* gameDllPath =  "./build/game.so";
global bool32 Grunning = TRUE;

typedef struct X11GameLib{
  void* dll;
  GameUpdateRender* game_update_render;
  DebugFontDraw* debug_font_draw;
  struct timespec prevTime;
}X11GameLib;

internal void x11_quit(void)
{
  printf("quitting gracefully...\n");
  Grunning = FALSE;
}

internal void x11_load_game_dll(X11GameLib* game)
{
  struct stat attibs;
  game->dll = dlopen(gameDllPath, RTLD_NOW);
  if (game->dll == NULL)
  {
    printf("wrong dll path: %s\n", gameDllPath);
    assert(game->dll);
    // TODO: platform panic
  }
  game->game_update_render = dlsym(game->dll, "game_update_render");
  assert(game->game_update_render);
  game->debug_font_draw = dlsym(game->dll, "debug_font_draw");
  assert(game->debug_font_draw);
  stat(gameDllPath, &attibs);
  game->prevTime = attibs.st_ctim;
}

internal void x11_reload_game_dll(X11GameLib* game)
{
  struct stat attibs;
  if (game->dll == NULL)
  {
    x11_load_game_dll(game);
    printf("Loaded game dll\n");
    return;
  }
  stat(gameDllPath, &attibs);
  if (attibs.st_ctim.tv_sec > game->prevTime.tv_sec)
  {
    dlclose(game->dll);
    x11_load_game_dll(game);
    printf("Reloaded game dll\n");
  }
}

int main(int argc, char** argv)
{
  UNUSED(argc);
  UNUSED(argv);
  u64 tscFreq = 0;
  bool32 canUseTsc = x86_can_use_rdtsc(&tscFreq);
  if (canUseTsc)
  {
    printf("Using rdtsc, frequency: %lu\n", tscFreq);
    printf("freq = %lu\n", tscFreq);
  }
  X11GameLib gameLib = {0};
  x11_reload_game_dll(&gameLib);
  Display* display = XOpenDisplay(NULL);
  if (display == NULL)
  {
    fprintf(stderr, "Could not open x11 display\n");
    return 1;
  }

  Window window = XCreateSimpleWindow(display, XDefaultRootWindow(display), 0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, 0, 0,0);
  XWindowAttributes windowAttribs = {0};
  XGetWindowAttributes(display, window, &windowAttribs);
  Backbuffer backbuffer = {0};
  backbuffer.bytesPerPixel = 4;
  backbuffer.pixelsPerLine = HORIZONTAL_RESOLUTION;
  backbuffer.pitch = backbuffer.bytesPerPixel * HORIZONTAL_RESOLUTION;
  backbuffer.lineCount = VERTICAL_RESOLUTION;
  backbuffer.blueShift  = 0;
  backbuffer.greenShift = 8;
  backbuffer.redShift   = 16;
  backbuffer.alphaShift = 24;
  u32 bytesPerBuffer = backbuffer.bytesPerPixel * backbuffer.lineCount * backbuffer.pixelsPerLine;
  u64 permanentMemorySize = PERMANENT_MEMORY_SIZE;
  u64 temporaryMemorySize = TEMPORARY_MEMORY_SIZE;
  u64 poolSize = (u64)bytesPerBuffer*2LL + permanentMemorySize + temporaryMemorySize;
  void* memory = mmap(NULL, poolSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (memory == MAP_FAILED)
  {
    printf("Could not map memory: %s\n", strerror(errno));
    return 1;
  }

  void* frontbuffer = memory;
  backbuffer.buffer = (u8*)memory + bytesPerBuffer;
  Memory permaMemory = {
    .data = (void*)((u8*)memory + bytesPerBuffer + bytesPerBuffer),
    .size = permanentMemorySize
  };
  Memory tempMemory = {
    .data = (void*)((u8*)memory + bytesPerBuffer + bytesPerBuffer + permanentMemorySize),
    .size = temporaryMemorySize
  };

  XImage* frontimage = XCreateImage(display, windowAttribs.visual, windowAttribs.depth, ZPixmap, 0, (char*)frontbuffer, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, 32, HORIZONTAL_RESOLUTION * backbuffer.bytesPerPixel);
  XImage* backimage = XCreateImage(display, windowAttribs.visual, windowAttribs.depth, ZPixmap, 0, (char*)backbuffer.buffer, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, 32, HORIZONTAL_RESOLUTION * backbuffer.bytesPerPixel);
  GC gc = XCreateGC(display, window, 0, NULL);
  Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wmDeleteWindow, 1);
  XSelectInput(display, window, KeyPressMask | PointerMotionMask);
  XMapWindow(display, window);
  PlatformProcs platformProcs = (PlatformProcs){
    .debug_printf = printf,
    .quit = x11_quit,
  };

  u64 tscLast, tscEnd = 0, tscWorkEnd = 0, tscWorkLast;
  tscLast = x86_rdtsc();
  tscWorkLast = tscLast;
  f64 secondsElapsed = 0.0f;
  f32 fps = 0.0f;
  u64 workMicroseconds = 0;

  for (;Grunning;)
  {
    x11_reload_game_dll(&gameLib);
    Keyboard keyboard = {0};
    while (XPending(display) > 0)
    {
      XEvent event = {0};
      XNextEvent(display, &event);
      switch (event.type)
      {
        case KeyPress: 
          {

            switch (XLookupKeysym(&event.xkey, 0))
            {
              case 'q':
                {
                  keyboard.key[KEY_CHAR_Q] = TRUE;
                  break;
                }
              case 'z':
                {
                  keyboard.key[KEY_CHAR_Z] = TRUE;
                  break;
                }
              case 'u':
                {
                  keyboard.key[KEY_CHAR_U] = TRUE;
                  break;
                }
              case XK_Up:
                {
                  keyboard.key[KEY_UP] = TRUE;
                  break;
                }
              case XK_Down:
                {
                  keyboard.key[KEY_DOWN] = TRUE;
                  break;
                }
              case XK_Left:
                {
                  keyboard.key[KEY_LEFT] = TRUE;
                  break;
                }
              case XK_Right:
                {
                  keyboard.key[KEY_RIGHT] = TRUE;
                  break;
                }
              case XK_Return:
                {
                  keyboard.key[KEY_ENTER] = TRUE;
                  break;
                }
            }
            break;
          }
        case ClientMessage:
          {
            if ((Atom)event.xclient.data.l[0] == wmDeleteWindow)
            {
              Grunning = FALSE;
            }
            break;
          }
      }
    }
    memset(backbuffer.buffer, 0, bytesPerBuffer);
    gameLib.game_update_render(&backbuffer, keyboard, platformProcs, &permaMemory, &tempMemory, secondsElapsed);
    char buf[1024];
    sprintf(buf, "%lu [us], %.2f FPS\n", workMicroseconds, fps);
    gameLib.debug_font_draw(&backbuffer, buf, 50.0f, 50.0f, COLOR_PURE_WHITE);
    void* temp = backbuffer.buffer;
    XImage* tempImg = backimage;
    backbuffer.buffer = frontbuffer;
    backimage = frontimage;
    frontbuffer = temp;
    frontimage = tempImg;
    XPutImage(display, window, gc, frontimage, 0, 0, 0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    tscEnd = x86_rdtsc();
    tscWorkEnd = tscEnd;
    u64 tscDelta = tscEnd - tscLast;
    u64 microSecondsElapsed = (tscDelta * 1000 * 1000)/tscFreq;
    u64 tscWorkDelta = tscWorkEnd - tscWorkLast;
    workMicroseconds = (tscWorkDelta * 1000 * 1000)/tscFreq;
    secondsElapsed = (f64)tscDelta/(f64)tscFreq;
    //printf("%lu [us], %lf [s]\n", microSecondsElapsed, secondsElapsed);


    if (workMicroseconds < TARGET_US_PER_FRAME)
    {
      u64 remainingUs = TARGET_US_PER_FRAME - workMicroseconds;
      usleep(remainingUs);
    }
    else
    {
      printf("!!!!!Missing frame! %lu\n", microSecondsElapsed);
    }
    tscWorkLast = x86_rdtsc();
    fps = (f32)tscFreq/(f32)tscWorkDelta;
    tscLast = tscEnd;
  }

  XCloseDisplay(display);
  return 0;
}
