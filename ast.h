#include "tokenizer.h"

typedef enum {
  NODE_EXPRESION,
  NODE_DECLAR,
  NODE_IDENTIFIER,
  NODE_ASSIGN,
  NODE_EQUALS_SIGN,
  NODE_OPERATION,
  NODE_NUMBER,
  NODE_REFERENCE,
  NODE_KEYWORD,
  NODE_BRACKET_OPEN,
  NODE_BRACKET_CLOSE
} ast_type;

typedef struct ast {
  ast_type type;
  struct ast *rhs;
  struct ast *lhs;
  char *value;
} ast;

typedef struct {
  int left;
  int right;
} binding_power;

binding_power get_binding_power(enum token_type type);
ast_type token_type_to_ast_type(enum token_type type);
