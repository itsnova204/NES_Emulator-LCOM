# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = main.c ./nes_emu/mapper.c ./nes_emu/cartridge.c ./nes_emu/bus.c ./nes_emu/p6502.c 

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = main

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c -g $< -o $@

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
