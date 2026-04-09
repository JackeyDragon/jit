#include "ast.h"
#include "tokenizer.h"

struct token;
extern struct token *current;

typedef struct ast ast;
extern ast *parse_expression(int min_bp);
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast_type token_type_to_ast_type(enum token_type type) {
  switch (type) {
  case IDENTIFYER:
    return NODE_REFERENCE;
  case NUMBER:
    return NODE_NUMBER;
  case KEYWORD_IF:
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

ast *clone_ast(ast *source) {
  if (!source)
    return NULL;
  ast *ast_new = malloc(sizeof(struct ast));
  ast_new->lhs = clone_ast(source->lhs);
  ast_new->rhs = clone_ast(source->rhs);
  ast_new->next_statement = clone_ast(source->next_statement);
  ast_new->type = source->type;
  ast_new->value = source->value ? strdup(source->value) : NULL;
  return ast_new;
}

binding_power get_binding_power(enum token_type type) {
  switch (type) {
  case EQUAL:
  case NOT_EQUAL:
    return (binding_power){.left = 1, .right = 0.9};
  case AND:
  case OR:
    return (binding_power){.left = 1, .right = 0.9};
  case ADD:
  case MINUS:
    return (binding_power){.left = 2, .right = 2.1};
  case MULT:
  case DIV:
    return (binding_power){.left = 3, .right = 3.1};
  default:
    return (binding_power){.left = -1, .right = -1};
  }
}

void print_ast(ast *node, int depth) {
  if (!node)
    return;
  for (int i = 0; i < depth; i++)
    printf("  ");
  switch (node->type) {
  case NODE_DECLAR:
    printf("DECLAR: %s\n", node->value);
    break;
  case NODE_ASSIGN:
    printf("ASSIGN\n");
    break;
  case NODE_NUMBER:
    printf("NUMBER: %s\n", node->value);
    break;
  case NODE_REFERENCE:
    printf("REFERENCE: %s\n", node->value);
    break;
  case NODE_OPERATION:
    printf("OPERATION: %s\n", node->value);
    break;
  case NODE_KEYWORD:
    printf("KEYWORD: %s\n", node->value);
    break;
  case NODE_ROOT:
    printf("ROOT\n");
    break;
  case NODE_IF_CONDITION:
    printf("IF_CONDITION\n");
    break;
  default:
    printf("UNKNOWN (type=%d)\n", node->type);
  }
  print_ast(node->lhs, depth + 1);
  print_ast(node->rhs, depth + 1);
  if (node->next_statement) {
    for (int i = 0; i < depth; i++)
      printf("  ");
    printf("-> next_statement:\n");
    print_ast(node->next_statement, depth);
  }
}
