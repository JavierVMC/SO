all: chasquiEats client restaurant

chasquiEats: myFunctions.o chasquiEats.o queue.o
	gcc chasquiEats.o myFunctions.o queue.o -o chasquiEats -lrt

client: myFunctions.o queue.o client.o
	gcc client.o myFunctions.o queue.o -o client -lrt -lpthread

restaurant: myFunctions.o queue.o restaurant.o
	gcc restaurant.o myFunctions.o queue.o -o restaurant -lrt -lpthread

chasquiEats.o: chasquiEats.c
	gcc -c chasquiEats.c

client.o: client.c
	gcc -c client.c

restaurant.o: restaurant.c
	gcc -c restaurant.c

myFunctions.o: myFunctions.c
	gcc -c myFunctions.c

queue.o: queue.c
	gcc -c queue.c

.PHONY: clean

clean:
	rm -f chasquiEats client restaurant *.o