#include "uefi.h"

EfiStatus efi_main(EfiHandle imageHandle, EfiSystemTable* st) {
  EfiStatus status = 0;
  EfiInputKey key;
  usize x;

  status = st->conOut->output_string(st->conOut, L"Hello World\r\n");


  status = st->bootServices->wait_for_event(1, &st->conIn->waitForKey, &x);


  return status;
};

