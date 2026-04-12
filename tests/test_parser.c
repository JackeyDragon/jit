#include "test.h"
#include "../src/tokenizer.h"
#include "../src/ast.h"

extern struct token *current;
extern ast *parse_statement();

void test_parse_number() {
    struct token *tokens = tokinize("42");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "AST should not be NULL");
}

void test_parse_var_decl() {
    struct token *tokens = tokinize("var x = 5;");
    current = tokens;
    ast *tree = parse_statement();
    TEST_ASSERT(tree != NULL, "AST should not be NULL");
}

int main() {
    printf("=== Parser Tests ===\n");
    TEST_RUN(test_parse_number);
    TEST_RUN(test_parse_var_decl);
    printf("\nAll parser tests passed!\n");
    return 0;
}