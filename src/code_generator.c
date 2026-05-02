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
#include <stdio.h>
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

void print_attomic_expr(attomic_expr *expr) {
  if (!expr)
    return;
  if (expr->op) {
    printf("Operator: %s\n", token_type_to_string(expr->op));
  }
  if (expr->lhs) {
    printf("LHS:\n");
    print_ast(expr->lhs, 0);
  }
  if (expr->rhs) {
    printf("RHS:\n");
    print_ast(expr->rhs, 0);
  }
  if (expr->next) {
    printf("Next:\n");
    print_attomic_expr(expr->next);
  }
}

#ifdef TEST_ATOMIC

extern struct token *current;
extern ast *parse_statement();

int main(int argc, char *argv[]) {
  // Parse: int x = (3 + 2) * a[3] + b * 4;
  current = tokinize("int x = (3 + 2) * a[3] + b * 4;");

  ast *stmt = parse_statement();
  if (!stmt) {
    printf("Parsing failed\n");
    return 1;
  }

  if (stmt->type != NODE_DECLAR || !stmt->data.DECLARE.expression) {
    printf("Not a declaration with expression\n");
    return 1;
  }

  ast *expr = stmt->data.DECLARE.expression;
  printf("Original Expression AST:\n");
  print_ast(expr, 0);

  attomic_expr *atomic = expr_to_attomic(expr);
  if (!atomic) {
    printf("Atomic conversion failed\n");
    return 1;
  }

  printf("\nAtomic Expression:\n");
  print_attomic_expr(atomic);

  return 0;
}

#endif
