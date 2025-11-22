# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

# Directories
SRC_DIR = Source\ Code
BUILD_DIR = build
RELEASE_DIR = Release

# Targets
TARGET_A = exercise_a
TARGET_B = exercise_b
TARGET_C = exercise_c
TARGET_D = exercise_d
TARGET_E = exercise_e

# Source files
SOURCES_A = $(SRC_DIR)/exercise_a.cpp
SOURCES_B = $(SRC_DIR)/exercise_b.cpp
SOURCES_C = $(SRC_DIR)/exercise_c.cpp
SOURCES_D = $(SRC_DIR)/exercise_d.cpp
SOURCES_E = $(SRC_DIR)/exercise_e.cpp

# Build all executables
all: $(RELEASE_DIR)/$(TARGET_A) $(RELEASE_DIR)/$(TARGET_B) $(RELEASE_DIR)/$(TARGET_C) $(RELEASE_DIR)/$(TARGET_D) $(RELEASE_DIR)/$(TARGET_E)

# Build exercise_a
$(RELEASE_DIR)/$(TARGET_A): $(SOURCES_A)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_A) -o $(RELEASE_DIR)/$(TARGET_A) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_A)"

# Build exercise_b
$(RELEASE_DIR)/$(TARGET_B): $(SOURCES_B)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_B) -o $(RELEASE_DIR)/$(TARGET_B) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_B)"

# Build exercise_c (requires opencv_contrib)
$(RELEASE_DIR)/$(TARGET_C): $(SOURCES_C)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_C) -o $(RELEASE_DIR)/$(TARGET_C) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_C)"

# Build exercise_d (requires opencv_contrib)
$(RELEASE_DIR)/$(TARGET_D): $(SOURCES_D)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_D) -o $(RELEASE_DIR)/$(TARGET_D) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_D)"

# Build exercise_e (requires opencv_contrib)
$(RELEASE_DIR)/$(TARGET_E): $(SOURCES_E)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_E) -o $(RELEASE_DIR)/$(TARGET_E) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_E)"
	@echo "Dependencies: OpenCV 4.x" > $(RELEASE_DIR)/dll_requirements.txt
	@echo "To install OpenCV on macOS: brew install opencv" >> $(RELEASE_DIR)/dll_requirements.txt

# Clean build artifacts
clean:
	rm -rf $(RELEASE_DIR)/$(TARGET_A) $(RELEASE_DIR)/$(TARGET_B) $(RELEASE_DIR)/$(TARGET_C) $(RELEASE_DIR)/$(TARGET_D) $(RELEASE_DIR)/$(TARGET_E)
	@echo "Cleaned build artifacts"

# Run with test image
run:
	./$(RELEASE_DIR)/$(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all     - Build the executable"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Run the program (camera mode)"
	@echo "  help    - Show this help message"

.PHONY: all clean run help
