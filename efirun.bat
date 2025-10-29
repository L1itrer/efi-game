@echo off
uefi-run -d -b ..\OVMF\DEBUG\OVMF.fd build\BOOTX64.EFI -- -net none -serial mon:stdio -s -S
