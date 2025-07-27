#!/bin/bash

echo "🔍 Debug build for MyOS..."

# Clean first
make clean

echo "📋 Checking source files..."
echo "Kernel sources:"
ls -la kernel/*.c | wc -l
echo "LIBC sources:"
ls -la kernel/libc/*.c | wc -l
echo "MM sources:"
ls -la kernel/mm/*.c | wc -l

echo ""
echo "🔨 Building step by step..."

# Build assembly first
echo "Building boot.s..."
nasm -f elf64 kernel/arch/x86_64/boot.s -o kernel/arch/x86_64/boot.o
if [ $? -ne 0 ]; then
    echo "❌ Assembly failed"
    exit 1
fi

# Build C files
CFLAGS="-ffreestanding -O2 -Wall -Wextra -m64 -nostdlib -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -mno-red-zone -Iinclude -Ikernel/mm -Ikernel/libc -std=gnu11 -fno-stack-protector -fno-omit-frame-pointer -mcmodel=kernel"

echo "Building kernel C files..."
for file in kernel/*.c; do
    if [ "$file" != "kernel/ps2.c" ]; then
        echo "  Compiling $file..."
        gcc $CFLAGS -c "$file" -o "${file%.c}.o"
        if [ $? -ne 0 ]; then
            echo "❌ Failed to compile $file"
            exit 1
        fi
    fi
done

echo "Building libc files..."
for file in kernel/libc/*.c; do
    echo "  Compiling $file..."
    gcc $CFLAGS -c "$file" -o "${file%.c}.o"
    if [ $? -ne 0 ]; then
        echo "❌ Failed to compile $file"
        exit 1
    fi
done

echo "Building mm files..."
for file in kernel/mm/*.c; do
    echo "  Compiling $file..."
    gcc $CFLAGS -c "$file" -o "${file%.c}.o"
    if [ $? -ne 0 ]; then
        echo "❌ Failed to compile $file"
        exit 1
    fi
done

echo ""
echo "📋 Checking object files..."
echo "Kernel objects:"
ls -la kernel/*.o | wc -l
echo "LIBC objects:"
ls -la kernel/libc/*.o | wc -l
echo "MM objects:"
ls -la kernel/mm/*.o | wc -l

echo ""
echo "🔗 Linking kernel..."
LDFLAGS="-nostdlib -z max-page-size=0x1000 -static -Bsymbolic --no-undefined --entry=_start"

# Collect all object files
OBJ_FILES="kernel/arch/x86_64/boot.o"
OBJ_FILES="$OBJ_FILES $(ls kernel/*.o | grep -v ps2.o | tr '\n' ' ')"
OBJ_FILES="$OBJ_FILES $(ls kernel/libc/*.o | tr '\n' ' ')"
OBJ_FILES="$OBJ_FILES $(ls kernel/mm/*.o | tr '\n' ' ')"

echo "Object files to link:"
echo "$OBJ_FILES"

ld -T linker.ld -o kernel.bin $OBJ_FILES $LDFLAGS

if [ $? -eq 0 ]; then
    echo "✅ Kernel linked successfully!"
    ls -la kernel.bin
    
    echo ""
    echo "🎉 Build completed successfully!"
    echo "To run: qemu-system-x86_64 -kernel kernel.bin -m 2G"
else
    echo "❌ Linking failed"
    exit 1
fi