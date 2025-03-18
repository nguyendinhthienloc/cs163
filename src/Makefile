# Set the C++ compiler
CXX = g++

# Flags for compilation
CXXFLAGS = -std=c++11 -Wall

# Raylib include and library paths
RAYLIB_INCLUDE = ../raylib/include  # Path to the 'include' directory (one level up)
RAYLIB_LIB = ../raylib/lib          # Path to the 'lib' directory (one level up)

# Define the target executable and output directory
TARGET_DIR = ../build
TARGET = $(TARGET_DIR)/linked_list_visualizer.exe  # Output file path

# Source files
SRCS = main.cpp ui.cpp linked_list.cpp

# Include directories
INCLUDES = -I. -I$(RAYLIB_INCLUDE)

# Link libraries
LIBS = -L$(RAYLIB_LIB) -lraylib -lmingw32 -lgdi32 -lwinpthread -lwinmm

# Ensure the build directory exists
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Compilation rule
$(TARGET): $(SRCS) | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

# Clean rule
clean:
	rm -f $(TARGET)
