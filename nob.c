#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_STRIP_PREFIX
#include "./src/thirdparty/nob/nob.h"

#define BUILD_DIR "./build"
#define EFI_EXE "BOOTX64.EFI"

void cflags(Cmd* cmd, bool warnings)
{
  if (warnings)
  {
    cmd_append(cmd, "-Wall");
    cmd_append(cmd, "-Wextra");
  }
  cmd_append(cmd, "-ffreestanding");
  cmd_append(cmd, "-fno-stack-protector");
  cmd_append(cmd, "-fno-stack-check");
  cmd_append(cmd, "-ggdb");
  cmd_append(cmd, "-mno-red-zone");
  // cmd_append(cmd, "-maccumulate-outgoing-args");
  cmd_append(cmd, "-fshort-wchar");
  cmd_append(cmd, "-DHAVE_USE_MS_ABI");
}

void posix_efi_flags(Cmd* cmd)
{
  cmd_append(cmd, "-I.");
  cmd_append(cmd, "-I./src/thirdparty/posix-uefi/uefi");
  cmd_append(cmd, "-I./usr/include");
  cmd_append(cmd, "-I./usr/include/efi");
  cmd_append(cmd, "-I./usr/include/efi/protocol");
  cmd_append(cmd, "-I./usr/include/efi/x86_64");
  cmd_append(cmd, "-D__x86_64__");
  cmd_append(cmd, "-Wno-builtin-declaration-mismatch");
}

void ldflags(Cmd* cmd)
{
  cmd_append(cmd, "-nostdlib");
  cmd_append(cmd, "-fuse-ld=lld");
  cmd_append(cmd, "--target=x86_64-windows");
  cmd_append(cmd, "-Wl,-entry:efi_main");
  cmd_append(cmd, "-Wl,-subsystem:efi_application");
}

void compiler(Cmd* cmd)
{
  // cmd_append(cmd, "gcc");
  cmd_append(cmd, "clang");
}

void objcopy_flags(Cmd* cmd)
{
  cmd_append(cmd, "-j", ".text");
  cmd_append(cmd, "-j", ".sdata");
  cmd_append(cmd, "-j", ".data");
  cmd_append(cmd, "-j", ".dynamic");
  cmd_append(cmd, "-j", ".dynsym");
  cmd_append(cmd, "-j", ".rel");
  cmd_append(cmd, "-j", ".rela");
  cmd_append(cmd, "-j", ".rel.*");
  cmd_append(cmd, "-j", ".rela.*");
  cmd_append(cmd, "-j", ".reloc");
  cmd_append(cmd, "--target", "efi-app-x86_64");
  cmd_append(cmd, "--subsystem=10");
  cmd_append(cmd, BUILD_DIR "/" EFI_EXE ".so");
  cmd_append(cmd, BUILD_DIR "/" EFI_EXE ".debug");
}

char* posix_efi_libc[] = {
  "crt_x86_64",
  "qsort",
  "dirent",
  "stat",
  "stdio",
  "stdlib",
  "string",
  "time",
  "unistd"
};

#define POSIX_EFI_DIR "./src/thirdparty/posix-uefi/uefi/"

bool build_debug(Cmd* cmd)
{
  Procs procs = {0};
  for (int i = 0;i < NOB_ARRAY_LEN(posix_efi_libc);++i)
  {
    cmd_append(cmd, "gcc");
    cflags(cmd, false);
    cmd_append(cmd, "-fpic");
    cmd_append(cmd, "-fPIC");
    posix_efi_flags(cmd);
    cmd_append(cmd, "-c");
    cmd_append(cmd, temp_sprintf("%s%s.c", POSIX_EFI_DIR, posix_efi_libc[i]));
    cmd_append(cmd, "-o", temp_sprintf("%s/%s.o", BUILD_DIR, posix_efi_libc[i]));
    if (!cmd_run(cmd, .async = &procs, .max_procs = 8)) return false;
  }
  procs_wait(procs);
  cmd_append(cmd, "ar");
  cmd_append(cmd, "-rsv");
  cmd_append(cmd, "./build/libuefi.a");
  // NOTE: excludes crt from libuefi
  for (int i = 1;i < NOB_ARRAY_LEN(posix_efi_libc);++i)
  {
    cmd_append(cmd, temp_sprintf("./build/%s.o", posix_efi_libc[i]));
  }
  if (!cmd_run(cmd)) return false;

  cmd_append(cmd, "gcc");
  cflags(cmd, true);
  cmd_append(cmd, "-fpic");
  cmd_append(cmd, "-fPIC");
  cmd_append(cmd, "-c");
  cmd_append(cmd, "./src/efi_platform.c");
  cmd_append(cmd, "-o", "./build/efi_platform.o");
  if (!cmd_run(cmd)) return false;
  cmd_append(cmd, "gcc");
  cflags(cmd, true);
  posix_efi_flags(cmd);
  cmd_append(cmd, "-c");
  cmd_append(cmd, "./src/posix_main.c");
  cmd_append(cmd, "-o", "./build/posix_main.o");
  if (!cmd_run(cmd)) return false;


  cmd_append(cmd, "ld");
  cmd_append(cmd, "-nostdlib");
  cmd_append(cmd, "-shared");
  cmd_append(cmd, "-Bsymbolic");
  cmd_append(cmd, "-Lbuild");
  cmd_append(cmd, "-Luefi");
  cmd_append(cmd, "./build/efi_platform.o", "./build/posix_main.o", temp_sprintf("%s/%s.o", BUILD_DIR, posix_efi_libc[0]));
  cmd_append(cmd, "-o", BUILD_DIR "/" EFI_EXE ".so");
  cmd_append(cmd, "-luefi");
  cmd_append(cmd, "-T", "./src/thirdparty/posix-uefi/uefi/elf_x86_64_efi.lds");
  if (!cmd_run(cmd)) return false;

  cmd_append(cmd, "objcopy");
  objcopy_flags(cmd);
  if (!cmd_run(cmd)) return false;

  cmd_append(cmd, "objcopy", "--only-keep-debug", BUILD_DIR "/" EFI_EXE ".so", BUILD_DIR "/" EFI_EXE ".debuginfo");
  if (!cmd_run(cmd)) return false;


  return true;
}

int main(int argc, char** argv)
{
  NOB_GO_REBUILD_URSELF(argc, argv);
  Cmd cmd_static = {0};
  Procs procs = {0};
  Cmd* cmd = &cmd_static;
  mkdir_if_not_exists(BUILD_DIR);

  
  // build_debug(cmd);

  compiler(cmd);
  cflags(cmd, true);
  cmd_append(cmd, "./src/efi_platform.c");
  cmd_append(cmd, "-o");
  cmd_append(cmd, BUILD_DIR "/" EFI_EXE);
  ldflags(cmd);

  if (!cmd_run(cmd, .async = &procs)) return 1;

  build_debug(cmd);

  procs_wait(procs);


  return 0;
}
