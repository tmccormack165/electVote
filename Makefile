out: elect.o 
	gcc elect.o -o out

elect.o: elect.c
	gcc -c elect.c

clean:
	rm *.o out