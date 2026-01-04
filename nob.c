#define NOB_IMPLEMENTATION
#if !defined(_WIN32)
#  define NOB_EXPERIMENTAL_DELETE_OLD
#endif
#define NOB_STRIP_PREFIX
#include "./src/thirdparty/nob.h"

#define BUILD_DIR "./build"
#define EFI_EXE "BOOTX64.EFI"
#define SRC_BUILD_DIR  "./src_build"
#define META_DIR "./meta"

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
  cmd_append(cmd, "-g");
  cmd_append(cmd, "-mno-red-zone");
  // cmd_append(cmd, "-maccumulate-outgoing-args");
  cmd_append(cmd, "-fshort-wchar");
  cmd_append(cmd, "-DHAVE_USE_MS_ABI");
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
  cmd_append(cmd, "clang");
}

int build_x11(Cmd* cmd, Procs* procs)
{
  compiler(cmd);
  cmd_append(cmd, "-Wall", "-Wextra");
  cmd_append(cmd, "-g");
  cmd_append(cmd, "./src/x11_platform.c");
  cmd_append(cmd, "-o", BUILD_DIR "/sokoban_x11");
  //cmd_append(cmd, "-nostdlib");
  cmd_append(cmd, "-lX11");
  if (!cmd_run(cmd, .async = procs)) return 1;
  compiler(cmd);
  cmd_append(cmd, "./src/game.c");
  cmd_append(cmd, "-Wall", "-Wextra", "-g");
  cmd_append(cmd, "-o", BUILD_DIR "/game.so");
  cmd_append(cmd, "-shared", "-fPIC");
  cmd_append(cmd, "-fpic");
  cmd_append(cmd, "-nostdlib");
  cmd_append(cmd, "-O2");
  if (!cmd_run(cmd, .async = procs)) return 1;
  return 0;
}

int main(int argc, char** argv)
{
  NOB_GO_REBUILD_URSELF(argc, argv);
  Cmd cmd_static = {0};
  Procs procs = {0};
  Cmd* cmd = &cmd_static;
  if (!mkdir_if_not_exists(BUILD_DIR)) return 1;
  if (!mkdir_if_not_exists(META_DIR)) return 1;

  if (needs_rebuild1(BUILD_DIR "/ttf2c", SRC_BUILD_DIR "/ttf2c.c"))
  {
    compiler(cmd);
    cmd_append(cmd, "-Wall", "-Wextra");
    cmd_append(cmd, SRC_BUILD_DIR "/ttf2c.c");
    cmd_append(cmd, "-o", BUILD_DIR "/ttf2c");
    cmd_append(cmd, "-lm", "-g");
    if (!cmd_run(cmd, .async = &procs)) return 1;
  }
  if (needs_rebuild1(BUILD_DIR "/burnimg", SRC_BUILD_DIR "/burnimg.c"))
  {
    compiler(cmd);
    cmd_append(cmd, "-Wall", "-Wextra");
    cmd_append(cmd, SRC_BUILD_DIR "/burnimg.c");
    cmd_append(cmd, "-o", BUILD_DIR "/burnimg");
    cmd_append(cmd, "-g");
    if (!cmd_run(cmd, .async = &procs)) return 1;
  }
  procs_flush(&procs);
  if (needs_rebuild1(META_DIR "/roboto.h", SRC_BUILD_DIR "/ttf2c.c"))
  {
    cmd_append(cmd, BUILD_DIR "/ttf2c");
    cmd_append(cmd, "./assets/fonts/Roboto-Regular.ttf");
    cmd_append(cmd, META_DIR "/roboto.h");
    cmd_append(cmd, "roboto");
    cmd_append(cmd, "30");
    if (!cmd_run(cmd)) return 1;
  }
  if (needs_rebuild1(META_DIR "/roboto_small.h", SRC_BUILD_DIR "/ttf2c.c"))
  {
    cmd_append(cmd, BUILD_DIR "/ttf2c");
    cmd_append(cmd, "./assets/fonts/Roboto-Regular.ttf");
    cmd_append(cmd, META_DIR "/roboto_small.h");
    cmd_append(cmd, "roboto_small");
    cmd_append(cmd, "20");
    if (!cmd_run(cmd)) return 1;
  }
  build_x11(cmd, &procs);

  compiler(cmd);
  cflags(cmd, true);
  cmd_append(cmd, "-O2");
  cmd_append(cmd, "./src/efi_platform.c");
  cmd_append(cmd, "-o");
  cmd_append(cmd, BUILD_DIR "/" EFI_EXE);
  cmd_append(cmd, "-I./src/thirdparty/stb/");
  ldflags(cmd);

  if (!cmd_run(cmd, .async = &procs)) return 1;

  procs_wait(procs);


  return 0;
}
