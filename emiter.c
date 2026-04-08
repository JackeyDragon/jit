#include "ast.h"
#include "symbol_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry *head;
int init() { return 0; }

int eval_expression(ast *expresion, int *status) {
  (*status) = 0;
  if (expresion->type != NODE_EXPRESION) {
    (*status) = 1;
    return 0;
  }

  ast *lhs = expresion->lhs;

  switch (lhs->type) {
  case NODE_NUMBER:
    return atoi(lhs->value);
  case NODE_REFERENCE:
    return get_entry_by_name(lhs->value)->val;
  case NODE_OPERATION:
    if (strcmp(lhs->value, "+") == 0) {
      return eval_expression(lhs->lhs, status) +
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "-") == 0) {
      return eval_expression(lhs->lhs, status) -
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "*") == 0) {
      return eval_expression(lhs->lhs, status) *
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "/") == 0) {
      return eval_expression(lhs->lhs, status) /
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "&&") == 0) {
      return eval_expression(lhs->lhs, status) &&
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "||") == 0) {
      return eval_expression(lhs->lhs, status) ||
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "==") == 0) {
      return eval_expression(lhs->lhs, status) ==
             eval_expression(lhs->rhs, status);
    }
    if (strcmp(lhs->value, "!=") == 0) {
      return eval_expression(lhs->lhs, status) !=
             eval_expression(lhs->rhs, status);
    }
    (*status) = 1;
    return 0;
  default:
    (*status) = 1;
    return 0;
  }

  return 0;
}

int declare(ast *statement) {
  int status = 0;
  char *name = statement->value;
  int val = eval_expression(statement->lhs, &status);
  print_table();
  return status;
}

int execute_statement(ast *statement) {

  switch (statement->type) {
  case NODE_ROOT:
    init();
    break;
  case NODE_DECLAR:
    declare(statement);
    break;
  default:
    print("bad statement");
  }

  return 0;
}

int main(int argc, char *argv[]) { return EXIT_SUCCESS; }
