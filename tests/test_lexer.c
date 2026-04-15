#include "../src/tokenizer.h"
#include "test.h"
#include <string.h>

void test_tokenize_number() {
  struct token *tokens = tokinize("42");
  TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
  TEST_ASSERT_EQ(tokens->type, LITERAL_INT, "First token should be NUMBER");
  TEST_ASSERT_STR_EQ(tokens->value, "42", "Token value should be 42");
}

void test_tokenize_identifier() {
  struct token *tokens = tokinize("x");
  TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
  TEST_ASSERT_EQ(tokens->type, IDENTIFYER, "First token should be IDENTIFIER");
}

void test_tokenize_var_keyword() {
  struct token *tokens = tokinize("float");
  TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
  TEST_ASSERT_EQ(tokens->type, TYPE_FLOAT, "First token should be KEYWORD_VAR");
}

void test_tokenize_add() {
  struct token *tokens = tokinize("5 + 3");
  TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
  TEST_ASSERT_EQ(tokens->type, LITERAL_INT, "First token should be NUMBER");
  TEST_ASSERT_EQ(tokens->next->type, ADD, "Second token should be ADD");
}

void test_tokenize_assignment() {
  struct token *tokens = tokinize("x = 5");
  TEST_ASSERT(tokens != NULL, "Tokens should not be NULL");
  TEST_ASSERT_EQ(tokens->type, IDENTIFYER, "First token should be IDENTIFIER");
  TEST_ASSERT_EQ(tokens->next->type, TOKEN_ASSIGN,
                 "Second token should be TOKEN_ASSIGN");
}

int main() {
  printf("=== Lexer Tests ===\n");
  TEST_RUN(test_tokenize_number);
  TEST_RUN(test_tokenize_identifier);
  TEST_RUN(test_tokenize_var_keyword);
  TEST_RUN(test_tokenize_add);
  TEST_RUN(test_tokenize_assignment);
  printf("\nAll lexer tests passed!\n");
  return 0;
}
