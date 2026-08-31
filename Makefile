BIN_NAME  := rv
BUILD_DIR := build

FINAL_BIN := $(BUILD_DIR)/$(BIN_NAME)
SRC_DIR   := src
INC_DIR   := include

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
HPP_FILES := $(wildcard $(INC_DIR)/*.hpp)

$(FINAL_BIN): $(BUILD_DIR) $(CPP_FILES) $(HPP_FILES)
	g++ -I$(INC_DIR) $(CPP_FILES) -o $(FINAL_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean test generate
clean:
	rm -rf $(BUILD_DIR)

test: $(FINAL_BIN)
	bash test.sh

generate: $(FINAL_BIN)
	bash generate-test-output.sh

