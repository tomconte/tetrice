# Makefile for PHC-25 Proof of Concept using z88dk
#
# This builds a simple test program for the PHC-25 using the z88dk toolchain
# with the S-OS target and custom CRT0.

# z88dk installation path
Z88DK_PATH = C:/Users/tomco/src/z88dk

# Compiler and tools
ZCC = $(Z88DK_PATH)/bin/zcc
OBJDUMP = $(Z88DK_PATH)/bin/z88dk-dis

# Build configuration
TARGET = +sos
CRT0 = src/crt0.asm
SOURCES = src/main.c
OUTPUT = phc25_test

# Compiler flags
CFLAGS = #-O2 -Wall
LDFLAGS = -crt0=$(CRT0) -m

# Default target
all: $(OUTPUT).bin

# Build the binary
$(OUTPUT).bin: $(SOURCES) $(CRT0)
	$(ZCC) $(TARGET) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT).bin $(SOURCES)
	@echo "Build complete: $(OUTPUT).bin"
	@echo "Memory map saved to: $(OUTPUT).map"

# Disassemble the output for inspection
disasm: $(OUTPUT).bin
	$(OBJDUMP) $(OUTPUT).bin > $(OUTPUT).asm
	@echo "Disassembly saved to: $(OUTPUT).asm"

# Clean build artifacts
clean:
	$(RM) $(OUTPUT).bin $(OUTPUT).map $(OUTPUT).asm $(OUTPUT) *.o *.tmp

# Show build information
info:
	@echo "PHC-25 POC Build Configuration:"
	@echo "  z88dk path: $(Z88DK_PATH)"
	@echo "  Target: $(TARGET)"
	@echo "  CRT0: $(CRT0)"
	@echo "  Sources: $(SOURCES)"
	@echo "  Output: $(OUTPUT).bin"

.PHONY: all clean info disasm
