#include "efi.h"
#define ERR_CODE(en, str) str,
char16* efiErrorCodeStrs[] = {
  ERROR_CODES
};
#undef ERR_CODE

global EfiSystemTable* Gst;

#define HORIZONTAL_RESOLUTION (1920 / 2)
#define VERTICAL_RESOLUTION (1080 / 2)

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
  return 0;
}

int is_transmit_empty()
{
  return inb(PORT + 5) & 0x20;
}

void write_serial(char a) 
{
  while (is_transmit_empty() == 0);
  outb(PORT,a);
}

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
  print(L"(press any key)\r\n");
  wait_for_key();
}

void print_error_wait(char16* str, EfiStatus errCode)
{
  print(str);
  print(L"Error code:");
  print(efiErrorCodeStrs[errCode]);
  print(L"\r\n");
  print(L"(press any key)\r\n");
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
    write_serial('u');
    write_serial('r');
    write_serial('m');
    write_serial('o');
    write_serial('m');
  }


  status = Gst->bootServices->locate_protocol(&gopGuid, NULL, (void**)&gop);
  if (EFI_ERROR(status))
  {
    print_and_wait(L"Could not locate GOP!\r\n");
    return status;
  }
  status = gop->query_mode(gop, gop->mode == NULL ? 0 : gop->mode->mode, &sizeOfInfo, &gopInfo);
  if (status == EFI_NOT_READY)
  {
    status = gop->set_mode(gop, 0);
  }
  if (EFI_ERROR(status))
  {
    print_and_wait(L"Could not get native mode!\r\n");
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
  u32 bytesPerBuffer = backbuffer.bytesPerPixel * backbuffer.lineCount * backbuffer.pixelsPerLine;
  status = Gst->bootServices->allocate_pool(EfiBootServicesData, bytesPerBuffer*2, (void**)&frontbuffer);
  if (EFI_ERROR(status))
  {
    print_error_wait(L"Could not allocate memory for the frontbuffer and backbuffer\r\n", status);
    return status;
  }
  for (;;)
  {
    backbuffer.buffer = (u8*)frontbuffer + bytesPerBuffer;
    memoryset(backbuffer.buffer, 0, bytesPerBuffer);
    fill_backbuffer(backbuffer);
    temp = backbuffer.buffer;
    backbuffer.buffer = frontbuffer;
    frontbuffer = temp;
    status = gop->blt(gop, frontbuffer, EfiBltBufferToVideo, 0, 0, 0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, 0);
  }
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
