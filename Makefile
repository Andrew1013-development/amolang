CC := gcc
CXX := g++
CCFLAGS := -std=c23 -Wall -Wextra -Wpedantic -g
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -g

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
TEST_DIR := tests

TARGET := $(BIN_DIR)/amocc.exe
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
TEST_FILES := $(wildcard $(TEST_DIR)/*/test-*.amo)
TEST_ANSWERS := $(wildcard $(TEST_DIR)/*/test-*.ans)
TEST_RESULTS := $(patsubst $(TEST_DIR)/%.amo, $(OBJ_DIR)/%.test, $(TEST_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CCFLAGS) $^ -o $@
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CCFLAGS) -D IN_DEVELOPMENT -c $< -o $@
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET) test2.amo

# NOTE: use diff when developing for Linux
# for now fc.exe is usable since development is still on Windows
test: clean all $(TEST_RESULTS) test-cli
	@echo "===== ALL TESTS COMPLETED ====="
$(OBJ_DIR)/%.test: $(TEST_DIR)/%.amo
	@echo "Running test: $*"
	@mkdir -p $(dir $@)
	./$(TARGET) --debug-$(firstword $(subst /, ,$*)) $< > $@
	@powershell diff $(subst /,\,$@) $(subst /,\,$(TEST_DIR)/$*.ans)
	@echo
test-cli:
	@echo "===== ARGUMENT TESTS ====="
	./$(TARGET) --help
	./$(TARGET) --version
	./$(TARGET) --debug-preprocessor test.amo
	./$(TARGET) --debug-preprocessor --debug-lexer --debug-parser test.amo
	-./$(TARGET) --debug-preprocessor --debug-lexer --debug-parser

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean
