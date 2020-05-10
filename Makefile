out : electThread.c
	gcc  electThread.c -o opt -lpthread -w
	gcc elect.c -o naive -w

clean:
	rm  naive opt









