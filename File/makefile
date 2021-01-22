SRC_DIR = .
OBJ_DIR = ./Built
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
CC = g++ -std=c++17
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lsqlite3 -lpthread
binaries = $(OBJ_DIR)/theMain.out

main: $(OBJ_FILES)
   $(CC) $(LFLAGS) $(OBJ_FILES) $(LIBS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
   $(CC) $(CFLAGS) $(OBJ_FILES).o -c -o $@ $<

clean:
	rm -f $(binaries) $(OBJ_DIR).o