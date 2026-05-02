/* todo
 * some function (function function) {
 * calc prolog
 * calc epilog
 * switch (NODE_TYPE) // this is the body
 * malloc size of code
 * return code ptr;
 * }
 *
 * */
#include "ast.h"
#include "symbol_table.h"
#include <stdlib.h>

typedef struct attomic_expr attomic_expr;
attomic_expr *expr_to_attomic(ast *expr);

#define SWITCH_EXPR(ret_expr, expr, site)                                      \
  switch (expr->data.EXPRESSION.site->type) {                                  \
  case NODE_IDENTIFYER:                                                        \
  case NODE_FUNCTION_CALL:                                                     \
  case NODE_ARRAY_ACCESS:                                                      \
    ret_expr->site = expr;                                                     \
    break;                                                                     \
  default:                                                                     \
    ret_expr->next = expr_to_attomic(expr->data.EXPRESSION.rhs);               \
  }

typedef struct attomic_expr {
  int name;
  enum token_type op;
  ast *rhs;
  ast *lhs;
  attomic_expr *next;
} attomic_expr;

attomic_expr *expr_to_attomic(ast *expr) {
  attomic_expr *ret_val = malloc(sizeof(attomic_expr));
  ret_val->next = NULL;

  SWITCH_EXPR(ret_val, expr, rhs);

  SWITCH_EXPR(ret_val, expr, lhs)

  return ret_val;
}

void *compile(function *function) { return NULL; }
