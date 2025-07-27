#!/bin/bash

echo "🧪 MyOS Boot Test"
echo "=================="

echo "Testing kernel binary..."
if [ -f "kernel.bin" ]; then
    echo "✅ kernel.bin exists ($(ls -lh kernel.bin | awk '{print $5}'))"
else
    echo "❌ kernel.bin missing - rebuilding..."
    make clean && make all
fi

echo ""
echo "Testing ISO file..."
if [ -f "myos.iso" ]; then
    echo "✅ myos.iso exists ($(ls -lh myos.iso | awk '{print $5}'))"
else
    echo "❌ myos.iso missing - creating..."
    make iso
fi

echo ""
echo "🚀 Starting MyOS with minimal QEMU options..."
echo ""
echo "INSTRUCTIONS:"
echo "1. Wait for GRUB menu to appear"
echo "2. Press ENTER to select 'MyOS - Custom Operating System'"
echo "3. Watch for any error messages"
echo "4. If screen goes black, wait 10-15 seconds"
echo "5. Press Ctrl+Alt+G to release mouse if needed"
echo "6. Press Ctrl+C in terminal to exit if stuck"
echo ""

read -p "Press Enter to start MyOS..."

# Start with basic options and serial output
qemu-system-x86_64 \
    -cdrom myos.iso \
    -m 2G \
    -serial stdio \
    -no-reboot \
    -no-shutdown