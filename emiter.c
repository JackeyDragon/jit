#include "ast.h"
#include "symbol_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int execute_statement();
int count = 0; // counts how many things have been executed. This way we print
               // symbol_table only once

// todo linked list of lines
typedef struct code_line {
  ast *code;
  int line_count;
  struct code_line *next;
} code_line;

code_line *head;
code_line *tail;
ast *current_statement;

int init() {
  head = malloc(sizeof(code_line));
  head->line_count = 0;
  head->code = current_statement;
  head->next = NULL;
  tail = head;
  return 0;
}

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

int exec_assign() {
  int status = 0;
  int val = eval_expression(current_statement->rhs, &status);
  set_entry_val(current_statement->lhs->value, val);
  return status;
}

int exec_if() {
  int status = 0;
  int val = eval_expression(current_statement->rhs, &status);

  if (val == 0) {
    ast *body = current_statement->next_statement;
    current_statement = body ? body->next_statement : NULL;
  }
  return status;
}

int declare() {
  int status = 0;
  int val = eval_expression(current_statement->rhs, &status);
  insert(current_statement->value, val);
  return status;
}

int execute_statement() {
  switch (current_statement->type) {
  case NODE_ROOT:
    init();
    break;
  case NODE_DECLAR:
    declare();
    break;
  case NODE_IF_CONDITION:
    exec_if();
    break;
  case NODE_ASSIGN:
    exec_assign();
    break;
  default:
    print("bad statement");
    return 1;
  }

  if (current_statement != NULL && current_statement->next_statement != NULL) {
    current_statement = current_statement->next_statement;
    return execute_statement();
  }
  count = 0;
  return 0;
}

int exec(ast *statement) {
  ast *clone = clone_ast(statement);
  current_statement = clone;
  return execute_statement();
}
