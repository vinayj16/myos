#!/bin/bash

echo "🧪 Simple MyOS Boot Test"
echo "========================"

echo "Step 1: Clean build..."
make clean > /dev/null 2>&1

echo "Step 2: Build with fixed boot loader..."
if make all; then
    echo "✅ Build successful!"
    
    echo "Step 3: Check kernel binary..."
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin: $(ls -lh kernel.bin | awk '{print $5}')"
        
        # Check if it's a valid ELF file
        file kernel.bin
        
        echo "Step 4: Create ISO..."
        if make iso; then
            echo "✅ ISO created: $(ls -lh myos.iso | awk '{print $5}')"
            
            echo ""
            echo "Step 5: Test boot with verbose output..."
            echo "Watch for:"
            echo "- GRUB loading kernel"
            echo "- Multiboot handoff"
            echo "- Kernel initialization messages"
            echo ""
            echo "If you see 'MyOS Kernel Starting...' then it's working!"
            echo ""
            
            # Try booting with verbose output
            timeout 30s qemu-system-x86_64 \
                -cdrom myos.iso \
                -m 512M \
                -serial stdio \
                -d guest_errors \
                -no-reboot \
                -no-shutdown || echo "Boot test completed"
                
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