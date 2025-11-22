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
TARGET_F = exercise_f
TARGET_G = exercise_g
TARGET_H = exercise_h
TARGET_I = exercise_i
TARGET_MAIN = cvlab
TARGET_AUTO = cvlab_auto

# Source files
SOURCES_A = $(SRC_DIR)/exercise_a.cpp
SOURCES_B = $(SRC_DIR)/exercise_b.cpp
SOURCES_C = $(SRC_DIR)/exercise_c.cpp
SOURCES_D = $(SRC_DIR)/exercise_d.cpp
SOURCES_E = $(SRC_DIR)/exercise_e.cpp
SOURCES_F = $(SRC_DIR)/exercise_f.cpp
SOURCES_G = $(SRC_DIR)/exercise_g.cpp
SOURCES_H = $(SRC_DIR)/exercise_h.cpp
SOURCES_I = $(SRC_DIR)/exercise_i.cpp
SOURCES_MAIN = $(SRC_DIR)/main.cpp
SOURCES_AUTO = $(SRC_DIR)/cvlab_auto.cpp

# Build all executables
all: $(RELEASE_DIR)/$(TARGET_MAIN) $(RELEASE_DIR)/$(TARGET_AUTO) $(RELEASE_DIR)/$(TARGET_A) $(RELEASE_DIR)/$(TARGET_B) $(RELEASE_DIR)/$(TARGET_C) $(RELEASE_DIR)/$(TARGET_D) $(RELEASE_DIR)/$(TARGET_E) $(RELEASE_DIR)/$(TARGET_F) $(RELEASE_DIR)/$(TARGET_G) $(RELEASE_DIR)/$(TARGET_H) $(RELEASE_DIR)/$(TARGET_I)

# Build main unified program (requires opencv_contrib)
$(RELEASE_DIR)/$(TARGET_MAIN): $(SOURCES_MAIN)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_MAIN) -o $(RELEASE_DIR)/$(TARGET_MAIN) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_MAIN)"

# Build automated program
$(RELEASE_DIR)/$(TARGET_AUTO): $(SOURCES_AUTO)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_AUTO) -o $(RELEASE_DIR)/$(TARGET_AUTO) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_AUTO)"

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

# Build exercise_f (requires opencv_contrib)
$(RELEASE_DIR)/$(TARGET_F): $(SOURCES_F)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_F) -o $(RELEASE_DIR)/$(TARGET_F) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_F)"

# Build exercise_g
$(RELEASE_DIR)/$(TARGET_G): $(SOURCES_G)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_G) -o $(RELEASE_DIR)/$(TARGET_G) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_G)"

# Build exercise_h (requires opencv_contrib)
$(RELEASE_DIR)/$(TARGET_H): $(SOURCES_H)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_H) -o $(RELEASE_DIR)/$(TARGET_H) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_H)"

# Build exercise_i
$(RELEASE_DIR)/$(TARGET_I): $(SOURCES_I)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES_I) -o $(RELEASE_DIR)/$(TARGET_I) $(OPENCV_FLAGS)
	@echo "Build complete: $(RELEASE_DIR)/$(TARGET_I)"
	@echo "Dependencies: OpenCV 4.x" > $(RELEASE_DIR)/dll_requirements.txt
	@echo "To install OpenCV on macOS: brew install opencv" >> $(RELEASE_DIR)/dll_requirements.txt

# Clean build artifacts
clean:
	rm -rf $(RELEASE_DIR)/$(TARGET_MAIN) $(RELEASE_DIR)/$(TARGET_AUTO) $(RELEASE_DIR)/$(TARGET_A) $(RELEASE_DIR)/$(TARGET_B) $(RELEASE_DIR)/$(TARGET_C) $(RELEASE_DIR)/$(TARGET_D) $(RELEASE_DIR)/$(TARGET_E) $(RELEASE_DIR)/$(TARGET_F) $(RELEASE_DIR)/$(TARGET_G) $(RELEASE_DIR)/$(TARGET_H) $(RELEASE_DIR)/$(TARGET_I)
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
