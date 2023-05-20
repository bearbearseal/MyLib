src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
CC = g++
CXXFLAGS = -Wall -c -std=c++17
LFLAGS = -Wall -std=c++17
LIBS = -lpthread
binary = theMain

main: $(obj)
	$(CC) $(LFLAGS) $^ $(LIBS) -o $(binary)

.PHONY: clean
clean:
	rm -f $(obj) $(binary)
