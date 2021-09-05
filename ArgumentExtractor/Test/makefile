OBJS = main.o ArgumentExtractor.o
CC = g++ -std=c++17
CFLAGS = -Wall -O2 -c
LFLAGS = -Wall
binaries = ArgExt.bin

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

ArgumentExtractor.o : ../ArgumentExtractor.h ../ArgumentExtractor.cpp
	$(CC) $(CFLAGS) ../ArgumentExtractor.cpp

clean:
	rm -f $(binaries) *.o
