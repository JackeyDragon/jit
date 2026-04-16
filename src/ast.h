#ifndef AST_H
#define AST_H

#include "tokenizer.h"
#include "types.h"

typedef enum {
  NODE_EXPRESION,
  NODE_DECLAR,
  NODE_IF_CONDITION,
  NODE_ARRAY_ACCESS,
  NODE_ASSIGN,
  NODE_EQUALS_SIGN,
  NODE_OPERATION,
  NODE_INT,
  NODE_FLOAT,
  NODE_ARRAY,
  NODE_KEYWORD,
  NODE_BRACKET_OPEN,
  NODE_BRACKET_CLOSE,
  NODE_GOTO,
  NODE_BLOCK,
  NODE_TYPE,
  NODE_IDENTIFYER,
  NODE_PARAMETER_DECLARATION,
  NODE_FUNCTION_DECLARATION,
  NODE_FUNCTION_CALL,
  NODE_PARAMETER,
  NODE_ROOT
} ast_type;

typedef struct ast ast;

typedef struct ast {
  ast_type type;
  ast *next_statement;
  union {
    struct LITTERAL {
      value value;
    } LITTERAL;
    struct IDENTIFYER {
      char *name;
    } IDENTIFYER;
    struct EXPRESSION {
      ast *lhs;
      ast *rhs;
      enum token_type operaton;
    } EXPRESSION;
    struct DECLARE {
      ast *identifyer;
      build_in_types type;
      ast *expression;
    } DECLARE;
    struct ASSIGN {
      ast *identifyer;
      ast *expression;
    } ASSIGN;
    struct IF {
      ast *condition;
      ast *if_body;
      ast *else_body;
    } IF;
    struct GOTO {
      ast *expression;
    } GOTO;
    struct BLOCK {
      ast **array;
      int count;
    } BLOCK;
    struct TYPE {
      build_in_types type;
      ast *rhs;
    } TYPE;
    struct FUNCTION_DECLARATION {
      ast *return_type;
      ast *identifyer;
      ast *parameter;
      ast *block;
    } FUNCTION_DECLARATION;
    struct PARAMETER_DECLARATION {
      ast *type;
      ast *name;
      ast *next_param;
    } PARAMETER_DECLARATION;
    struct FUNCTION_CALL {
      char *name;
      ast *params;
    } FUNCTION_CALL;
    struct PARAMETER {
      ast *expression;
      ast *next_param;
    } PARAMETER;
  } data;
} ast;

typedef struct {
  int left;
  int right;
} binding_power;

binding_power get_binding_power(enum token_type type);
ast_type token_type_to_ast_type(enum token_type type);
ast *clone_ast(ast *source);

#endif
