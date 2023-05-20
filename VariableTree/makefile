src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
obj += PrioritizedValue.o Value.o Variable.o
obj += TcpSocket.o TcpListener.o
CC = g++
CXXFLAGS = -Wall -c -std=c++17
LFLAGS = -Wall -std=c++17
LIBS = -lpthread
binary = theMain

main: $(obj)
	$(CC) $(LFLAGS) $^ $(LIBS) -o $(binary)

PrioritizedValue.o : ../Basic/PrioritizedValue.h ../Basic/PrioritizedValue.cpp
	$(CC) $(CXXFLAGS) ../Basic/PrioritizedValue.cpp -o $@

Value.o : ../Basic/Value.h ../Basic/Value.cpp
	$(CC) $(CXXFLAGS) ../Basic/Value.cpp -o $@

Variable.o : ../Basic/Variable.h ../Basic/Variable.cpp
	$(CC) $(CXXFLAGS) ../Basic/Variable.cpp -o $@

TcpSocket.o : ../TcpSocket/TcpSocket.h ../TcpSocket/TcpSocket.cpp
	$(CC) $(CXXFLAGS) ../TcpSocket/TcpSocket.cpp -o $@

TcpListener.o : ../TcpSocket/TcpListener.h ../TcpSocket/TcpListener.cpp
	$(CC) $(CXXFLAGS) ../TcpSocket/TcpListener.cpp -o $@

.PHONY: clean
clean:
	rm -f $(obj) $(binary)
