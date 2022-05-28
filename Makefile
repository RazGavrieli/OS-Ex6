CC=gcc
CXX=clang++-9 -std=c++2a
FLAGS=-pthread
OBJECTS=active_object.o  guard.o      main1.o       pollserver.o  reactor.o    testAOserver.o	AOclient.o  guardtest.o  pollclient.o  queue.o       singleton.o  testsingleton.o
HEADERS=$(wildcard *.hpp)
SOURCES1-3_PATH=sources1-3
SOURCES4-6_PATH=sources4-6

all: main1 AOclient testAO guardtest singletontest pollclient pollserver

## QUESTIONS 1 TO 3 ##

main1: main1.o queue.o active_object.o
	$(CC) $(FLAGS) main1.o queue.o active_object.o -o main1

main1.o: $(HEADERS)  $(SOURCES1-3_PATH)/main1.c 
	$(CC) $(FLAGS) -c $(SOURCES1-3_PATH)/main1.c

active_object.o: $(SOURCES1-3_PATH)/active_object.c $(HEADERS)
	$(CC) $(FLAGS) -c $(SOURCES1-3_PATH)/active_object.c

queue.o: $(SOURCES1-3_PATH)/queue.c $(HEADERS)
	$(CC) $(FLAGS) -c $(SOURCES1-3_PATH)/queue.c

AOclient: AOclient.o
	$(CXX) $(FLAGS) AOclient.o -o AOclient

AOclient.o: $(HEADERS)  $(SOURCES1-3_PATH)/AOclient.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES1-3_PATH)/AOclient.cpp 

testAO: testAOserver.o
	$(CXX) $(FLAGS) testAOserver.o -o testAO

testAOserver.o: $(HEADERS) $(SOURCES1-3_PATH)/testAOserver.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES1-3_PATH)/testAOserver.cpp 

## QUESTION 4 ##
guard.o: $(HEADERS)  $(SOURCES4-6_PATH)/guard.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/guard.cpp 

guardtest.o: $(HEADERS)  $(SOURCES4-6_PATH)/guardtest.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/guardtest.cpp 

guardtest: guard.o guardtest.o
	$(CXX) $(FLAGS) guardtest.o guard.o -o guardtest

## QUESTION 5  ##
singleton.o: $(HEADERS)  $(SOURCES4-6_PATH)/singleton.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/singleton.cpp 

testsingleton.o: $(HEADERS)  $(SOURCES4-6_PATH)/testsingleton.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/testsingleton.cpp 

singletontest: singleton.o testsingleton.o
	$(CXX) $(FLAGS) testsingleton.o singleton.o -o singletontest

## QUESTION 6  ##

reactor.o: $(HEADERS)  $(SOURCES4-6_PATH)/reactor.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/reactor.cpp 

pollserver.o: $(HEADERS)  $(SOURCES4-6_PATH)/pollserver.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/pollserver.cpp 

pollserver: pollserver.o reactor.o
	$(CXX) $(FLAGS) pollserver.o reactor.o -o pollserver

pollclient.o: $(HEADERS)  $(SOURCES4-6_PATH)/pollclient.cpp 
	$(CXX) $(FLAGS) -c $(SOURCES4-6_PATH)/pollclient.cpp 

pollclient: pollclient.o
	$(CXX) $(FLAGS) pollclient.o  -o pollclient

# ### library requirement ###
Ex6: $(OBJECTS)
	gcc --shared -fPIC -pthread $(OBJECTS) 

clean: 
	rm -f *.o main1 testAO AOclient guardtest pollclient pollserver singletontest
 