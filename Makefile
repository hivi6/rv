BIN_NAME  := rv
BUILD_DIR := build
BIN_PATH  := $(BUILD_DIR)/$(BIN_NAME)

TEST32_NAME := rv-test32
TEST32_PATH := $(BUILD_DIR)/$(TEST32_NAME)

TEST64_NAME := rv-test64
TEST64_PATH := $(BUILD_DIR)/$(TEST64_NAME)

CPP_FLAGS := -std=c++23

SRC_DIR := src
INC_DIR := include

MAIN_FILE   := $(SRC_DIR)/main.cpp
TEST32_FILE := $(SRC_DIR)/test32.cpp
TEST64_FILE := $(SRC_DIR)/test64.cpp
HPP_FILES   := $(wildcard $(INC_DIR)/*.hpp)

.PHONY: build clean test generate create-build-dir
build: $(BIN_PATH) $(TEST32_PATH) $(TEST64_PATH)

clean:
	rm -rf $(BUILD_DIR)

test: $(BIN_PATH) $(TEST32_PATH) $(TEST64_PATH)
	bash test.sh

generate: $(BIN_PATH) $(TEST32_PATH) $(TEST64_PATH)
	bash generate-test-output.sh

create-build-dir:
	mkdir -p $(BUILD_DIR)

$(BIN_PATH): $(MAIN_FILE) $(HPP_FILES) | create-build-dir
	g++ $(CPP_FLAGS) -I$(INC_DIR) $(MAIN_FILE) -o $(BIN_PATH)

$(TEST32_PATH): $(TEST32_FILE) $(HPP_FILES) | create-build-dir
	g++ $(CPP_FLAGS) -I$(INC_DIR) $(TEST32_FILE) -o $(TEST32_PATH)

$(TEST64_PATH): $(TEST64_FILE) $(HPP_FILES) | create-build-dir
	g++ $(CPP_FLAGS) -I$(INC_DIR) $(TEST64_FILE) -o $(TEST64_PATH)

