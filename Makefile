# C64 Card Battler - Makefile
# Requires cc65 toolchain and VICE emulator

CC := C:/bin/cc65/bin/cl65.exe
TARGET := c64
PROGRAM := cardbattle.prg
BUILD_DIR := build
SRC_DIR := src

# Compiler flags
CFLAGS := -t $(TARGET) -O -Or -Cl -I$(SRC_DIR)
LDFLAGS := -t $(TARGET) -m $(BUILD_DIR)/cardbattle.map

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Default target
all: $(BUILD_DIR) $(BUILD_DIR)/$(PROGRAM)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link program
$(BUILD_DIR)/$(PROGRAM): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo Build complete: $(BUILD_DIR)/$(PROGRAM)

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Run in VICE emulator
run: $(BUILD_DIR)/$(PROGRAM)
	x64sc $(BUILD_DIR)/$(PROGRAM)

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)
	@echo Clean complete

.PHONY: all run clean
