OBJS = main.o SerialPort.o SyncedSerialPort.o 
CC = g++
CFLAGS = -Wall -c
LFLAGS = -Wall
binaries = theMain

main : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

SerialPort.o : SerialPort.h SerialPort.cpp
	$(CC) $(CFLAGS) SerialPort.cpp

SyncedSerialPort.o : SerialPort.h SyncedSerialPort.h SyncedSerialPort.cpp
	$(CC) $(CFLAGS) SyncedSerialPort.cpp

clean:
	rm -f $(binaries) *.o
