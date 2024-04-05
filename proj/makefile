# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = main.c ./src/mapper.c ./src/cartridge.c ./src/bus.c ./src/p6502.c ./src/ppu.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = main

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Debug target
test: CFLAGS += -DTEST
test: clean all

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
TARGET = main
