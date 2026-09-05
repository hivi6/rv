BIN_NAME  := rv
BUILD_DIR := build
BIN_PATH  := $(BUILD_DIR)/$(BIN_NAME)

CPP_FLAGS := -std=c++23

SRC_DIR := src
INC_DIR := include

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
HPP_FILES := $(wildcard $(INC_DIR)/*.hpp)

$(BIN_PATH): $(BUILD_DIR) $(CPP_FILES) $(HPP_FILES)
	g++ $(CPP_FLAGS) -I$(INC_DIR) $(CPP_FILES) -o $(BIN_PATH)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean test generate
clean:
	rm -rf $(BUILD_DIR)

test: $(BIN_PATH)
	bash test.sh

generate: $(BIN_PATH)
	bash generate-test-output.sh

