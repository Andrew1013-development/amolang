CC := gcc
CCFLAGS := -std=c17 -Wall -Wextra -Wpedantic -g

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

TARGET := $(BIN_DIR)/amocc.exe
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CCFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET) test.amo

# NOTE: use diff when developing for Linux
# for now fc.exe is usable since development is still on Windows
test: clean | all
	@echo "Running tests..."
	@mkdir build\parser
	#./$(TARGET) tests/parser/var_decl.amo > build/parser/var_decl.out
	./$(TARGET) tests/parser/func_decl.amo > build/parser/func_decl.out
	./$(TARGET) tests/parser/decl.amo > build/decl.out
	#@fc.exe build\\decl.out tests\\parser\\var_decl.ans
	#@fc.exe build\\decl.out tests\\parser\\func_decl.ans
	@fc.exe build\\decl.out tests\\parser\\decl.ans
	@echo "All tests passed."

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean
