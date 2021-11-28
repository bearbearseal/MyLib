OBJS = main.o
OBJS += TcpListener.o TcpSocket.o
OBJS += UdpListener.o UdpSocket.o
OBJS += FileIOer.o FileBrowser.o
OBJS += StringManipulator.o UdpFileBrowser.o
OBJS += FileThreadWriter.o LogThreadWriter.o
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

TcpListener.o: ../TcpSocket/TcpListener.h ../TcpSocket/TcpListener.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpListener.cpp

TcpSocket.o: ../TcpSocket/TcpSocket.h ../TcpSocket/TcpSocket.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpSocket.cpp

UdpListener.o: ../UdpSocket/UdpListener.h ../UdpSocket/UdpListener.cpp
	$(CC) $(CFLAGS) ../UdpSocket/UdpListener.cpp

UdpSocket.o: ../UdpSocket/UdpSocket.h ../UdpSocket/UdpSocket.cpp
	$(CC) $(CFLAGS) ../UdpSocket/UdpSocket.cpp

FileIOer.o: FileIOer.h FileIOer.cpp
	$(CC) $(CFLAGS) FileIOer.cpp

FileBrowser.o: FileBrowser.h FileBrowser.cpp
	$(CC) $(CFLAGS) FileBrowser.cpp

StringManipulator.o: ../StringManipulator/StringManipulator.h ../StringManipulator/StringManipulator.cpp
	$(CC) $(CFLAGS) ../StringManipulator/StringManipulator.cpp

UdpFileBrowser.o: UdpFileBrowser.h UdpFileBrowser.cpp
	$(CC) $(CFLAGS) UdpFileBrowser.cpp

FileThreadWriter.o: FileThreadWriter.h FileThreadWriter.cpp
	$(CC) $(CFLAGS) FileThreadWriter.cpp

LogThreadWriter.o: LogThreadWriter.h LogThreadWriter.cpp
	$(CC) $(CFLAGS) LogThreadWriter.cpp

clean:
	rm -f $(binaries) *.o