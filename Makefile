# Makefile for tetrice - supports alice and phc25 targets
TARGET ?= alice
SRC = tetrice.c

# Alice target configuration
CC68 = /opt/cc68
ALICE_ADDR = 14150
ALICE_PLATFORM_SRC = platform_alice.c
ALICE_FLAGS = -DALICE

# PHC25 target configuration (update these paths/flags as needed)
PHC25_CC = /path/to/phc25/compiler
PHC25_ADDR = 8000
PHC25_PLATFORM_SRC = platform_phc25.c
PHC25_FLAGS = -DPHC25

.PHONY: all clean alice phc25

all: tetrice.k7

# Target-specific builds
alice: TARGET = alice
alice: tetrice.k7

phc25: TARGET = phc25
phc25: tetrice.k7

# Alice build process
ifeq ($(TARGET),alice)
tetrice.k7: tetrice_alice
	mv tetrice.c10 tetrice.k7

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
# PHC25 build process
tetrice.k7: tetrice_phc25
	# Add any PHC25-specific final steps here
	cp tetrice_phc25.bin tetrice.k7

tetrice_phc25:
	# Replace these with actual PHC25 toolchain commands
	$(PHC25_CC) $(PHC25_FLAGS) -o tetrice_main.o -c $(SRC)
	$(PHC25_CC) $(PHC25_FLAGS) -o tetrice_platform.o -c $(PHC25_PLATFORM_SRC)
	# Add linking step for PHC25
	# $(PHC25_LINKER) -o tetrice_phc25.bin tetrice_main.o tetrice_platform.o
	# Add any PHC25-specific post-processing steps
	@echo "PHC25 build process - update with actual toolchain commands"

else
$(error Unknown target: $(TARGET). Use 'alice' or 'phc25')
endif

clean:
	rm -f *.o *.s a.out a.out.k7 tetrice.k7 tetrice_temp.s tetrice.s platform_alice_temp.s platform_alice.s tetrice.c10 tetrice_phc25.bin

# Help target
help:
	@echo "Available targets:"
	@echo "  alice  - Build for Alice platform (default)"
	@echo "  phc25  - Build for PHC25 platform"
	@echo "  clean  - Remove build artifacts"
	@echo ""
	@echo "Usage: make [TARGET=alice|phc25]"

