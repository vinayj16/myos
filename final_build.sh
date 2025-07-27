#!/bin/bash

echo "🎯 Final MyOS Build"
echo "==================="

echo "Cleaning previous build..."
make clean > /dev/null 2>&1

echo "Building kernel..."
if make all 2>&1; then
    echo ""
    echo "✅ BUILD SUCCESSFUL!"
    
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin: $(ls -lh kernel.bin | awk '{print $5}')"
        
        echo "Creating ISO..."
        if make iso 2>&1; then
            echo "✅ myos.iso: $(ls -lh myos.iso | awk '{print $5}')"
            
            echo ""
            echo "🚀 READY TO RUN!"
            echo ""
            echo "Starting MyOS in QEMU..."
            echo "- GRUB menu will appear"
            echo "- Select 'MyOS - Custom Operating System'"
            echo "- Press ENTER to boot"
            echo "- Watch for kernel debug messages"
            echo "- Desktop should appear in VGA mode (320x200)"
            echo ""
            echo "Press Ctrl+Alt+G to release mouse from QEMU"
            echo "Press Ctrl+C in terminal to exit QEMU"
            echo ""
            
            # Start QEMU with VGA mode
            qemu-system-x86_64 \
                -cdrom myos.iso \
                -m 2G \
                -smp 2 \
                -vga std \
                -serial stdio
                
        else
            echo "⚠️  ISO creation failed, trying direct kernel boot..."
            qemu-system-x86_64 -kernel kernel.bin -m 2G -serial stdio
        fi
    else
        echo "❌ kernel.bin not found"
        exit 1
    fi
else
    echo ""
    echo "❌ BUILD FAILED"
    echo "Check error messages above"
    exit 1
fi