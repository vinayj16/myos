#!/bin/bash

# Simple build script for MyOS
echo "Building MyOS..."

# Clean previous build
echo "Cleaning..."
rm -f kernel/*.o kernel/libc/*.o kernel/mm/*.o kernel/arch/x86_64/*.o
rm -f kernel.bin myos.iso

# Compile assembly files
echo "Assembling boot.s..."
nasm -f elf64 kernel/arch/x86_64/boot.s -o kernel/arch/x86_64/boot.o

# Compile C files
CFLAGS="-ffreestanding -O2 -Wall -Wextra -m64 -nostdlib -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -mno-red-zone -Iinclude -Ikernel/mm -std=gnu11 -fno-stack-protector -fno-omit-frame-pointer -mcmodel=kernel"

echo "Compiling kernel sources..."
gcc $CFLAGS -c kernel/desktop.c -o kernel/desktop.o
gcc $CFLAGS -c kernel/font.c -o kernel/font.o
gcc $CFLAGS -c kernel/framebuffer.c -o kernel/framebuffer.o
gcc $CFLAGS -c kernel/fs.c -o kernel/fs.o
gcc $CFLAGS -c kernel/graphics.c -o kernel/graphics.o
gcc $CFLAGS -c kernel/idt.c -o kernel/idt.o
gcc $CFLAGS -c kernel/keyboard.c -o kernel/keyboard.o
gcc $CFLAGS -c kernel/kmain.c -o kernel/kmain.o
gcc $CFLAGS -c kernel/mouse.c -o kernel/mouse.o
gcc $CFLAGS -c kernel/process.c -o kernel/process.o
gcc $CFLAGS -c kernel/text.c -o kernel/text.o
gcc $CFLAGS -c kernel/ui.c -o kernel/ui.o
gcc $CFLAGS -c kernel/window.c -o kernel/window.o

echo "Compiling libc sources..."
gcc $CFLAGS -c kernel/libc/memmove.c -o kernel/libc/memmove.o
gcc $CFLAGS -c kernel/libc/printf.c -o kernel/libc/printf.o
gcc $CFLAGS -c kernel/libc/stdio.c -o kernel/libc/stdio.o
gcc $CFLAGS -c kernel/libc/string.c -o kernel/libc/string.o

echo "Compiling memory management..."
gcc $CFLAGS -c kernel/mm/mm.c -o kernel/mm/mm.o

# Link kernel
echo "Linking kernel..."
LDFLAGS="-nostdlib -z max-page-size=0x1000 -static -Bsymbolic --no-undefined --entry=_start"

ld -T linker.ld -o kernel.bin \
   kernel/arch/x86_64/boot.o \
   kernel/desktop.o \
   kernel/font.o \
   kernel/framebuffer.o \
   kernel/fs.o \
   kernel/graphics.o \
   kernel/idt.o \
   kernel/keyboard.o \
   kernel/kmain.o \
   kernel/mouse.o \
   kernel/process.o \
   kernel/text.o \
   kernel/ui.o \
   kernel/window.o \
   kernel/libc/memmove.o \
   kernel/libc/printf.o \
   kernel/libc/stdio.o \
   kernel/libc/string.o \
   kernel/mm/mm.o \
   $LDFLAGS

if [ $? -eq 0 ]; then
    echo "Kernel built successfully!"
    
    # Create ISO
    echo "Creating bootable ISO..."
    mkdir -p iso/boot/grub
    cp kernel.bin iso/boot/
    cp boot/grub.cfg iso/boot/grub/ 2>/dev/null || echo "grub.cfg not found, creating minimal one..."
    
    # Create minimal grub.cfg if it doesn't exist
    if [ ! -f boot/grub.cfg ]; then
        mkdir -p boot
        cat > boot/grub.cfg << EOF
set timeout=0
set default=0

menuentry "MyOS" {
    multiboot2 /boot/kernel.bin
    boot
}
EOF
        cp boot/grub.cfg iso/boot/grub/
    fi
    
    grub-mkrescue -o myos.iso iso 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "ISO created successfully: myos.iso"
        echo "Run with: qemu-system-x86_64 -cdrom myos.iso -m 2G"
    else
        echo "Failed to create ISO"
    fi
    
    rm -rf iso
else
    echo "Build failed!"
    exit 1
fi
