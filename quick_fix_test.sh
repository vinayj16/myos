#!/bin/bash

echo "🔧 Quick Fix Test"
echo "================="

echo "Cleaning..."
make clean > /dev/null 2>&1

echo "Building with IRQ stubs..."
if make all; then
    echo ""
    echo "✅ BUILD SUCCESSFUL!"
    
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin: $(ls -lh kernel.bin | awk '{print $5}')"
        
        echo "Creating ISO..."
        if make iso; then
            echo "✅ myos.iso: $(ls -lh myos.iso | awk '{print $5}')"
            
            echo ""
            echo "🚀 Testing boot..."
            echo "If you see GRUB menu, press ENTER to boot MyOS"
            echo "Look for 'MyOS Kernel Starting...' message"
            echo ""
            
            # Quick boot test
            timeout 20s qemu-system-x86_64 \
                -cdrom myos.iso \
                -m 512M \
                -serial stdio \
                -no-reboot || echo "Boot test completed"
                
        else
            echo "❌ ISO creation failed"
        fi
    else
        echo "❌ kernel.bin not found"
    fi
else
    echo "❌ Build failed"
    exit 1
fi