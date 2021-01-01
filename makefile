exec: BmpLib.o OutilsLib.o image.o main.o
	gcc -Wall BmpLib.o OutilsLib.o image.o main.o -o exec -lm

main.o: main.c image.h wav.h
	gcc -c -Wall main.c

image.o: image.c lib/BmpLib.h lib/OutilsLib.h image.h
	gcc -c -Wall image.c

BmpLib.o: lib/BmpLib.c lib/BmpLib.h lib/OutilsLib.h
	gcc -c -Wall lib/BmpLib.c

OutilsLib.o: lib/OutilsLib.c lib/BmpLib.h lib/OutilsLib.h
	gcc -c -Wall lib/OutilsLib.c