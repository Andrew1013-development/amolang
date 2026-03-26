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
	$(CC) $(CCFLAGS) -D IN_DEVELOPMENT -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET) test.amo

# NOTE: use diff when developing for Linux
# for now fc.exe is usable since development is still on Windows
test: clean | all
	@echo "Running tests..."
	@echo "===== PARSER TESTS ====="
	@mkdir build\parser
	./$(TARGET) --debug-parser tests/parser/var_decl.amo > build/parser/var_decl.test
	./$(TARGET) --debug-parser tests/parser/func_decl.amo > build/parser/func_decl.test
	./$(TARGET) --debug-parser tests/parser/decl.amo > build/parser/decl.test
	#@fc.exe build\\parser\\var_decl.test tests\\parser\\var_decl.ans
	#@fc.exe build\\parser\\func_decl.test tests\\parser\\func_decl.ans
	#@fc.exe build\\parser\\decl.test tests\\parser\\decl.ans
	#@echo "===== ARGUMENT TESTS ====="
	#./$(TARGET) --help
	#./$(TARGET) --version
	#./$(TARGET) --debug-preprocessor test.amo
	#./$(TARGET) --debug-preprocessor --debug-lexer --debug-parser test.amo
	#-./$(TARGET) --debug-preprocessor --debug-lexer --debug-parser
	@echo "All tests passed."

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean
