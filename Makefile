all: a31 a32 a33

a31:
	cc -std=c99 -Wall -fopenmp -o a31 a31.c

a32:
	cc -std=c99 -Wall -fopenmp -o a32 a32.c

a33:
	cc -std=c99 -Wall -fopenmp -lm -o a33 a33.c

clean:
	rm a31 a32 a33