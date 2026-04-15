CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS =

SRCS = src/main.c src/lexer.c src/parser.c src/ast.c src/code.c src/emiter.c src/symbol_table.c src/types.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/jit

TEST_SRCS = tests/test_lexer.c tests/test_parser.c tests/test_function.c tests/test_integration.c
TEST_TARGETS = tests/test_lexer tests/test_parser tests/test_function tests/test_integration

.PHONY: all clean test unit integration benchmark

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_TARGETS) tests/test_benchmark benchmark_results.jsonl

test: unit integration

unit: $(TEST_TARGETS)
	@echo "=== Running all unit tests ==="
	@for test in $(TEST_TARGETS); do ./$$test; done

tests/test_lexer: tests/test_lexer.o src/lexer.o src/tokenizer.h
	$(CC) $(CFLAGS) -o $@ $^

tests/test_parser: tests/test_parser.o src/parser.o src/ast.o src/lexer.o src/emiter.o src/symbol_table.o
	$(CC) $(CFLAGS) -o $@ $^

tests/test_function: tests/test_function.o src/parser.o src/ast.o src/lexer.o src/emiter.o src/symbol_table.o src/code.o src/types.o
	$(CC) $(CFLAGS) -o $@ $^

tests/test_integration: tests/test_integration.o src/parser.o src/ast.o src/lexer.o src/emiter.o src/symbol_table.o
	$(CC) $(CFLAGS) -o $@ $^

tests/test_lexer.o: tests/test_lexer.c tests/test.h src/tokenizer.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_parser.o: tests/test_parser.c tests/test.h src/tokenizer.h src/ast.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_function.o: tests/test_function.c tests/test.h src/tokenizer.h src/ast.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_integration.o: tests/test_integration.c tests/test.h src/tokenizer.h src/ast.h src/symbol_table.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_benchmark: tests/test_benchmark.o src/parser.o src/ast.o src/lexer.o src/emiter.o src/symbol_table.o
	$(CC) $(CFLAGS) -o $@ $^

tests/test_benchmark.o: tests/test_benchmark.c tests/test.h src/tokenizer.h src/ast.h
	$(CC) $(CFLAGS) -c $< -o $@

integration: $(TARGET)
	@echo "=== Running Integration Tests ==="
	@echo "var x = 5;" | ./$(TARGET)
	@echo ""
	@echo "Integration test complete"

benchmark: tests/test_benchmark
	@echo "=== Running Benchmarks ==="
	./tests/test_benchmark