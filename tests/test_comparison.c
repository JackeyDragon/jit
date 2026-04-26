#include "test.h"
#include "../src/tokenizer.h"
#include "../src/ast.h"
#include "../src/symbol_table.h"
#include "../src/types.h"

extern struct token *current;
extern ast *parse_statement();
extern int exec(ast *statement);
extern value *lookup(char *name);
extern void reset_table(void);

int get_int(char *name) {
    value *v = lookup(name);
    return v ? v->value.i : 0;
}

void reset_for_test() {
    reset_table();
    init_table();
}

void test_greater_than_int() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5 > 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "5 > 3 should be 1");
}

void test_greater_than_false() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 > 5;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 0, "3 > 5 should be 0");
}

void test_greater_than_equal() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 > 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 0, "3 > 3 should be 0");
}

void test_less_than_int() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 < 5;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3 < 5 should be 1");
}

void test_less_than_false() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5 < 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 0, "5 < 3 should be 0");
}

void test_less_than_equal() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 < 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 0, "3 < 3 should be 0");
}

void test_greater_equal_true() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5 >= 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "5 >= 3 should be 1");
}

void test_greater_equal_equal() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 >= 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3 >= 3 should be 1");
}

void test_greater_equal_false() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 >= 5;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 0, "3 >= 5 should be 0");
}

void test_less_equal_true() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 <= 5;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3 <= 5 should be 1");
}

void test_less_equal_equal() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 <= 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3 <= 3 should be 1");
}

void test_less_equal_false() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5 <= 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 0, "5 <= 3 should be 0");
}

void test_greater_than_float() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5.0 > 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "5.0 > 3 should be 1");
}

void test_less_than_float() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 < 5.0;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3 < 5.0 should be 1");
}

void test_greater_equal_float() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3.0 >= 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3.0 >= 3 should be 1");
}

void test_less_equal_float() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 3 <= 3.0;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 1, "3 <= 3.0 should be 1");
}

int main() {
    printf("=== Comparison Operator Tests ===\n");
    
    printf("\n--- GREATER_THAN ---\n");
    TEST_RUN(test_greater_than_int);
    TEST_RUN(test_greater_than_false);
    TEST_RUN(test_greater_than_equal);
    
    printf("\n--- LESS_THAN ---\n");
    TEST_RUN(test_less_than_int);
    TEST_RUN(test_less_than_false);
    TEST_RUN(test_less_than_equal);
    
    printf("\n--- GREATER_EQUAL ---\n");
    TEST_RUN(test_greater_equal_true);
    TEST_RUN(test_greater_equal_equal);
    TEST_RUN(test_greater_equal_false);
    
    printf("\n--- LESS_EQUAL ---\n");
    TEST_RUN(test_less_equal_true);
    TEST_RUN(test_less_equal_equal);
    TEST_RUN(test_less_equal_false);
    
    printf("\n--- FLOAT OPERANDS ---\n");
    TEST_RUN(test_greater_than_float);
    TEST_RUN(test_less_than_float);
    TEST_RUN(test_greater_equal_float);
    TEST_RUN(test_less_equal_float);
    
    printf("\nAll comparison operator tests passed!\n");
    return 0;
}