# for debug build: make DEBUG=1
# otherwise: make

CXX = g++
DEBUG_FLAGS = -g -fno-omit-frame-pointer
COMMON_FLAGS = -std=c++17 -pthread -march=native -flto -Xclang -fopenmp -I/opt/homebrew/opt/libomp/include
VECTORIZE_FLAGS = -fvectorize -fslp-vectorize -fveclib=libmvec
CXXFLAGS = $(COMMON_FLAGS) $(VECTORIZE_FLAGS)
CXXFLAGS += $(if $(DEBUG),-Og $(DEBUG_FLAGS),-O3)
LDFLAGS = -Wl,-stack_size -Wl,0x1000000 -L/opt/homebrew/opt/libomp/lib -lomp

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