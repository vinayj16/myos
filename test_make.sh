#!/bin/bash

echo "🔍 Testing make with verbose output..."

# Clean first
make clean

# Try to build with verbose output
echo "Running make with verbose output..."
make V=1 all 2>&1 | tee make_verbose.log

echo ""
echo "Build result: $?"

if [ -f "kernel.bin" ]; then
    echo "✅ kernel.bin created successfully!"
    ls -la kernel.bin
else
    echo "❌ kernel.bin not found"
    echo ""
    echo "Last 20 lines of build log:"
    tail -20 make_verbose.log
fi