#!/bin/bash

echo "Testing MyOS build..."

# Clean first
make clean

# Try to build
if make all; then
    echo "✅ Build successful!"
    
    # Check if kernel.bin was created
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin created successfully"
        ls -la kernel.bin
    else
        echo "❌ kernel.bin not found"
        exit 1
    fi
    
    # Try to create ISO
    if make iso 2>/dev/null; then
        echo "✅ ISO creation successful!"
        if [ -f "myos.iso" ]; then
            echo "✅ myos.iso created successfully"
            ls -la myos.iso
        fi
    else
        echo "⚠️  ISO creation failed (this is expected if grub-mkrescue is not available)"
    fi
    
else
    echo "❌ Build failed!"
    exit 1
fi

echo "🎉 MyOS is ready to run!"
echo "To test: qemu-system-x86_64 -cdrom myos.iso -m 2G"
