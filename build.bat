@echo off
mkdir build
clang -Wall -Wextra -Wno-unused-parameter -g --target=x86_64-windows -nostdlib -ffreestanding -fuse-ld=lld -fshort-wchar -mno-red-zone -march=x86-64 -Wl,/entry:efi_main -Wl,/subsystem:efi_application -o build\BOOTX64.EFI src\efi_platform.c

REM clang -Wall -Wextra -nostdlib -c -fPIC -Wno-unused-parameter -fshort-wchar -ggdb -shared --target=x86_64-linux -o build\bootx64.so src\efi_platform.c

