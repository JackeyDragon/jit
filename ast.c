#include "ast.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast_type token_type_to_ast_type(enum token_type type) {
  switch (type) {
  case IDENTIFYER:
    return NODE_IDENTIFIER;
  case NUMBER:
    return NODE_NUMBER;
  case KEYWORD_VAR:
    return NODE_KEYWORD;
  case ADD:
  case MINUS:
  case MULT:
  case DIV:
  case AND:
  case OR:
  case EQUAL:
  case NOT_EQUAL:
    return NODE_OPERATION;
  case TOKEN_ASSIGN:
    return NODE_EQUALS_SIGN;
  case BRACKET_OPEN:
    return NODE_BRACKET_OPEN;
  case BRACKET_CLOSE:
    return NODE_BRACKET_CLOSE;
  default:
    return -1;
  }
}

binding_power get_binding_power(enum token_type type) {
  switch (type) {
  case EQUAL:
  case NOT_EQUAL:
    return (binding_power){.left = 10, .right = 9};
  case AND:
  case OR:
    return (binding_power){.left = 10, .right = 9};
  case ADD:
  case MINUS:
    return (binding_power){.left = 10, .right = 9};
  case MULT:
  case DIV:
    return (binding_power){.left = 10, .right = 9};
  default:
    return (binding_power){.left = -1, .right = -1};
  }
}

int main(int argc, char *argv[]) { return EXIT_SUCCESS; }
