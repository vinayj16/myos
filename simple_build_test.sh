#!/bin/bash

echo "🚀 Simple build test..."

# Clean everything
echo "Cleaning..."
make clean > /dev/null 2>&1
rm -f kernel/*.o kernel/libc/*.o kernel/mm/*.o kernel/arch/x86_64/*.o
rm -f kernel.bin myos.iso

# Try building again
echo "Building..."
make all

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin created ($(ls -lh kernel.bin | awk '{print $5}'))"
        
        # Try to create ISO
        if make iso > /dev/null 2>&1; then
            echo "✅ ISO created successfully"
            if [ -f "myos.iso" ]; then
                echo "✅ myos.iso ready ($(ls -lh myos.iso | awk '{print $5}'))"
                echo ""
                echo "🎉 MyOS is ready to run!"
                echo "Command: qemu-system-x86_64 -cdrom myos.iso -m 2G"
            fi
        else
            echo "⚠️  ISO creation failed (grub tools may not be available)"
            echo "But kernel.bin is ready for testing"
        fi
    fi
else
    echo "❌ Build failed"
    exit 1
fi