OBJS = main.o Sqlite3.o Sqlite3Listener.o TcpListener.o TcpSocket.o
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

Sqlite3Listener.o: Sqlite3Listener.h Sqlite3Listener.cpp
	$(CC) $(CFLAGS) Sqlite3Listener.cpp

TcpListener.o: ../TcpSocket/TcpListener.h ../TcpSocket/TcpListener.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpListener.cpp

TcpSocket.o: ../TcpSocket/TcpSocket.h ../TcpSocket/TcpSocket.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpSocket.cpp

clean:
	rm -f $(binaries) *.o
