# Compiler and output executable name (defaults to gcc if not specified)
CC ?= gcc
TARGET = popcount_bench

# Paths to your header-only libraries (adjust if they are in subdirectories)
SIMDE_INCLUDE_DIR = .
LIBPOPCNT_INCLUDE_DIR = .

# Base compilation flags for all files
CFLAGS = -O3 -march=native -Wall -I$(SIMDE_INCLUDE_DIR) -I$(LIBPOPCNT_INCLUDE_DIR)

# Specialized flags specifically for the math kernels to prevent layout shifts
KERNEL_CFLAGS = $(CFLAGS) -falign-functions=32 

# Object files needed for the final executable
OBJS = popcount_bench.o popcount_kernels.o

# Default target
.PHONY: all clean
all: $(TARGET)

# Linking phase
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -lm

# Compile the main driver (standard optimizations)
popcount_bench.o: popcount_bench.c popcount_kernels.h
	$(CC) $(CFLAGS) -c popcount_bench.c -o popcount_bench.o

# Compile the kernels (strict cache-alignment optimizations)
popcount_kernels.o: popcount_kernels.c popcount_kernels.h
	$(CC) $(KERNEL_CFLAGS) -c popcount_kernels.c -o popcount_kernels.o

# Cleanup utility
clean:
	rm -f $(OBJS) $(TARGET) timings.csv