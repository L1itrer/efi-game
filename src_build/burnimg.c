#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../src/thirdparty/nob.h"
#include <stdlib.h>
#include <stdio.h>

#define TMP_FILE "temp.txt"
#define EFI_DIR "./efi/"

int main(void)
{
  Cmd* cmd = calloc(1, sizeof(Cmd));
  cmd_append(cmd, "dd");
  cmd_append(cmd, "if=/dev/zero", "of=./build/uefi.img");
  cmd_append(cmd, "bs=512", "count=93750");
  if (!cmd_run(cmd)) return 1;
  FILE* f = fopen(TMP_FILE, "wb+");
  if (f == NULL)
  {
    printf("could not open tmp file\n");
    return 1;
  }
  // HACK: gdisk is not a good program here, i want to provide flags
  // not stdin
  fprintf(f, "o\ny\nn\n1\n2048\n93716\nef00\nw\ny\n");
  fclose(f);
  cmd_append(cmd, "gdisk", "./build/uefi.img");
  if (!cmd_run(cmd, .stdin_path = TMP_FILE)) return 1;
  remove(TMP_FILE);

  cmd_append(cmd, "losetup", "-f");
  if (!cmd_run(cmd, .stdout_path = TMP_FILE)) return 1;
  f = fopen(TMP_FILE, "r");
  char loopback[64] = {0};
  fscanf(f, "%s", loopback);
  fclose(f);
  remove(TMP_FILE);

  cmd_append(cmd, "losetup", "--offset", "1048576", "--sizelimit", "46934528");
  cmd_append(cmd, loopback, "./build/uefi.img");
  if (!cmd_run(cmd)) return 1;

  cmd_append(cmd, "mkdosfs", "-F", "32", loopback);
  if (!cmd_run(cmd)) return 1;
  if (!mkdir_if_not_exists(EFI_DIR)) return 1;

  cmd_append(cmd, "mount", loopback, EFI_DIR);
  if (!cmd_run(cmd)) return 1;
  if (!mkdir_if_not_exists(EFI_DIR "EFI/")) return 1;
  if (!mkdir_if_not_exists(EFI_DIR "EFI/BOOT/")) return 1;

  cmd_append(cmd, "cp", "./build/BOOTX64.EFI", EFI_DIR "EFI/BOOT/BOOTX64.EFI");
  //nob_copy_file("./build/BOOTX64.EFI", EFI_DIR "EFI/BOOT/BOOTX64.EFI");

  if (!cmd_run(cmd)) return 1;
  cmd_append(cmd, "umount", EFI_DIR);
  if (!cmd_run(cmd)) return 1;
  cmd_append(cmd, "losetup", "-d", loopback);
  if (!cmd_run(cmd)) return 1;

  printf("works\n");
  return 0;
}
