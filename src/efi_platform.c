#include "uefi.h"

global EfiSystemTable* Gst;

#define RESOLUTION_WIDTH (1920 / 2)
#define RESOLUTION_HEIGHT (1080 / 2)

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

EfiStatus efi_main(EfiHandle imageHandle, EfiSystemTable* st) 
{
  EfiStatus status = 0;
  EfiGuid gopGuid, loadedImageGuid;
  EfiGraphicsOutputProtocol* gop;
  EfiGraphicsOutputModeInformation* gopInfo;
  EfiLoadedImageProtocol* loadedImage;
  usize sizeOfInfo, modeCount;
  gopGuid.specified = (EfiGuidStruct) EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  loadedImageGuid.specified = (EfiGuidStruct) EFI_LOADED_IMAGE_PROTOCOL_GUID;
  Gst = st;
  status = disable_watchdog();

  status = Gst->bootServices->locate_protocol(&loadedImageGuid, NULL, (void**)&loadedImage);
  if (EFI_ERROR(status))
  {
    print_and_wait(L"Could not locate loadedImage\r\n");
    return status;
  }

  volatile u64* marker_ptr = (u64*)0x10000;
  volatile u64* image_base_ptr = (u64*)0x10008;
  *image_base_ptr = (u64)loadedImage->imageBase;
  *marker_ptr = 0xDEADBEEF;


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
    if (hRes >= RESOLUTION_HEIGHT && vRes >= RESOLUTION_WIDTH && format == PixelBlueGreenRedReserved8BitPerColor)
    {
      gop->set_mode(gop, i);
      foundMode = TRUE;
      break;
    }
  }
  if (foundMode)
  {
    print_and_wait(L"found suitable mode!\r\n");
  }
  else
  {
    print_and_wait(L"could not find suitable mode!\r\n");
  }


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
