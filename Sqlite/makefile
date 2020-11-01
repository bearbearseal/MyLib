OBJS = main.o Sqlite3.o
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lsqlite3
binaries = theMain.out

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

Sqlite3.o : Sqlite3.h Sqlite3.cpp
	$(CC) $(CFLAGS) Sqlite3.cpp

clean:
	rm -f $(binaries) *.o
