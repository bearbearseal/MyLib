OBJS = main.o TcpSocket.o
CC = g++ -std=c++17 -Wno-psabi
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lpthread
binaries = theMain

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

TcpSocket.o: TcpSocket.h TcpSocket.cpp
	$(CC) $(CFLAGS) TcpSocket.cpp

clean:
	rm -f $(binaries) *.o
