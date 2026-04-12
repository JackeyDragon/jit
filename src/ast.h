#ifndef AST_H
#define AST_H

#include "tokenizer.h"

typedef enum {
  NODE_EXPRESION,
  NODE_DECLAR,
  NODE_IF_CONDITION,
  NODE_REFERENCE,
  NODE_ASSIGN,
  NODE_EQUALS_SIGN,
  NODE_OPERATION,
  NODE_NUMBER,
  NODE_KEYWORD,
  NODE_BRACKET_OPEN,
  NODE_BRACKET_CLOSE,
  NODE_GOTO,
  NODE_ROOT
} ast_type;

typedef struct ast {
  ast_type type;
  struct ast *rhs;
  struct ast *lhs;
  struct ast *next_statement;
  char *value;
} ast;

typedef struct {
  int left;
  int right;
} binding_power;

binding_power get_binding_power(enum token_type type);
ast_type token_type_to_ast_type(enum token_type type);
ast *clone_ast(ast *source);

#endif
