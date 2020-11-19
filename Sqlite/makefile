OBJS = main.o Sqlite3.o Sqlite3JsonTalker.o
OBJS += Sqlite3TcpListener.o TcpListener.o TcpSocket.o
OBJS += Sqlite3UdpListener.o UdpListener.o UdpSocket.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lsqlite3 -lpthread
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp Test.hpp
	$(CC) $(CFLAGS) main.cpp

Sqlite3.o : Sqlite3.h Sqlite3.cpp
	$(CC) $(CFLAGS) Sqlite3.cpp

Sqlite3JsonTalker.o: Sqlite3JsonTalker.h Sqlite3JsonTalker.cpp
	$(CC) $(CFLAGS) Sqlite3JsonTalker.cpp

Sqlite3TcpListener.o: Sqlite3TcpListener.h Sqlite3TcpListener.cpp
	$(CC) $(CFLAGS) Sqlite3TcpListener.cpp

TcpListener.o: ../TcpSocket/TcpListener.h ../TcpSocket/TcpListener.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpListener.cpp

TcpSocket.o: ../TcpSocket/TcpSocket.h ../TcpSocket/TcpSocket.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpSocket.cpp

Sqlite3UdpListener.o: Sqlite3UdpListener.h Sqlite3UdpListener.cpp
	$(CC) $(CFLAGS) Sqlite3UdpListener.cpp

UdpListener.o: ../UdpSocket/UdpListener.h ../UdpSocket/UdpListener.cpp
	$(CC) $(CFLAGS) ../UdpSocket/UdpListener.cpp

UdpSocket.o: ../UdpSocket/UdpSocket.h ../UdpSocket/UdpSocket.cpp
	$(CC) $(CFLAGS) ../UdpSocket/UdpSocket.cpp

clean:
	rm -f $(binaries) *.o
