# About

The repository contains an implementation of a simple [Sokoban](https://en.wikipedia.org/wiki/Sokoban)-like game written in pure C that works on several platforms including an efi platform. The game is written in the style of [handmade hero](https://guide.handmadehero.org/) which allows it to easliy be ported to even most obscure platforms.

# Building

To build the project you need only a C compiler, specifically clang. In theory any compiler could be used but producing efi images is the simplest using clang. Regardless of the platform you're on the build should be exactly two commands. Run the commands from the root of the project (the folder containing nob.c file).

## Linux

```sh
clang nob.c -o nob
./nob
```

## Windows

```cmd
clang nob.c -o nob
nob.exe
```

Once you create the nob executable you do not need to recompile nob.c - it will recompile itself. Check out [nob.h](https://github.com/tsoding/nob.h).

# Utilities

Aside from game executables there are some build utilities that should appear in the build folder. Here are explanations for all of them.

## efirun

A quick way to test the game on efi platform. Assumes you have qemu-syste,-x86_64 in PATH. Download the split version of [OVMF](https://retrage.github.io/edk2-nightly/) and provide it as command arguments.

```cmd
build/efirun <path_to_ovmf_code> <path_to_ovmf_vars>
```

## burnimg

A way to produce an uefi.img that can be burned onto a usb stick. It assumes you have the following utilities in PATH: dd, gdisk, losetup, mkdosfs and umount.

```cmd
build/burnimg
```

## ttf2c

An utility to produce c files with prerendered bitmap fonts. Used by the build system. Not meant as a user utility.

# How to run on real hardware (efi)

No warranty for destroyed hardware! You need a usb stick, build the project using nob and run burnimg utility. An uefi.img should appear in the build folder. Burn the image onto the usb using something like [Rufus](https://rufus.ie/en/) or [Balena Etcher](https://etcher.balena.io/). To run the plug the usb into your mashine before it on and enter uefi firmware settings. Boot the usb from there.

