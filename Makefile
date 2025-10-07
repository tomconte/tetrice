# Cross-platform commands
UNAME_S := $(shell uname -s 2>nul || echo Windows)

ifneq (,$(findstring CYGWIN,$(UNAME_S)))
    RM = rm -f
    CP = cp
    MV = mv
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    RM = rm -f
    CP = cp
    MV = mv
else ifneq (,$(findstring MSYS,$(UNAME_S)))
    RM = rm -f
    CP = cp
    MV = mv
else ifeq ($(UNAME_S),Linux)
    RM = rm -f
    CP = cp
    MV = mv
else ifeq ($(UNAME_S),Darwin)
    RM = rm -f
    CP = cp
    MV = mv
else
    # Native Windows
    RM = del /Q
    CP = copy
    MV = move
endif

# Makefile for tetrice - supports alice and phc25 targets
TARGET ?= alice
SRC = tetrice.c

# Alice target configuration
CC68 = /opt/cc68
ALICE_ADDR = 14150
ALICE_PLATFORM_SRC = platform_alice.c
ALICE_FLAGS = -DALICE

# PHC25 target configuration using z88dk
Z88DK_PATH = C:/Users/tomco/src/z88dk
PHC25_ZCC = $(Z88DK_PATH)/bin/zcc
PHC25_TARGET = +sos
PHC25_CRT0 = crt0_phc25.asm
PHC25_ADDR = 49161
PHC25_PLATFORM_SRC = platform_phc25.c #debug_font.c
PHC25_FLAGS = -DPHC25
BIN_TO_PHC = C:/Users/tomco/src/phc25/phc25_tools/bin_to_phc/bin_to_phc.exe

.PHONY: all clean alice phc25

all: tetrice.k7 tetrice.phc

# Target-specific builds
alice: TARGET = alice
alice: tetrice.k7

phc25: TARGET = phc25
phc25: tetrice.phc

# Alice build process
ifeq ($(TARGET),alice)
tetrice.k7: tetrice_alice
	$(MV) tetrice.c10 tetrice.k7

tetrice_alice:
	$(CC68)/lib/cc68 -I $(CC68)/include/mc10/ -I $(CC68)/include/ -r --add-source --cpu 6803 -D__6803__ -D__TANDY_MC10__ $(ALICE_FLAGS) $(SRC) > tetrice_temp.s
	$(CC68)/lib/cc68 -I $(CC68)/include/mc10/ -I $(CC68)/include/ -r --add-source --cpu 6803 -D__6803__ -D__TANDY_MC10__ $(ALICE_FLAGS) $(ALICE_PLATFORM_SRC) > platform_alice_temp.s
	$(CC68)/lib/copt $(CC68)/lib/cc68.rules < tetrice_temp.s > tetrice.s
	$(CC68)/lib/copt $(CC68)/lib/cc68.rules < platform_alice_temp.s > platform_alice.s
	$(CC68)/bin/as68 tetrice.s
	$(CC68)/bin/as68 platform_alice.s
	$(CC68)/bin/ld68 -b -C $(ALICE_ADDR) -Z 0x90 -o tetrice $(CC68)/lib/crt0_mc10.o tetrice.o platform_alice.o $(CC68)/lib/libc.a $(CC68)/lib/libio6803.a $(CC68)/lib/libmc10.a $(CC68)/lib/lib6803.a
	wlen=$$(expr $$(wc -c < tetrice | awk '{print $$1}') - $(ALICE_ADDR)); $(CC68)/lib/mc10-tapeify tetrice tetrice.c10 $(ALICE_ADDR) $$wlen $(ALICE_ADDR)

else ifeq ($(TARGET),phc25)
# PHC25 build process using z88dk
tetrice.phc: tetrice_phc25
	$(MV) tetrice_code_compiler.bin tetrice.bin
	$(BIN_TO_PHC) phetrice .\tetrice.bin .\tetrice.phc

tetrice_phc25:
	$(PHC25_ZCC) $(PHC25_TARGET) $(PHC25_FLAGS) -SO3 --opt-code-size -DNDEBUG -crt0=$(PHC25_CRT0) -m -o tetrice $(SRC) $(PHC25_PLATFORM_SRC)

else
$(error Unknown target: $(TARGET). Use 'alice' or 'phc25')
endif

clean:
	$(RM) *.o *.s a.out a.out.k7 tetrice.k7 tetrice_temp.s tetrice.s platform_alice_temp.s platform_alice.s tetrice.c10 tetrice.bin tetrice.map tetrice_code_compiler.bin tetrice.phc tetrice

# Help target
help:
	@echo "Available targets:"
	@echo "  alice  - Build for Alice platform (default)"
	@echo "  phc25  - Build for PHC25 platform"
	@echo "  clean  - Remove build artifacts"
	@echo ""
	@echo "Usage: make [TARGET=alice|phc25]"

