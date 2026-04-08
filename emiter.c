#include "ast.h"
#include "symbol_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int init() { return 0; }

int eval_expression(ast *expresion, int *status) {
  (*status) = 0;
  if (expresion->type == NODE_NUMBER) {
    return atoi(expresion->value);
  }
  if (expresion->type == NODE_REFERENCE) {
    return get_entry_by_name(expresion->value)->val;
  }
  if (expresion->type != NODE_EXPRESION && expresion->type != NODE_OPERATION) {
    (*status) = 1;
    return 0;
  }

  ast *expr = expresion->type == NODE_OPERATION ? expresion : expresion->lhs;

  if (strcmp(expr->value, "ADD") == 0) {
    return eval_expression(expr->lhs, status) +
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "MINUS") == 0) {
    return eval_expression(expr->lhs, status) -
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "MULT") == 0) {
    return eval_expression(expr->lhs, status) *
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "DIV") == 0) {
    return eval_expression(expr->lhs, status) /
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "AND") == 0) {
    return eval_expression(expr->lhs, status) &&
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "OR") == 0) {
    return eval_expression(expr->lhs, status) ||
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "EQUAL") == 0) {
    return eval_expression(expr->lhs, status) ==
           eval_expression(expr->rhs, status);
  }
  if (strcmp(expr->value, "NOT_EQUAL") == 0) {
    return eval_expression(expr->lhs, status) !=
           eval_expression(expr->rhs, status);
  }
  (*status) = 1;
  return 0;
}

int exec_assign(ast *statement) {
  int status = 0;
  int val = eval_expression(statement->rhs, &status);
  set_entry_val(statement->lhs->value, val);
  return status;
}

int exec_if(ast *statement) { return 1; }

int declare(ast *statement) {
  int status = 0;
  int val = eval_expression(statement->rhs, &status);
  insert(statement->value, val);
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
  case NODE_IF_CONDITION:
    exec_if(statement);
    break;
  case NODE_ASSIGN:
    exec_assign(statement);
    break;
  default:
    print("bad statement");
    return 1;
  }

  if (statement->next_statement != NULL) {
    return execute_statement(statement->next_statement);
  }

  return 0;
}
