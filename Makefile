BUILD_DIR := build

CPP_FLAGS := -std=c++23

SRC_DIR := src
INC_DIR := include

HPP_FILES := $(wildcard $(INC_DIR)/*.hpp)
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
BIN_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%,$(SRC_FILES))

.PHONY: all clean test generate
all: $(BIN_FILES)

clean:
	rm -rf $(BUILD_DIR)

test: $(BIN_FILES)
	bash test.sh

generate: $(BIN_FILES)
	bash generate-test-output.sh

$(BUILD_DIR)/%: $(SRC_DIR)/%.cpp $(HPP_FILES) | $(BUILD_DIR)
	g++ $(CPP_FLAGS) -I$(INC_DIR) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

