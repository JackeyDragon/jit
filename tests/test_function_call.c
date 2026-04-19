#include "test.h"
#include "../src/tokenizer.h"
#include "../src/ast.h"
#include "../src/symbol_table.h"
#include "../src/types.h"

extern struct token *current;
extern struct token *tokinize(char *string);
extern ast *parse_statement();
extern int exec(ast *statement);
extern value *lookup(char *name);
extern function *lookup_function(char *name);
extern void reset_table(void);
extern void init_table(void);

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

/* ============================================================
   PARSER TESTS - Verify parsing works correctly
   ============================================================ */

void test_parse_function_no_params() {
    reset_for_test();
    struct token *tokens = tokinize("int foo() { return 0; }");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_FUNCTION_DECLARATION, "Should be FUNCTION_DECLARATION");
}

void test_parse_function_single_param() {
    reset_for_test();
    struct token *tokens = tokinize("int foo(int x) { return x; }");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_FUNCTION_DECLARATION, "Should be FUNCTION_DECLARATION");
}

void test_parse_function_multiple_params() {
    reset_for_test();
    struct token *tokens = tokinize("int add(int a, int b) { return a + b; }");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_FUNCTION_DECLARATION, "Should be FUNCTION_DECLARATION");
}

void test_parse_function_with_body() {
    reset_for_test();
    struct token *tokens = tokinize("int foo() { int x = 5; return x; }");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_FUNCTION_DECLARATION, "Should be FUNCTION_DECLARATION");
}

void test_parse_function_return() {
    reset_for_test();
    struct token *tokens = tokinize("int foo() { return 5; }");
    TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "Parse should succeed");
    TEST_ASSERT(tree->type == NODE_FUNCTION_DECLARATION, "Should be FUNCTION_DECLARATION");
}

void test_parse_function_bad_missing_paren() {
    reset_for_test();
    struct token *tokens = tokinize("int foo { }");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree == NULL, "Should fail - missing (");
}

void test_parse_function_bad_missing_return_type() {
    reset_for_test();
    struct token *tokens = tokinize("foo() { return 0; }");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree == NULL || tree->type != NODE_FUNCTION_DECLARATION, "Should fail or not be function declaration");
}

/* ============================================================
   INTEGRATION TESTS - Verify execution works correctly
   ============================================================ */

void test_function_declare_simple() {
    reset_for_test();
    struct token *tokens = tokinize("int five() { return 5; }");
    current = tokens;
    ast *tree = parse_statement();
    int result = exec(tree);
    TEST_ASSERT_EQ(result, 0, "Execution should succeed");
    
    function *fn = lookup_function("five");
    TEST_ASSERT(fn != NULL, "Function 'five' should be registered");
}

void test_function_declare_with_param() {
    reset_for_test();
    struct token *tokens = tokinize("int double(int x) { return x + x; }");
    current = tokens;
    ast *tree = parse_statement();
    int result = exec(tree);
    TEST_ASSERT_EQ(result, 0, "Execution should succeed");
    
    function *fn = lookup_function("double");
    TEST_ASSERT(fn != NULL, "Function 'double' should be registered");
    TEST_ASSERT(fn->parameter_count == 1, "Should have 1 parameter");
}

void test_function_call_no_args() {
    reset_for_test();
    
    struct token *tokens = tokinize("int getFive() { return 5; }");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int result = getFive();");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("result"), 5, "getFive() should return 5");
}

void test_function_call_with_args() {
    reset_for_test();
    
    struct token *tokens = tokinize("int add(int a, int b) { return a + b; }");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int sum = add(2, 3);");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("sum"), 5, "add(2, 3) should return 5");
}

void test_function_call_expr_args() {
    reset_for_test();
    
    struct token *tokens = tokinize("int add(int a, int b) { return a + b; }");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int sum = add(1 + 2, 3 * 4);");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("sum"), 15, "add(1+2, 3*4) should return 15");
}

void test_function_nested_call() {
    reset_for_test();
    
    struct token *tokens = tokinize("int five() { return 5; }");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int add(int a, int b) { return a + b; }");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int result = add(five(), five());");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("result"), 10, "add(five(), five()) should return 10");
}

void test_function_call_in_expression() {
    reset_for_test();
    
    struct token *tokens = tokinize("int triple(int x) { return x * 3; }");
    current = tokens;
    exec(parse_statement());
    
    tokens = tokinize("int result = triple(2) + 1;");
    current = tokens;
    exec(parse_statement());
    
    TEST_ASSERT_EQ(get_int_val("result"), 7, "triple(2) + 1 should return 7");
}

int main() {
    printf("=== Function Tests ===\n\n");
    
    printf("--- Parser Tests ---\n");
    TEST_RUN(test_parse_function_no_params);
    TEST_RUN(test_parse_function_single_param);
    TEST_RUN(test_parse_function_multiple_params);
    TEST_RUN(test_parse_function_with_body);
    TEST_RUN(test_parse_function_return);
    TEST_RUN(test_parse_function_bad_missing_paren);
    TEST_RUN(test_parse_function_bad_missing_return_type);
    
    printf("\n--- Integration Tests ---\n");
    TEST_RUN(test_function_declare_simple);
    TEST_RUN(test_function_declare_with_param);
    TEST_RUN(test_function_call_no_args);
    TEST_RUN(test_function_call_with_args);
    TEST_RUN(test_function_call_expr_args);
    TEST_RUN(test_function_nested_call);
    TEST_RUN(test_function_call_in_expression);
    
    printf("\nAll function tests passed!\n");
    return 0;
}