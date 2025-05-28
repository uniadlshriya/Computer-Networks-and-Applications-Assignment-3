# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Source file
SRC = DistanceVector.cpp

# Output executable name
EXEC = DistanceVector

# Rule to compile the program
$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC)

# Clean rule to remove the executable
clean:
	rm -f $(EXEC)