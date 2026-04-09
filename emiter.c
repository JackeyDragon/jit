#include "ast.h"
#include "symbol_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int execute_statement();
typedef struct code_line {
  ast *code;
  int line_count;
  struct code_line *next;
} code_line;

bool initialized = false;
code_line *head;
code_line *tail;
ast *current_statement;
int current_line = -1;

int init() {
  if (initialized)
    return 0;
  head = malloc(sizeof(code_line));
  head->line_count = 0;
  head->code = NULL;
  head->next = NULL;
  tail = head;
  initialized = true;
  return 0;
}

int eval_expression(ast *expresion, int *status) {
  (*status) = 0;
  if (expresion->type == NODE_NUMBER) {
    return atoi(expresion->value);
  }
  if (expresion->type == NODE_REFERENCE) {
    struct entry *entry = get_entry_by_name(expresion->value);
    if (!entry) {
      (*status) = 1;
      return 0;
    }
    return entry->val;
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

int exec_goto() {
  int status = 0;
  int line = eval_expression(current_statement->rhs, &status);
  code_line *tmp = head;
  while (tmp->line_count != line) {
    if (tmp->next == NULL)
      return 1;
    tmp = tmp->next;
  }
  current_statement = tmp->code;
  current_line = line;
  return status;
}

int declare() {
  int status = 0;
  int val = eval_expression(current_statement->rhs, &status);
  insert(current_statement->value, val);
  return status;
}

code_line *get_next_code_line(int i) {
  code_line *tmp = head;
  while (tmp->line_count != i + 1) {
    if (tmp->next == NULL)
      return NULL;
    tmp = tmp->next;
  }
  return tmp;
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
  case NODE_GOTO:
    exec_goto();
    break;
  default:
    print("bad statement");
    return 1;
  }
  code_line *next_line = get_next_code_line(current_line);
  if (current_statement != NULL && current_statement->next_statement != NULL) {
    current_statement = current_statement->next_statement;
    return execute_statement();
  } else if (next_line != NULL) {
    current_statement = next_line->code;
    current_line++;
  }
  return 0;
}

int exec(ast *statement) {
  ast *clone = clone_ast(statement);
  current_statement = clone;
  init();
  if (!head->code) {
    head->code = clone;
    head->line_count = 0;
    current_line = 0;
  } else {
    tail->next = malloc(sizeof(struct code_line));
    tail->next->line_count = tail->line_count + 1;
    tail->next->code = clone;
    tail->next->next = NULL;
    tail = tail->next;
    current_line = tail->line_count;
  }
  return execute_statement();
}

char *print_expr(ast *node) {
  static char buf[256];
  if (!node) return "";

  switch (node->type) {
  case NODE_NUMBER:
    return node->value;
  case NODE_REFERENCE:
    return node->value;
  case NODE_OPERATION:
  case NODE_EXPRESION: {
    char *op = node->value;
    char *l = print_expr(node->lhs);
    char *r = print_expr(node->rhs);
    snprintf(buf, sizeof(buf), "%s %s %s", l, op, r);
    return buf;
  }
  default:
    return "";
  }
}

char *print_statement(ast *node) {
  static char buf[1024];
  buf[0] = '\0';

  while (node) {
    char stmt[256] = "";

    switch (node->type) {
    case NODE_DECLAR:
      snprintf(stmt, sizeof(stmt), "var %s = %s",
               node->value ? node->value : "",
               print_expr(node->rhs));
      break;
    case NODE_ASSIGN:
      snprintf(stmt, sizeof(stmt), "%s = %s",
               print_expr(node->lhs),
               print_expr(node->rhs));
      break;
    case NODE_IF_CONDITION: {
      char *cond = print_expr(node->rhs);
      snprintf(stmt, sizeof(stmt), "if (%s)", cond);
      break;
    }
    case NODE_GOTO:
      snprintf(stmt, sizeof(stmt), "goto %s", print_expr(node->rhs));
      break;
    default:
      break;
    }

    if (buf[0] != '\0') {
      strcat(buf, "; ");
    }
    strcat(buf, stmt);

    node = node->next_statement;
  }

  return buf;
}

void print_all_lines() {
  if (!initialized || !head) {
    return;
  }

  code_line *tmp = head;
  while (tmp) {
    printf("%d: %s\n", tmp->line_count, print_statement(tmp->code));
    tmp = tmp->next;
  }
}
