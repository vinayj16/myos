#!/bin/bash

echo "🔧 Quick build test for MyOS..."

# Clean and build
make clean > /dev/null 2>&1
echo "Cleaned previous build"

echo "Attempting to build..."
if make all 2>&1 | tee build.log; then
    echo "✅ Build successful!"
    
    if [ -f "kernel.bin" ]; then
        echo "✅ kernel.bin created ($(ls -lh kernel.bin | awk '{print $5}'))"
    else
        echo "❌ kernel.bin not found"
        exit 1
    fi
    
    echo "🎉 MyOS build completed successfully!"
    echo "To run: make run"
    
else
    echo "❌ Build failed. Check build.log for details."
    echo "Last few lines of build log:"
    tail -10 build.log
    exit 1
fi