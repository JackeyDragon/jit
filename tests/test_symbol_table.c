#include "test.h"
#include "../src/symbol_table.h"
#include "../src/types.h"

void reset_for_test() {
    reset_table();
    init_table();
}

void test_insert_lookup_int() {
    reset_for_test();
    value v = {.type = INT, .size = sizeof(int), .value.i = 42};
    insert("x", &v);
    
    value *found = lookup("x");
    TEST_ASSERT(found != NULL, "x should be found");
    TEST_ASSERT_EQ(found->value.i, 42, "x should equal 42");
    TEST_ASSERT_EQ(found->type, INT, "x type should be INT");
}

void test_insert_lookup_float() {
    reset_for_test();
    value v = {.type = FLOAT, .size = sizeof(float), .value.f = 3.14f};
    insert("pi", &v);
    
    value *found = lookup("pi");
    TEST_ASSERT(found != NULL, "pi should be found");
    float diff = found->value.f - 3.14f;
    if (diff < 0) diff = -diff;
    TEST_ASSERT(diff < 0.001f, "pi should be approximately 3.14");
    TEST_ASSERT_EQ(found->type, FLOAT, "pi type should be FLOAT");
}

void test_lookup_not_found() {
    reset_for_test();
    value *found = lookup("nonexistent");
    TEST_ASSERT(found == NULL, "lookup should return NULL for non-existent");
}

void test_reset_clears_values() {
    reset_for_test();
    value v = {.type = INT, .size = sizeof(int), .value.i = 99};
    insert("y", &v);
    
    reset_table();
    value *found = lookup("y");
    TEST_ASSERT(found == NULL, "y should not be found after reset");
}

void test_enter_scope() {
    reset_for_test();
    int result = enter();
    TEST_ASSERT_EQ(result, 0, "enter should return 0");
    leave();
}

void test_insert_in_scope() {
    reset_for_test();
    enter();
    
    value v = {.type = INT, .size = sizeof(int), .value.i = 100};
    insert("z", &v);
    
    value *found = lookup("z");
    TEST_ASSERT(found != NULL, "z should be found in scope");
    TEST_ASSERT_EQ(found->value.i, 100, "z should equal 100");
    
    leave();
}

void test_leave_removes_scoped_var() {
    reset_for_test();
    enter();
    
    value v = {.type = INT, .size = sizeof(int), .value.i = 200};
    insert("a", &v);
    
    leave();
    
    value *found = lookup("a");
    TEST_ASSERT(found == NULL, "a should not be found after leave");
}

void test_inner_shadows_outer() {
    reset_for_test();
    value outer_val = {.type = INT, .size = sizeof(int), .value.i = 1};
    insert("shared", &outer_val);
    
    enter();
    value inner_val = {.type = INT, .size = sizeof(int), .value.i = 2};
    insert("shared", &inner_val);
    
    value *found = lookup("shared");
    TEST_ASSERT_EQ(found->value.i, 2, "inner scope value should be 2");
    
    leave();
    
    found = lookup("shared");
    TEST_ASSERT_EQ(found->value.i, 1, "outer scope value should be 1");
}

void test_nested_scopes() {
    reset_for_test();
    
    enter();
    value v1 = {.type = INT, .size = sizeof(int), .value.i = 1};
    insert("x", &v1);
    
    enter();
    value v2 = {.type = INT, .size = sizeof(int), .value.i = 2};
    insert("y", &v2);
    
    TEST_ASSERT_EQ(lookup("x")->value.i, 1, "x should be 1");
    TEST_ASSERT_EQ(lookup("y")->value.i, 2, "y should be 2");
    
    leave();
    TEST_ASSERT_EQ(lookup("x")->value.i, 1, "x still accessible");
    TEST_ASSERT(lookup("y") == NULL, "y should be gone");
    
    leave();
    TEST_ASSERT(lookup("x") == NULL, "x should be gone");
}

void test_multiple_enter_leave() {
    reset_for_test();
    
    for (int i = 0; i < 10; i++) {
        enter();
        value v = {.type = INT, .size = sizeof(int), .value.i = i};
        char name[4] = "v0";
        name[1] = '0' + i;
        insert(name, &v);
        TEST_ASSERT(lookup(name) != NULL, "variable should exist in scope");
    }
    
    for (int i = 9; i >= 0; i--) {
        leave();
    }
    
    TEST_ASSERT(lookup("v0") == NULL, "stack should be empty after all leaves");
}

int main() {
    printf("=== Symbol Table Tests ===\n\n");
    TEST_RUN(test_insert_lookup_int);
    TEST_RUN(test_insert_lookup_float);
    TEST_RUN(test_lookup_not_found);
    TEST_RUN(test_reset_clears_values);
    TEST_RUN(test_enter_scope);
    TEST_RUN(test_insert_in_scope);
    TEST_RUN(test_leave_removes_scoped_var);
    TEST_RUN(test_inner_shadows_outer);
    TEST_RUN(test_nested_scopes);
    TEST_RUN(test_multiple_enter_leave);
    printf("\nAll symbol table tests passed!\n");
    return 0;
}
