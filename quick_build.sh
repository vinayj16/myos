#!/bin/bash

echo "🚀 Quick MyOS Build"
echo "==================="

echo "Cleaning..."
make clean > /dev/null 2>&1

echo "Building..."
if make all; then
    echo ""
    echo "✅ Build successful!"
    
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin: $(ls -lh kernel.bin | awk '{print $5}')"
        
        echo "Creating ISO..."
        if make iso; then
            echo "✅ myos.iso: $(ls -lh myos.iso | awk '{print $5}')"
            
            echo ""
            echo "🎮 Ready to run!"
            echo "Command: make run"
            echo ""
            echo "Or manually:"
            echo "qemu-system-x86_64 -cdrom myos.iso -m 2G"
            
        else
            echo "⚠️  ISO creation failed, but kernel.bin is ready"
        fi
    else
        echo "❌ kernel.bin not found"
    fi
else
    echo "❌ Build failed"
    exit 1
fi
