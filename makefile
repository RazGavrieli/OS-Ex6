#!make -f

CXX=clang++-9
CXXFLAGS=-pthread -std=c++2a
OBJECTS=server.cpp stack.c 

VALGRIND_FLAGS=--leak-check=full

# test: testC stack
# 	$(CC) stack.o test.o -o test

# testC: 
# 	$(CC) test.c -c

all: server
	./$^

client: clientC
	./client localhost
	
clientC: client.cpp
	$(CXX) $^ -o client

server: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o server

stack: stack.c
	$(CC) stack.c stack.h -c 

main1: main1.o queue.o active_object.o
	$(CC) main1.o queue.o active_object.o -lpthread -o main

main.o: main1.c
	$(CC) main1.c -c 

queue.o: queue.c
	$(CC) queue.c -c 

active_object.o: active_object.c
	$(CC) active_object.c -c 

valgrind: server
	valgrind $(VALGRIND_FLAGS) ./server

clean:
	rm -f *.o main server ser cli a.out client original stack.h.gch tests test locker.txt *.gch