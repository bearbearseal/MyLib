OBJS = main.o 
#OBJS += PrioritizedValue.o Value.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lpthread
#INC = -I../Basic
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp HashKey.h
	$(CC) $(CFLAGS) main.cpp

PrioritizedValue.o : PrioritizedValue.h PrioritizedValue.cpp
	$(CC) $(CFLAGS) PrioritizedValue.cpp

Value.o : Value.h Value.cpp
	$(CC) $(CFLAGS) Value.cpp
