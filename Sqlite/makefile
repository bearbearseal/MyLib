OFOLDER = ObjectFolder/
OBJS = $(OFOLDER)main.o $(OFOLDER)Sqlite3.o
OBJS += $(OFOLDER)Sqlite3JsonTalker.o
OBJS += $(OFOLDER)Sqlite3TcpListener.o $(OFOLDER)TcpListener.o $(OFOLDER)TcpSocket.o
OBJS += $(OFOLDER)Sqlite3UdpListener.o $(OFOLDER)UdpListener.o $(OFOLDER)UdpSocket.o
OBJS += $(OFOLDER)Variable.o $(OFOLDER)Value.o $(OFOLDER)VariableSqlite3.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lsqlite3 -lpthread
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

$(OFOLDER)main.o : main.cpp Test.hpp
	$(CC) $(CFLAGS) main.cpp -o $@

$(OFOLDER)Sqlite3.o : Sqlite3.h Sqlite3.cpp
	$(CC) $(CFLAGS) Sqlite3.cpp -o $@

$(OFOLDER)Sqlite3JsonTalker.o: Sqlite3JsonTalker.h Sqlite3JsonTalker.cpp
	$(CC) $(CFLAGS) Sqlite3JsonTalker.cpp -o $@

$(OFOLDER)Sqlite3TcpListener.o: Sqlite3TcpListener.h Sqlite3TcpListener.cpp
	$(CC) $(CFLAGS) Sqlite3TcpListener.cpp -o $@

$(OFOLDER)TcpListener.o: ../TcpSocket/TcpListener.h ../TcpSocket/TcpListener.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpListener.cpp -o $@

$(OFOLDER)TcpSocket.o: ../TcpSocket/TcpSocket.h ../TcpSocket/TcpSocket.cpp
	$(CC) $(CFLAGS) ../TcpSocket/TcpSocket.cpp -o $@

$(OFOLDER)Sqlite3UdpListener.o: Sqlite3UdpListener.h Sqlite3UdpListener.cpp
	$(CC) $(CFLAGS) Sqlite3UdpListener.cpp -o $@

$(OFOLDER)UdpListener.o: ../UdpSocket/UdpListener.h ../UdpSocket/UdpListener.cpp
	$(CC) $(CFLAGS) ../UdpSocket/UdpListener.cpp -o $@

$(OFOLDER)UdpSocket.o: ../UdpSocket/UdpSocket.h ../UdpSocket/UdpSocket.cpp
	$(CC) $(CFLAGS) ../UdpSocket/UdpSocket.cpp -o $@

$(OFOLDER)VariableSqlite3.o: VariableSqlite3.h VariableSqlite3.cpp
	$(CC) $(CFLAGS) VariableSqlite3.cpp -o $@

$(OFOLDER)Variable.o: ../Basic/Variable.h ../Basic/Variable.cpp
	$(CC) $(CFLAGS) ../Basic/Variable.cpp -o $@

$(OFOLDER)Value.o: ../Basic/Value.h ../Basic/Value.cpp
	$(CC) $(CFLAGS) ../Basic/Value.cpp -o $@

clean:
	rm -f $(binaries) $(OFOLDER)*.o
