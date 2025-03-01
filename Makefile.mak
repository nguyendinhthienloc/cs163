# Compiler
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./include

# Raylib Libraries (Ensure you have raylib installed in w64devkit)
LDFLAGS = -L. -lraylib -lglfw3 -lopengl32 -lgdi32 -lwinmm

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Executable Name
TARGET = $(BUILD_DIR)/DataVisualizer.exe

# Default Rule: Compile everything
all: $(TARGET)

# Linking: Generate the final executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $(TARGET) $(LDFLAGS)

# Compile each .cpp file into .o (object file)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: all
	./$(TARGET)

# Clean the compiled files
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)
