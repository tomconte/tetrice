# Set path
CC68 = /opt/cc68
ADDR = 14150

all:
	$(CC68)/lib/cc68 -I $(CC68)/include/mc10/ -I $(CC68)/include/ -r --add-source --cpu 6803 -D__6803__ -D__TANDY_MC10__ tetrice.c > temp.s
	$(CC68)/lib/copt $(CC68)/lib/cc68.rules < temp.s > tetrice.s
	$(CC68)/bin/as68 tetrice.s
	$(CC68)/bin/ld68 -b -C $(ADDR) -Z 0x90 -o tetrice $(CC68)/lib/crt0_mc10.o tetrice.o $(CC68)/lib/libc.a $(CC68)/lib/libio6803.a $(CC68)/lib/libmc10.a $(CC68)/lib/lib6803.a

	wlen=$$(expr $$(wc -c < tetrice | awk '{print $1}') - $(ADDR)); \
	$(CC68)/lib/mc10-tapeify tetrice tetrice.c10 $(ADDR) $$wlen $(ADDR)

	mv tetrice.c10 tetrice.k7

clean:
	rm -f *.o *.s a.out a.out.k7 tetrice.k7
