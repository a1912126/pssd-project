# Compiler and linker settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall  # C++11 standard and warnings
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system  # SFML libraries

# Source and executable names
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = mygame

# Default target to build the executable
all: $(EXEC)

# Rule to create the executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Rule to create the object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJ) $(EXEC)

# Phony targets
.PHONY: all clean
