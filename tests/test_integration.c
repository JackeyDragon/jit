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

void test_integration_var_declaration() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 5, "x should equal 5");
}

void test_integration_addition() {
    reset_for_test();
    struct token *tokens = tokinize("int x = 5 + 3;");
    current = tokens;
    ast *tree = parse_statement();
    exec(tree);
    TEST_ASSERT_EQ(get_int("x"), 8, "x should equal 8");
}

int main() {
    printf("=== Integration Tests ===\n");
    TEST_RUN(test_integration_var_declaration);
    TEST_RUN(test_integration_addition);
    printf("\nIntegration tests complete!\n");
    return 0;
}