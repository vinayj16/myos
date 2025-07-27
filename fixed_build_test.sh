#!/bin/bash

echo "🔧 MyOS Fixed Build Test"
echo "========================"

echo "Step 1: Clean build environment..."
make clean > /dev/null 2>&1

echo "Step 2: Building kernel..."
if make all; then
    echo "✅ Kernel build successful!"
    
    echo "Step 3: Creating ISO..."
    if make iso; then
        echo "✅ ISO created successfully!"
        
        echo "Step 4: Checking files..."
        echo "  kernel.bin: $(ls -lh kernel.bin 2>/dev/null | awk '{print $5}' || echo 'missing')"
        echo "  myos.iso: $(ls -lh myos.iso 2>/dev/null | awk '{print $5}' || echo 'missing')"
        
        echo ""
        echo "🚀 Starting MyOS..."
        echo ""
        echo "BOOT INSTRUCTIONS:"
        echo "1. GRUB menu will appear"
        echo "2. Select 'MyOS - Custom Operating System'"
        echo "3. Press ENTER"
        echo "4. Watch for kernel debug messages"
        echo "5. Desktop should appear after initialization"
        echo ""
        echo "DEBUG: Watch for these messages:"
        echo "  - MyOS Kernel Starting..."
        echo "  - Parsing multiboot tags..."
        echo "  - Framebuffer: [resolution] at [address]"
        echo "  - Graphics initialized."
        echo "  - Memory manager initialized."
        echo "  - Desktop initialized. Entering main loop..."
        echo ""
        
        read -p "Press Enter to boot MyOS..."
        
        # Boot with serial output for debugging
        qemu-system-x86_64 \
            -cdrom myos.iso \
            -m 2G \
            -smp 2 \
            -serial stdio \
            -vga std
            
    else
        echo "❌ ISO creation failed"
        echo "Trying to run kernel directly..."
        qemu-system-x86_64 -kernel kernel.bin -m 2G -serial stdio
    fi
else
    echo "❌ Build failed!"
    echo "Check the error messages above."
    exit 1
fi