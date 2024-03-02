all:
	cc68 -tmc10 tetrice.c -o tetrice
	mv tetrice.c10 tetrice.k7

clean:
	rm -f *.o a.out a.out.k7 tetrice.k7
