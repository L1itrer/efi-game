#include "efi.h"
#include "game.h"
#define x86_INTRINSICS_IMPLEMENTATION
#include "x86_intrinsics.h"

#ifdef RELEASE
#  else
#  ifndef STB_SPRINTF_IMPLEMENTATION
#    define STB_SPRINTF_IMPLEMENTATION
#  endif
// magic variable required by nonexistent crt
int _fltused = 0x9875;
#include "thirdparty/stb/stb_sprintf.h"
#endif
#include "game.c"

#define ERR_CODE(en, str) str,
char16* efiErrorCodeStrsWchar[] = {
  ERROR_CODES_WCHAR
};
#undef ERR_CODE


#define ERR_CODE(en, str) str,
char* efiErrorCodeStrs[] = {
  ERROR_CODES
};
#undef ERR_CODE

global EfiSystemTable* Gst;
global bool32 GcanUseSerial = FALSE;
global bool32 Grunning = TRUE;



internal inline void outb(u16 port, u8 data)
{
  __asm__ __volatile__("outb %1, %0" : : "dN" (port), "a" (data));
}

internal inline u8 inb(u16 port)
{
  u8 r;
  __asm__ __volatile__("inb %1, %0" : "=a" (r) : "dN" (port));
  return r;
}

// NOTE: shamelessly copied from https://wiki.osdev.org/Serial_Ports#Example_Code
#define PORT 0x3f8
internal int init_serial()
{
  outb(PORT + 1, 0x00); // disables all interrupts
  outb(PORT + 3, 0x80); // enables dlab whathever it is
  outb(PORT + 0, 0x03); // sets divisor to 3 or something
  outb(PORT + 1, 0x00); // same as above but hi byte
  outb(PORT + 3, 0x03);
  outb(PORT + 2, 0xC7);
  outb(PORT + 4, 0x0B);
  outb(PORT + 4, 0x1E);
  #define SERIAL_TEST_BYTE 0xAE
  outb(PORT + 0, SERIAL_TEST_BYTE);
  if (inb(PORT+0) != SERIAL_TEST_BYTE)
  {
    return 1;
  }
  outb(PORT + 4, 0x0F);
  GcanUseSerial = TRUE;
  return 0;
}

internal int is_transmit_empty()
{
  return inb(PORT + 5) & 0x20;
}

internal void write_serial(char a) 
{
  while (is_transmit_empty() == 0);
  outb(PORT,a);
}

internal i32 debug_printf(const char* fmt, ...)
{
  i32 count = 0;
#ifdef RELEASE
#else
  if (GcanUseSerial)
  {
    char buffer[1024] = {0};
    va_list va;
    va_start(va, fmt);
    count = stbsp_vsnprintf(buffer, 1024, fmt, va);
    va_end(va);
    for (int i = 0;i < count;++i)
    {
      write_serial(buffer[i]);
    }
  }
#endif
  return count;
}


void efi_quit(void)
{
  debug_printf("quitting gracefully...\n");
  Grunning = FALSE;
}

internal EfiStatus print(char16* str)
{
  EfiStatus status;
  status = Gst->conOut->output_string(Gst->conOut, str);

  return status;
}

internal EfiStatus wait_for_key()
{
  usize x;
  return Gst->
    bootServices->
    wait_for_event(1, &Gst->conIn->waitForKey, &x);
}

void print_and_wait(char16* str)
{
  print(str);
  print(L"(press any key)\r\n");
  wait_for_key();
}

void print_error_wait(char16* str, EfiStatus errCode)
{
  print(str);
  print(L"Error code:");
  print(efiErrorCodeStrsWchar[errCode]);
  print(L"\r\n");
  print(L"(press any key)\r\n");
  wait_for_key();
}

internal EfiStatus disable_watchdog()
{
  return Gst->bootServices->set_watchdog_timer(0, 0, 0, NULL);
}


void* memset(void* dest, int value, size_t count)
{
  u8* buf = dest;
  usize i = 0;
  for (; i < count;++i)
  {
    buf[i] = (u8)value;
  }
  return dest;
}


void efi_poll_keyboard(Keyboard* keyboard)
{
  EfiInputKey key;
  EfiStatus result = EFI_SUCCESS;
  while (result == EFI_SUCCESS)
  {
    result = Gst->conIn->read_key_stroke(Gst->conIn, &key);
    if (result == EFI_SUCCESS)
    {
      for (i32 i = 0;i < __KEY_COUNT;++i)
      {
        if ((key.scanCode == keyScanCodes[i]) && keyPrintable[i] == 0)
          keyboard->key[i] = TRUE;
        else if (key.unicodeChar == keyPrintable[i] && keyScanCodes[i] == 0)
          keyboard->key[i] = TRUE;
      }
    }
  }
  Gst->conIn->reset(Gst->conIn, 0);
}


u64 efi_measure_rdtsc_freq(void)
{
  EfiTime time;
  Gst->runtimeServices->get_time(&time, NULL);
  u64 startSecond = time.second + time.minute*60;
  u64 currSecond = startSecond;
  // align to a whole second
  for (;;)
  {
    Gst->runtimeServices->get_time(&time, NULL);
    currSecond = time.second + time.minute*60;
    if (currSecond != startSecond) break;
  }
  u64 currRdtsc = x86_rdtsc();
  for (;;)
  {
    Gst->runtimeServices->get_time(&time, NULL);
    if (currSecond != (time.second + time.minute*60)) break;
  }
  u64 endRdrsc  = x86_rdtsc();
  return endRdrsc - currRdtsc;
}

bool32 efi_get_rdtsc_freq(u64* freq)
{
  bool32 result = x86_can_use_rdtsc(freq);
  if (!result)
  {
    *freq = efi_measure_rdtsc_freq();
  }
  return result;
}


// make clang shut up about unused image handle
global EfiHandle gImageHandle;

EfiStatus efi_main(EfiHandle imageHandle, EfiSystemTable* st) 
{
  EfiGraphicsOutputBltPixel *frontbuffer, *temp;
  Backbuffer backbuffer;
  EfiStatus status = 0;
  EfiGuid gopGuid;
  EfiGraphicsOutputProtocol* gop;
  EfiGraphicsOutputModeInformation* gopInfo;
  usize sizeOfInfo, modeCount;
  gImageHandle = imageHandle;
  gopGuid.specified = (EfiGuidStruct) EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  Gst = st;
  status = disable_watchdog();
  if (init_serial() == 0)
  {
    debug_printf("Serial works, can do printf debugging...!\n");
  }

  u64 tscFreq = 0;
  bool32 invariantArt = efi_get_rdtsc_freq(&tscFreq);//x86_can_use_rdtsc(&tscFreq);
  if (invariantArt)
  {
    debug_printf("This cpu supports invariant tsc!\n");
  }
  else
  {
    debug_printf("No invariant tsc for you! Detected freq of %llu\n", tscFreq);
  }


  status = Gst->bootServices->locate_protocol(&gopGuid, NULL, (void**)&gop);
  if (EFI_ERROR(status))
  {
    debug_printf("Could not locate GOP!: %s\n", efiErrorCodeStrs[status]);
    return status;
  }
  status = gop->query_mode(gop, gop->mode == NULL ? 0 : gop->mode->mode, &sizeOfInfo, &gopInfo);
  if (status == EFI_NOT_READY)
  {
    status = gop->set_mode(gop, 0);
  }
  if (EFI_ERROR(status))
  {
    debug_printf("Could not get native mode: %s\n", efiErrorCodeStrs[status]);
    return status;
  }
  modeCount = gop->mode->maxMode;
  bool32 foundMode = FALSE;
  for (usize i = 0;i < modeCount;++i)
  {
    status = gop->query_mode(gop, i, &sizeOfInfo, &gopInfo);
    u32 hRes = gopInfo->hotizontalResolution;
    u32 vRes = gopInfo->verticalResolution;
    i32 format = gopInfo->pixelFormatEnum;
    if ((vRes >= VERTICAL_RESOLUTION) && (hRes >= HORIZONTAL_RESOLUTION) && (format == PixelBlueGreenRedReserved8BitPerColor))
    {
      gop->set_mode(gop, i);
      foundMode = TRUE;
      break;
    }
  }
  if (!foundMode)
  {
    debug_printf("Could not find suitable mode!\n");
    return status;
  }
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
  // TODO: allocate pool actually has very small limits
  // maybe I should read the memory map and allocate stuff
  // myself
  status = Gst->bootServices->allocate_pool(EfiBootServicesData, poolSize, (void**)&frontbuffer);
  if (EFI_ERROR(status))
  {
    debug_printf("Could not allocate %llu bytes for the memory pool: %s\n", poolSize, efiErrorCodeStrs[status]);
    return status;
  }
  Memory permaMemory = {
    .data = (void*)((u8*)frontbuffer + bytesPerBuffer + bytesPerBuffer),
    .size = permanentMemorySize
  };
  Memory tempMemory = {
    .data = (void*)((u8*)frontbuffer + bytesPerBuffer + bytesPerBuffer + permanentMemorySize),
    .size = temporaryMemorySize
  };
  memset(permaMemory.data, 0, permanentMemorySize);
  memset(tempMemory.data, 0, temporaryMemorySize);
  backbuffer.buffer = (u8*)frontbuffer + bytesPerBuffer;
  Keyboard keyboard = {0};
  // TODO: add a panic
  PlatformProcs procs = {
    .debug_printf = debug_printf,
    .quit = efi_quit,
  };
  u64 tscLast = 0, tscEnd = 0, tscWorkLast = 0, tscWorkEnd = 0;
  if (invariantArt)
  {
    tscLast = x86_rdtsc();
    tscWorkLast = tscLast;
  }
  char canUseRdtscMsg[16];
  char freqMsg[16];
  stbsp_sprintf(canUseRdtscMsg, "rdtsc: %s", invariantArt ? "true" : "false");
  stbsp_sprintf(freqMsg, "freq %llu", tscFreq);
  f64 secondsElapsed = 0.03;
  debug_printf("Initialization complete\n");
  u64 workUs = 0;
  for (;Grunning;)
  {
    memset(backbuffer.buffer, 0, bytesPerBuffer);
    memset(&keyboard, 0, sizeof(keyboard));
    efi_poll_keyboard(&keyboard);
    game_update_render(&backbuffer, keyboard, procs, &permaMemory, &tempMemory, secondsElapsed);
#if 0
    debug_font_draw(&backbuffer, canUseRdtscMsg, 500.0f, 50.0f, COLOR_PURE_WHITE);
    debug_font_draw(&backbuffer, freqMsg, 500.0f, 100.0f, COLOR_PURE_WHITE);
    debug_font_draw(&backbuffer, tprintf("workUs %llu", workUs), 500.0f, 150.0f, COLOR_PURE_WHITE);
#endif
    temp = backbuffer.buffer;
    backbuffer.buffer = frontbuffer;
    frontbuffer = temp;
    status = gop->blt(gop, frontbuffer, EfiBltBufferToVideo, 0, 0, 0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, 0);
    u64 remainingUs = 0;
    tscEnd = x86_rdtsc();
    tscWorkEnd = tscEnd;
    u64 tscDelta = tscEnd - tscLast;
    u64 tscWorkDelta = tscWorkEnd - tscWorkLast;
    //u64 microSecondsElapsed = (tscDelta * 1000 * 1000)/tscFreq;
    secondsElapsed = (f64)tscDelta/(f64)tscFreq;
    workUs = (tscWorkDelta * 1000 * 1000)/tscFreq;
    if (workUs < TARGET_US_PER_FRAME)
    {
      remainingUs = TARGET_US_PER_FRAME - workUs;
    }
    else
    {
      debug_printf("Missing frame!\n");
    }
    if (EFI_ERROR(Gst->bootServices->stall(remainingUs)))
    {
      debug_printf("did not stall correctly\n");
    }
    // can do this every frame rdtsc is safe it's just useless
    // without frequency
    tscWorkLast = x86_rdtsc();
    tscLast = tscEnd;
  }
  debug_printf("loop end\n");
  Gst->conOut->clear_screen(Gst->conOut);


  return status;
}

// for testing of types etc
#ifdef EFI_TEST
#include <stdio.h>
int main() {
  EfiGuid gopGuid;
  printf("guid        size: %zu\n", sizeof(EfiGuid));
  printf("guid struct size: %zu\n", sizeof(EfiGuidStruct));
  gopGuid.specified = (EfiGuidStruct) EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  printf("data1: size: %zu value: %x\n", sizeof(gopGuid.specified.data1), gopGuid.specified.data1);
  printf("data2: size: %zu value: %x\n", sizeof(gopGuid.specified.data2), gopGuid.specified.data2);
  printf("data3: size: %zu value: %x\n", sizeof(gopGuid.specified.data3), gopGuid.specified.data3);
  for (int i = 0;i < 8;++i) {
    printf("%x, ", gopGuid.specified.data4[i]);
  }
  printf("\n");
  return 0;
}
#endif
