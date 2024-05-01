g++ -std=c++17 -Wall -c Timer.cpp -lpthread
g++ -std=c++17 -Wall -c Test.cpp -lpthread
g++ -Wall Test.o Timer.o -lpthread -o Timer
./Timer
