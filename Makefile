# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -I/opt/homebrew/include
LDFLAGS := -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system

# Target binary
TARGET := myapp

# Source files 
SRCS := $(wildcard *.cpp)

# Object files
OBJS := $(SRCS:.cpp=.o)

# Build rules
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

