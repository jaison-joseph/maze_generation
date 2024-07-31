# for debug build: make DEBUG=1
# otherwise: make

CXX = g++
DEBUG_FLAGS = -g -fno-omit-frame-pointer
COMMON_FLAGS = -std=c++17 -pthread -I/opt/homebrew/Cellar/gperftools/2.15/include
CXXFLAGS = $(COMMON_FLAGS)
CXXFLAGS += $(if $(DEBUG),-Og $(DEBUG_FLAGS),-O3)
LDFLAGS = -Wl,-stack_size -Wl,0x1000000 -DWITHGPERFTOOLS -L/opt/homebrew/Cellar/gperftools/2.15/lib -lprofiler

# The name of your executable
EXECUTABLE = foobar

# Source file
SOURCE = genetic_3.cpp

# Object file
OBJECT = $(SOURCE:.cpp=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT)
	$(CXX) $(OBJECT) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT) $(EXECUTABLE)

.PHONY: all clean