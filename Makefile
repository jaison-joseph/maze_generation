# for debug build: make DEBUG=1
# otherwise: make

OPENMP_INCLUDE =  -fopenmp -I/opt/homebrew/opt/libomp/include
OPENMP_LINK = -L/opt/homebrew/opt/libomp/lib -lomp

CXX = g++
DEBUG_FLAGS = -g -fno-omit-frame-pointer
COMMON_FLAGS = -std=c++17 -pthread -march=native -flto
CXXFLAGS = $(COMMON_FLAGS) 
CXXFLAGS += $(if $(DEBUG),-Og $(DEBUG_FLAGS),-O3)
CXXFLAGS += $(if $(OPENMP), $(OPENMP_INCLUDE),)
LDFLAGS = -Wl,-stack_size -Wl,0x1000000
LDFLAGS += $(if $(OPENMP), $(OPENMP_LINK),)

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