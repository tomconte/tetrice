# PHC-25 Proof of Concept

This directory contains a simple proof-of-concept program to test z88dk compilation for the Thomson PHC-25 microcomputer.

## Overview

The POC program demonstrates:
- Basic z88dk compilation using the S-OS target
- Custom CRT0 assembly file for PHC-25 memory layout
- Mode 11 graphics initialization (128x192, 4-color)
- Direct VRAM pixel plotting
- Simple test pattern display

## Files

- `src/main.c` - Main C program with graphics test
- `src/crt0.asm` - Minimal startup code for PHC-25
- `Makefile` - Build configuration for z88dk
- `README.md` - This file

## Building

Make sure z88dk is installed at `C:\Users\tomco\src\z88dk` (update Makefile if different).

```bash
# Build the program
make

# Test compilation only (no linking)
make test-compile

# Generate disassembly
make disasm

# Clean build artifacts
make clean

# Show build configuration
make info
```

## Expected Output

The program should generate:
- `phc25_test.bin` - Binary file ready for PHC-25
- `phc25_test.map` - Memory map showing code layout

## Test Pattern

When run on PHC-25, the program should display:
- Colored rectangles in each corner (yellow, blue, red, checkerboard)
- Red border around the entire screen
- Background filled with green

## Memory Layout

- Origin: $C000 (Program Work Area start)
- VRAM: $6000-$77FF (Screen page 1)
- Graphics Mode: Mode 11 (128x192, 4-color)

## Next Steps

Once this POC compiles successfully, the same approach can be used for the full Tetris implementation with the platform abstraction layer.