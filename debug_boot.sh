#!/bin/bash

echo "🔍 MyOS Boot Diagnostics"
echo "========================"

# Build with debug symbols
echo "Building debug version..."
make clean > /dev/null 2>&1
make CFLAGS="-DDEBUG -g" all

if [ $? -eq 0 ]; then
    echo "✅ Debug build successful"
    
    echo ""
    echo "🚀 Starting MyOS with debug output..."
    echo "Watch for kernel messages during boot..."
    echo ""
    echo "Expected boot sequence:"
    echo "1. GRUB loads kernel"
    echo "2. Kernel initialization messages"
    echo "3. Memory manager init"
    echo "4. Graphics initialization"
    echo "5. Desktop environment starts"
    echo ""
    echo "If system hangs, press Ctrl+C to exit QEMU"
    echo ""
    
    # Run with debug output
    qemu-system-x86_64 -cdrom myos.iso -m 2G -smp 2 -serial stdio -d cpu_reset
else
    echo "❌ Debug build failed"
    exit 1
fi
