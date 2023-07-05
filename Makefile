CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -O3 -Iinclude

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))

TARGET := bin/pseudointerp

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build/*.o $(TARGET)
