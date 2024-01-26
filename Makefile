# Makefile

# Compiler to use
CC = clang

# Source files
SRC = app.c

# Include directories
INCLUDES = -I/opt/homebrew/include/SDL2

# Compiler flags
CFLAGS = -D_THREAD_SAFE

# Output executable name
TARGET = app

# Directory to store build files
BUILD_DIR = bin

# Target with build directory
BUILD_TARGET = $(BUILD_DIR)/$(TARGET)

# Default rule
all: $(BUILD_TARGET)

# Rule for creating build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule for building the target
$(BUILD_TARGET): $(SRC) | $(BUILD_DIR)
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) -o $(BUILD_TARGET)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
