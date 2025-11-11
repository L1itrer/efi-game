#include "efi.h"

global EfiSystemTable* Gst;

#define HORIZONTAL_RESOLUTION (1920 / 2)
#define VERTICAL_RESOLUTION (1080 / 2)

EfiStatus print(char16* str)
{
  EfiStatus status;
  status = Gst->conOut->output_string(Gst->conOut, str);

  return status;
}

EfiStatus wait_for_key()
{
  usize x;
  return Gst->
    bootServices->
    wait_for_event(1, &Gst->conIn->waitForKey, &x);
}

void print_and_wait(char16* str)
{
  print(str);
  wait_for_key();
}

EfiStatus disable_watchdog()
{
  return Gst->bootServices->set_watchdog_timer(0, 0, 0, NULL);
}

/*void print_hex(u64 value)
{
  // TODO: finish this procedure
  const char16* array = L"0123456789ABCDEF";
  char16 buffer[1024] = {0};
  u64 digit;
  while (value != 0)
  {
    digit = value / 16;
  }
}*/

typedef struct Backbuffer {
  void* buffer;
  u32 pixelsPerLine;
  u32 lineCount;
  u32 pitch; // how many pixels to advance to new line
  u32 bytesPerPixel;
}Backbuffer;

void memoryset(void* buffer, u8 value, usize count)
{
  u8* buf = buffer;
  for (usize i = 0; i < count;++i)
  {
    buf[i] = value;
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
      EfiGraphicsOutputBltPixel* pixel = (EfiGraphicsOutputBltPixel*)pixelByte;
      // fill the color
      pixel->blue = 128;
      pixel->green = 128;
      pixel->red = 255;
      pixelByte += backbuffer.bytesPerPixel;
    }
    line += backbuffer.pitch;
  }
}

EfiStatus efi_main(EfiHandle imageHandle, EfiSystemTable* st) 
{
  EfiGraphicsOutputBltPixel *frontbuffer, *temp;
  Backbuffer backbuffer;
  EfiStatus status = 0;
  EfiGuid gopGuid;
  EfiGraphicsOutputProtocol* gop;
  EfiGraphicsOutputModeInformation* gopInfo;
  usize sizeOfInfo, modeCount;
  gopGuid.specified = (EfiGuidStruct) EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  Gst = st;
  status = disable_watchdog();


  status = Gst->bootServices->locate_protocol(&gopGuid, NULL, (void**)&gop);
  if (EFI_ERROR(status))
  {
    print(L"Could not locate GOP!\r\n");
    wait_for_key();
    return status;
  }
  status = gop->query_mode(gop, gop->mode == NULL ? 0 : gop->mode->mode, &sizeOfInfo, &gopInfo);
  if (status == EFI_NOT_READY)
  {
    status = gop->set_mode(gop, 0);
  }
  if (EFI_ERROR(status))
  {
    print(L"Could not get native mode!\r\n");
    wait_for_key();
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
    print_and_wait(L"could not find suitable mode!\r\n");
    return status;
  }
  backbuffer.bytesPerPixel = 4;
  backbuffer.pixelsPerLine = HORIZONTAL_RESOLUTION;
  backbuffer.pitch = backbuffer.bytesPerPixel * HORIZONTAL_RESOLUTION;
  backbuffer.lineCount = VERTICAL_RESOLUTION;
  u32 bytesPerBuffer = backbuffer.bytesPerPixel * backbuffer.lineCount * backbuffer.bytesPerPixel;
  status = Gst->bootServices->allocate_pool(EfiBootServicesData, bytesPerBuffer*2, (void**)&frontbuffer);
  if (EFI_ERROR(status))
  {
    print_and_wait(L"Could not allocate memory for the frontbuffer and backbuffer\r\n");
    return status;
  }
  memoryset(backbuffer.buffer, 0, bytesPerBuffer);
  fill_backbuffer(backbuffer);
  temp = backbuffer.buffer;
  backbuffer.buffer = frontbuffer;
  frontbuffer = temp;
  status = gop->blt(gop, frontbuffer, EfiBltBufferToVideo, 0, 0, 0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, 0);
  wait_for_key();


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
