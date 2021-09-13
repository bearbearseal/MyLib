OBJS = main.o SteadyEventTimer.o
CC = g++ -std=c++17
CFLAGS = -Wall -O2 -c
LFLAGS = -Wall
LIBS = -lpthread
#INC = -I../Basic
binaries = Test.bin

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

SteadyEventTimer.o : SteadyEventTimer.h SteadyEventTimer.cpp
	$(CC) $(CFLAGS) SteadyEventTimer.cpp

clean:
	rm -f $(binaries) *.o