#define NOB_IMPLEMENTATION
#include "../src/thirdparty/nob.h"

void usage(void)
{
  printf("Usage: ./build/efirun <path_to_ovmf_code> <path_to_ovmf_vars>\n");
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    usage();
    return 1;
  }
  const char* codePath = argv[1];
  const char* varsPath = argv[2];
  if (!nob_mkdir_if_not_exists("esp")) return 1;
  if (!nob_copy_file("build/BOOTX64.EFI", "esp/sokoban.efi")) return 1;
  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "qemu-system-x86_64", "-cpu", "qemu64");
  nob_cmd_append(&cmd, "-drive", nob_temp_sprintf("if=pflash,format=raw,unit=0,file=%s,readonly=on", codePath));
  nob_cmd_append(&cmd, "-drive", nob_temp_sprintf("if=pflash,format=raw,unit=1,file=%s", varsPath));
  nob_cmd_append(&cmd, "-drive", "format=raw,file=fat:rw:esp");
  nob_cmd_append(&cmd, "-net", "none");
  if (!nob_cmd_run(&cmd)) return 1;
  nob_log(NOB_INFO, "OK");
  return 0;
}
