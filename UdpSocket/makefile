OBJS = main.o UdpSocket.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lpthread
#INC = -I../Basic
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

UdpSocket.o : UdpSocket.h UdpSocket.cpp
	$(CC) $(CFLAGS) UdpSocket.cpp

clean:
	rm -f $(binaries) *.o