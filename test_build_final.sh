#!/bin/bash

echo "🔥 Final MyOS Build Test"
echo "========================"

echo "Cleaning..."
make clean > /dev/null 2>&1

echo "Building..."
if make all; then
    echo ""
    echo "🎉 BUILD SUCCESSFUL!"
    
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin created: $(ls -lh kernel.bin | awk '{print $5}')"
        
        echo "Creating ISO..."
        if make iso; then
            echo "✅ myos.iso created: $(ls -lh myos.iso | awk '{print $5}')"
            
            echo ""
            echo "🚀 LAUNCHING MyOS!"
            echo ""
            echo "What you should see:"
            echo "1. GRUB bootloader menu"
            echo "2. Select 'MyOS - Custom Operating System'"
            echo "3. Press ENTER to boot"
            echo "4. Kernel debug messages"
            echo "5. VGA desktop environment (320x200)"
            echo ""
            echo "Desktop features:"
            echo "- Blue gradient background"
            echo "- Desktop icons (Files, About, Calc, Settings)"
            echo "- Taskbar with Start button"
            echo "- Mouse cursor (simulated movement)"
            echo "- Clickable applications"
            echo ""
            echo "Controls:"
            echo "- Click icons to launch apps"
            echo "- Use calculator with mouse clicks"
            echo "- Type in notepad applications"
            echo "- Press Ctrl+Alt+G to release mouse"
            echo "- Press Ctrl+C to exit QEMU"
            echo ""
            
            # Launch QEMU
            qemu-system-x86_64 \
                -cdrom myos.iso \
                -m 2G \
                -smp 2 \
                -vga std \
                -serial stdio \
                -no-reboot
                
        else
            echo "⚠️  ISO creation failed, trying direct boot..."
            qemu-system-x86_64 -kernel kernel.bin -m 2G -serial stdio
        fi
    else
        echo "❌ kernel.bin not found"
        exit 1
    fi
else
    echo ""
    echo "❌ BUILD FAILED"
    exit 1
fi