@echo off
mkdir build
REM clang -Wall -Wextra -Wno-unused-parameter -g --target=x86_64-windows -nostdlib -ffreestanding -fuse-ld=lld -fshort-wchar -mno-red-zone -march=x86-64 -Wl,/entry:efi_main -Wl,/subsystem:efi_application -o build\BOOTX64.EFI src\efi_platform.c
gcc -Wall -Wextra -Wno-unused-parameter -ggdb -nostdlib -ffreestanding -fshort-wchar -mno-red-zone -march=x86-64 -Wl,--entry=efi_main -o build\BOOTX64.exe src\efi_platform.c 
REM -Wl,--subsystem=10

REM gcc -Wall -Wextra -nostdlib -shared -fpic -fPIC -Wno-unused-parameter -fshort-wchar -ggdb -shared -o build\bootx64.dll src\efi_platform.c -Wl,--entry=efi_main

REM objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 .\build\bootx64.exe .\build\bootx64.efi
objcopy -j .text -j .bss -j .xdata -j .pdata -j .rdata --target efi-app-x86_64 --subsystem=10 .\build\bootx64.exe .\build\bootx64.efi
REM objcopy --only-keep-debug .\build\bootx64.exe .\build\bootx64.efi.debug
objcopy -j .debug_aranges -j .debug_info -j .debug_abbrev -j .debug_line -j .debug_frame -j .debug_str -j .debug_line_str .\build\bootx64.exe .\build\bootx64.efi.debug

