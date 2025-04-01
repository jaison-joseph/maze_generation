# for debug build: make DEBUG=1
# for profiling: make PROFILE=1, then make run
# otherwise: make

OPENMP_INCLUDE =  -fopenmp -I/opt/homebrew/opt/libomp/include
OPENMP_LINK = -L/opt/homebrew/opt/libomp/lib -lomp

# profiler library path for macOS
GPERFTOOLS_PATH = /opt/homebrew/lib

ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

CXX = g++
DEBUG_FLAGS = -g -fno-omit-frame-pointer
# COMMON_FLAGS = -std=c++17 -pthread -march=native -flto
COMMON_FLAGS = -std=c++17 -pthread -march=native -flto # omit the flto for now
CXXFLAGS = $(COMMON_FLAGS) 
CXXFLAGS += $(if $(DEBUG),-Og $(DEBUG_FLAGS),-O3)
# CXXFLAGS += $(if $(DEBUG),-Og $(DEBUG_FLAGS))
CXXFLAGS += $(if $(OPENMP), $(OPENMP_INCLUDE),)

# Keep debugging symbols even in optimized mode
# CXXFLAGS += -g

# For profiling, we need frame pointers
ifeq ($(PROFILE),1)
    CXXFLAGS += -fno-omit-frame-pointer
endif

# profile output directory
PROFILE_DIR = profiling
PROFILE_OUTPUT = $(PROFILE_DIR)/prof.out

ifeq ($(DETECTED_OS),Windows)
    LDFLAGS := -Wl,--stack,16777216
else ifeq ($(DETECTED_OS),Darwin)
    LDFLAGS := -Wl,-stack_size -Wl,0x1000000
else ifeq ($(DETECTED_OS),Linux)
    LDFLAGS := -Wl,-z,stack-size=16777216
    CXXFLAGS += -fopt-info-vec-missed
else
    $(error Unsupported operating system: $(DETECTED_OS))
endif

# Add profiler flag when PROFILE=1
LDFLAGS += $(if $(PROFILE),-L$(GPERFTOOLS_PATH) -lprofiler,)

LDFLAGS += -pthread
LDFLAGS += $(if $(OPENMP), $(OPENMP_LINK),)

# The name of your executable
EXECUTABLE = foobar

# Source file
SOURCE = genetic_3.cpp

# Object file
OBJECT = $(SOURCE:.cpp=.o)

# Main targets
all: $(if $(PROFILE),profile,$(EXECUTABLE))

run: $(EXECUTABLE) $(PROFILE_DIR)
	CPUPROFILE=$(PROFILE_OUTPUT) ./$(EXECUTABLE)

# Profile target depends on executable and directory
profile: $(EXECUTABLE) $(PROFILE_DIR)

$(EXECUTABLE): $(OBJECT)
	$(CXX) $(OBJECT) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT) $(EXECUTABLE)

.PHONY: all clean profile