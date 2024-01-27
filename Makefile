all:
	cc68 -tmc10 tetrice.c
	mv a.out.c10 a.out.k7

clean:
	rm *.o a.out a.out.k7
