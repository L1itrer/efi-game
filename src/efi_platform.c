#include "uefi.h"

global EfiSystemTable* Gst;

EfiStatus print(char16* str) {
  EfiStatus status;
  status = Gst->conOut->output_string(Gst->conOut, str);

  return status;
}

EfiStatus wait_for_key() {
  usize x;
  return Gst->
    bootServices->
    wait_for_event(1, &Gst->conIn->waitForKey, &x);
}

EfiStatus disable_watchdog() {
  return Gst->bootServices->set_watchdog_timer(0, 0, 0, NULL);
}

EfiStatus efi_main(EfiHandle imageHandle, EfiSystemTable* st) {
  EfiStatus status = 0;
  EfiGuid gopGuid;
  gopGuid.specified = (EfiGuidStruct) EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  Gst = st;
  status = disable_watchdog();
  if (EFI_ERROR(status)) {
    print(L"could not disable watchdog!\r\n");
    wait_for_key();
  } else {
    print(L"disabled watchdog successfully!\r\n");
    wait_for_key();
  }

  return status;
};

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
