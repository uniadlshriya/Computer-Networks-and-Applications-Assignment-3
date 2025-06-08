# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Source file
SRC = DistanceVector.cpp

SRC1 = PoisonedReverse.cpp

# Output executable name
EXEC = DistanceVector

EXEC1 = PoisonedReverse

# Rule to compile the program
$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC)

$(EXEC1): $(SRC1)
	$(CXX) $(CXXFLAGS) -o $(EXEC1) $(SRC1)

# Clean rule to remove the executable
clean:
	rm -f $(EXEC) $(EXEC1)