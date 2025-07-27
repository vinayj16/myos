#!/bin/bash

echo "🎯 Working Boot Test"
echo "==================="

echo "Step 1: Clean build..."
make clean > /dev/null 2>&1

echo "Step 2: Build with working multiboot..."
if make all; then
    echo "✅ Build successful!"
    
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin: $(ls -lh kernel.bin | awk '{print $5}')"
        
        # Check the binary format
        echo "Binary info:"
        file kernel.bin
        
        echo ""
        echo "Step 3: Create ISO..."
        if make iso; then
            echo "✅ ISO created: $(ls -lh myos.iso | awk '{print $5}')"
            
            echo ""
            echo "Step 4: Boot test..."
            echo "This should now work without GRUB errors!"
            echo "Look for 'MyOS Kernel Starting...' message"
            echo ""
            echo "Press any key in QEMU to continue if it stops"
            echo "Press Ctrl+C here to exit"
            echo ""
            
            # Boot test
            qemu-system-x86_64 \
                -cdrom myos.iso \
                -m 512M \
                -serial stdio \
                -no-reboot \
                -no-shutdown
                
        else
            echo "❌ ISO creation failed"
            exit 1
        fi
    else
        echo "❌ kernel.bin not found"
        exit 1
    fi
else
    echo "❌ Build failed"
    exit 1
fi