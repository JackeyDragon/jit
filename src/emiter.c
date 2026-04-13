#include "ast.h"
#include "symbol_table.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int execute_statement();
char *print_statement(ast *node);

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

value eval_expression(ast *expresion, int *status) {
  (*status) = 0;
  if (expresion->type == NODE_INT || expresion->type == NODE_FLOAT) {
    return expresion->data.LITTERAL.value;
  }
  if (expresion->type == NODE_IDENTIFYER) {
    value *val = lookup(expresion->data.IDENTIFYER.name);
    if (!val) {
      (*status) = 1;
      return ERROR_VALUE;
    }
    return *val;
  }
  if (expresion->type != NODE_EXPRESION && expresion->type != NODE_OPERATION) {
    (*status) = 1;
    return ERROR_VALUE;
  }

  ast *expr = expresion->type == NODE_OPERATION
                  ? expresion
                  : expresion->data.EXPRESSION.lhs;

  if (expr->data.EXPRESSION.operaton == ADD) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i + rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == MINUS) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i - rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == MULT) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i * rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == DIV) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i / rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == AND) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i && rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == OR) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i || rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == EQUAL) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i == rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == NOT_EQUAL) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    return (value){.type = INT, .size = sizeof(int), .value.i = lhs.value.i != rhs.value.i};
  }
  (*status) = 1;
  return ERROR_VALUE;
}

int exec_assign() {
  int status = 0;
  value tmp = eval_expression(current_statement->data.ASSIGN.expression, &status);
  value *val = valuedup(&tmp);
  insert(current_statement->data.ASSIGN.identifyer->data.IDENTIFYER.name, val);
  return status;
}

int exec_if() {
  int status = 0;
  value tmp = eval_expression(current_statement->data.IF.condition, &status);
  value *val = valuedup(&tmp);

  if (val->value.i == 0) {
    current_statement = current_statement->data.IF.if_body;
  }
  return status;
}

int exec_goto() {
  int status = 0;
  value tmp = eval_expression(current_statement->data.GOTO.expression, &status);
  int line = tmp.value.i;
  code_line *tmp_line = head;
  while (tmp_line->line_count != line) {
    if (tmp_line->next == NULL)
      return 1;
    tmp_line = tmp_line->next;
  }
  current_statement = tmp_line->code;
  current_line = line;
  return status;
}

int declare() {
  int status = 0;
  value tmp = eval_expression(current_statement->data.DECLARE.expression, &status);
  value *val = valuedup(&tmp);
  insert(current_statement->data.DECLARE.identifyer->data.IDENTIFYER.name, val);
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
  while (1) {
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
      continue;
    default:
      print("bad statement");
      return 1;
    }

    code_line *next_line = get_next_code_line(current_line);
    if (current_statement != NULL &&
        current_statement->next_statement != NULL) {
      current_statement = current_statement->next_statement;
      continue;
    } else if (next_line != NULL) {
      current_statement = next_line->code;
      current_line++;
      continue;
    } else {
      break;
    }
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
  if (!node)
    return "";

  switch (node->type) {
  case NODE_INT:
    snprintf(buf, sizeof(buf), "%d", node->data.LITTERAL.value.value.i);
    return buf;
  case NODE_FLOAT:
    snprintf(buf, sizeof(buf), "%f", node->data.LITTERAL.value.value.f);
    return buf;
  case NODE_IDENTIFYER:
    return node->data.IDENTIFYER.name;
  case NODE_OPERATION:
  case NODE_EXPRESION: {
    char *op_str = "";
    switch (node->data.EXPRESSION.operaton) {
    case ADD: op_str = "+"; break;
    case MINUS: op_str = "-"; break;
    case MULT: op_str = "*"; break;
    case DIV: op_str = "/"; break;
    case AND: op_str = "&&"; break;
    case OR: op_str = "||"; break;
    case EQUAL: op_str = "=="; break;
    case NOT_EQUAL: op_str = "!="; break;
    default: op_str = "?"; break;
    }
    char *l = print_expr(node->data.EXPRESSION.lhs);
    char *r = print_expr(node->data.EXPRESSION.rhs);
    snprintf(buf, sizeof(buf), "%s %s %s", l, op_str, r);
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
               node->data.DECLARE.identifyer->data.IDENTIFYER.name, 
               print_expr(node->data.DECLARE.expression));
      break;
    case NODE_ASSIGN:
      snprintf(stmt, sizeof(stmt), "%s = %s", 
               print_expr(node->data.ASSIGN.identifyer),
               print_expr(node->data.ASSIGN.expression));
      break;
    case NODE_IF_CONDITION: {
      char *cond = print_expr(node->data.IF.condition);
      snprintf(stmt, sizeof(stmt), "if (%s)", cond);
      break;
    }
    case NODE_GOTO:
      snprintf(stmt, sizeof(stmt), "goto %s", print_expr(node->data.GOTO.expression));
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
