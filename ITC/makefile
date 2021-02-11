OBJS = main.o
CXX = g++
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lpthread
binaries = theMain.out

main : $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp TestITC.hpp TestReadWrite.hpp ITC.h
	$(CXX) $(CFLAGS) $(INC) main.cpp

clean:
	rm -f $(binaries) *.o
