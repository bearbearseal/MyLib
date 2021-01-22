SRC_DIR = .
OBJ_DIR = ./obj
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS = -Wall
CPPFLAGS = -Wall -c
CPPC = g++ -std=c++17
BINARY = Built/theMain.out

$(BINARY): $(OBJ_FILES)
	$(CPPC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPPC) $(CPPFLAGS) -o $@ $<

clean:
	rm -f $(BINARY) *.o
