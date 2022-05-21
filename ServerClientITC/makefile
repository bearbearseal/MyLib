OBJS = main.o
CXX = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lpthread
binaries = theMain.out

main : $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp ServerClientItc.h
	$(CXX) $(CFLAGS) $(INC) main.cpp

clean:
	rm -f $(binaries) *.o
