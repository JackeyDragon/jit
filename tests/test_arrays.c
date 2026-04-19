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
extern void init_table(void);
extern void print_ast(ast *node, int depth);

void print_tokens_test(struct token *tok) {
    printf("Tokens: ");
    while (tok) {
        printf("[%s:%s] ", token_type_to_string(tok->type), tok->value ? tok->value : "(null)");
        tok = tok->next;
    }
    printf("\n");
}

void reset_for_test() {
    reset_table();
    init_table();
}

value *get_var(char *name) {
    return lookup(name);
}

int get_int_val(char *name) {
    value *v = get_var(name);
    return v ? v->value.i : 0;
}

float get_float_val(char *name) {
    value *v = get_var(name);
    return v ? v->value.f : 0.0f;
}

/* ============================================================
   PARSER TESTS - Verify parsing works correctly
   ============================================================ */

void test_parse_array_declaration_int() {
    reset_for_test();
    struct token *tokens = tokinize("int arr[3] = {10, 20, 30};");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_DECLAR, "Should be NODE_DECLAR");
    TEST_ASSERT(tree->data.DECLARE.array == true, "Should be marked as array");
    TEST_ASSERT(tree->data.DECLARE.array_count == 3, "Should have 3 elements");
}

void test_parse_array_declaration_float() {
    reset_for_test();
    struct token *tokens = tokinize("float arr[2] = {1.5, 2.5};");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->data.DECLARE.array == true, "Should be marked as array");
    TEST_ASSERT(tree->data.DECLARE.array_count == 2, "Should have 2 elements");
}

void test_parse_array_single_element() {
    reset_for_test();
    struct token *tokens = tokinize("int arr[1] = {42};");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->data.DECLARE.array == true, "Should be marked as array");
    TEST_ASSERT(tree->data.DECLARE.array_count == 1, "Should have 1 element");
}

void test_parse_array_access() {
    reset_for_test();
    struct token *tokens = tokinize("int x = arr[0];");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_DECLAR, "Should be assignment");
    TEST_ASSERT(tree->data.DECLARE.expression != NULL, "Should have expression");
}

void test_parse_array_access_index_expr() {
    reset_for_test();
    struct token *tokens = tokinize("int x = arr[i + 1];");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
}

void test_parse_array_bad_no_close_bracket() {
    reset_for_test();
    struct token *tokens = tokinize("int arr[3 = {1, 2, 3};");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree == NULL, "Should fail - missing ]");
}

void test_parse_array_bad_no_close_brace() {
    reset_for_test();
    struct token *tokens = tokinize("int arr[3] = {1, 2, 3;");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree == NULL, "Should fail - missing }");
}

/* ============================================================
   INTEGRATION TESTS - Verify execution works correctly
   ============================================================ */

void test_int_array_declare() {
    reset_for_test();
    struct token *tokens = tokinize("int arr[3] = {10, 20, 30};");
    current = tokens;
    ast *tree = parse_statement();
    int result = exec(tree);
    TEST_ASSERT_EQ(result, 0, "Execution should succeed");
    
    value *v = get_var("arr");
    TEST_ASSERT(v != NULL, "arr should be in symbol table");
    TEST_ASSERT(v->array == true, "arr should be an array");
    TEST_ASSERT(v->size == 3, "arr should have size 3");
    TEST_ASSERT(v->type == INT, "arr should be INT type");
}

void test_float_array_declare() {
    reset_for_test();
    struct token *tokens = tokinize("float arr[2] = {1.5, 2.5};");
    current = tokens;
    ast *tree = parse_statement();
    int result = exec(tree);
    TEST_ASSERT_EQ(result, 0, "Execution should succeed");
    
    value *v = get_var("arr");
    TEST_ASSERT(v != NULL, "arr should be in symbol table");
    TEST_ASSERT(v->array == true, "arr should be an array");
    TEST_ASSERT(v->size == 2, "arr should have size 2");
    TEST_ASSERT(v->type == FLOAT, "arr should be FLOAT type");
}

void test_array_access_index_0() {
    reset_for_test();
    
    struct token *tokens = tokinize("int arr[3] = {10, 20, 30};");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int x = arr[0];");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("x"), 10, "arr[0] should be 10");
}

void test_array_access_index_1() {
    reset_for_test();
    
    struct token *tokens = tokinize("int arr[3] = {10, 20, 30};");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int x = arr[1];");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("x"), 20, "arr[1] should be 20");
}

void test_array_access_index_2() {
    reset_for_test();
    
    struct token *tokens = tokinize("int arr[3] = {10, 20, 30};");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int x = arr[2];");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("x"), 30, "arr[2] should be 30");
}

void test_array_single_element_access() {
    reset_for_test();
    
    struct token *tokens = tokinize("int arr[1] = {42};");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int x = arr[0];");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("x"), 42, "arr[0] should be 42");
}

void test_float_array_access() {
    reset_for_test();
    
    struct token *tokens = tokinize("float arr[2] = {1.5, 2.5};");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("float x = arr[1];");
    current = tokens;
    exec(parse_statement());
    
    value *v = get_var("x");
    TEST_ASSERT(v != NULL, "x should exist");
    TEST_ASSERT(v->type == FLOAT, "x should be FLOAT type");
}

void test_array_in_assignment() {
    reset_for_test();
    
    struct token *tokens = tokinize("int arr[2] = {5, 10};");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int sum = arr[0] + arr[1];");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("sum"), 15, "arr[0] + arr[1] should be 15");
}

int main() {
    printf("=== Array Tests ===\n\n");
    
    printf("--- Parser Tests ---\n");
    TEST_RUN(test_parse_array_declaration_int);
    TEST_RUN(test_parse_array_declaration_float);
    TEST_RUN(test_parse_array_single_element);
    TEST_RUN(test_parse_array_access);
    TEST_RUN(test_parse_array_access_index_expr);
    TEST_RUN(test_parse_array_bad_no_close_bracket);
    TEST_RUN(test_parse_array_bad_no_close_brace);
    
    printf("\n--- Integration Tests ---\n");
    TEST_RUN(test_int_array_declare);
    TEST_RUN(test_float_array_declare);
    TEST_RUN(test_array_access_index_0);
    TEST_RUN(test_array_access_index_1);
    TEST_RUN(test_array_access_index_2);
    TEST_RUN(test_array_single_element_access);
    TEST_RUN(test_float_array_access);
    TEST_RUN(test_array_in_assignment);
    
    printf("\nAll array tests passed!\n");
    return 0;
}