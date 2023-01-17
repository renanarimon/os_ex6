.PHONY: all clean
CC=gcc
CPP=g++
all: server client pollServer pollClient

server: server.o
	gcc server.o -o server -lpthread
client: client.o 
	gcc client.o -o client
server.o: server.c 
	gcc -c server.c 
client.o: client.c 
	gcc -c client.c
pollServer: pollServer.o
	$(CPP) pollserver.cpp reactor.cpp -o pollServer -pthread -lpthread
pollServer.o: pollserver.cpp
	$(CPP) -c pollserver.cpp
pollClient: pollclient.o
	$(CPP) pollclient.cpp -o pollClient -lpthread
pollclient.o: pollclient.cpp
	$(CPP) -c pollclient.cpp


clean :
	rm -f *.o client server pollClient pollServer 