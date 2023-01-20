all: barber 

barber: barber.o
	gcc barber.o -o barber -lm -lpthread

barber.o: barber.c
	gcc -c barber.c -lm -lpthread

clean:
	rm *.o barber 