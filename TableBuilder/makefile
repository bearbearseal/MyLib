OBJS = main.o FileIOer.o ArgumentExtractor.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
#LIBS = -lsqlite3 -lpthread
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

FileIOer.o: ../File/FileIOer.h ../File/FileIOer.cpp
	$(CC) $(CFLAGS) ../File/FileIOer.cpp

ArgumentExtractor.o: ../ArgumentExtractor/ArgumentExtractor.h ../ArgumentExtractor/ArgumentExtractor.cpp
	$(CC) $(CFLAGS) ../ArgumentExtractor/ArgumentExtractor.cpp

clean:
	rm -f $(binaries) *.o