OBJS = main.o StringManipulator.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

StringManipulator.o : StringManipulator.h StringManipulator.cpp
	$(CC) $(CFLAGS) StringManipulator.cpp

clean:
	rm -f $(binaries) *.o
