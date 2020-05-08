out: elect.o 
	gcc elect.o -o out

elect.o: elect.c
	gcc -c elect.c -lpthread

clean:
	rm *.o out