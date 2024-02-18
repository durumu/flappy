# Makefile

# Compiler to use
CXX = clang++

# Source files
SRC = app.cc

# Include directories
INCLUDES = -I/opt/homebrew/include/SDL2

# Compiler flags
CXXFLAGS = -O2 -Wall -std=c++20 -D_THREAD_SAFE

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
	$(CXX) $(SRC) $(INCLUDES) $(CXXFLAGS) -o $(BUILD_TARGET)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
