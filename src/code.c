#include "code.h"
#include "ast.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct code_line {
  ast *code;
  int line_count;
  struct code_line *next;
} code_line;

static bool initialized = false;
static code_line *head = NULL;
static code_line *tail = NULL;
static int current_line = -1;

int code_init(void) {
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

int code_add(ast *statement) {
  if (!initialized)
    code_init();

  if (!head->code) {
    head->code = statement;
    head->line_count = 0;
    current_line = 0;
  } else {
    tail->next = malloc(sizeof(struct code_line));
    tail->next->line_count = tail->line_count + 1;
    tail->next->code = statement;
    tail->next->next = NULL;
    tail = tail->next;
    current_line = tail->line_count;
  }
  return current_line;
}

ast *code_get_line(int line_number) {
  code_line *tmp = head;
  while (tmp) {
    if (tmp->line_count == line_number) {
      return tmp->code;
    }
    tmp = tmp->next;
  }
  return NULL;
}

int code_get_current_line(void) {
  printf("%d", current_line);
  return current_line;
}
int set_current_line(int line) {
  printf("setting current_line to %d", line);
  current_line = line;
  return 0;
}
char *code_print_expr_rec(ast *node, int depth);
char *code_print_expr(ast *node) { return code_print_expr_rec(node, 0); }

char *code_print_expr_rec(ast *node, int depth) {
  // Use two buffers alternating based on depth to avoid overwrites in recursion
  static char buf0[256];
  static char buf1[256];
  char *buf = (depth % 2 == 0) ? buf0 : buf1;

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
    case ADD:
      op_str = "+";
      break;
    case MINUS:
      op_str = "-";
      break;
    case MULT:
      op_str = "*";
      break;
    case DIV:
      op_str = "/";
      break;
    case AND:
      op_str = "&&";
      break;
    case OR:
      op_str = "||";
      break;
    case EQUAL:
      op_str = "==";
      break;
    case NOT_EQUAL:
      op_str = "!=";
      break;
    default:
      op_str = "?";
      break;
    }
    char *l = code_print_expr_rec(node->data.EXPRESSION.lhs, depth + 1);
    char *r = code_print_expr_rec(node->data.EXPRESSION.rhs, depth + 1);
    snprintf(buf, sizeof(buf), "%s %s %s", l, op_str, r);
    return buf;
  }
  default:
    return "";
  }
}

char *code_print_statement(ast *node) {
  static char buf[1024];
  buf[0] = '\0';

  while (node) {
    char stmt[256] = "";

    switch (node->type) {
    case NODE_DECLAR: {
      char *type_str = (node->data.DECLARE.type == FLOAT) ? "float" : "int";
      snprintf(stmt, sizeof(stmt), "%s %s = %s", type_str,
               node->data.DECLARE.identifyer->data.IDENTIFYER.name,
               code_print_expr(node->data.DECLARE.expression));
      break;
    }
    case NODE_ASSIGN:
      snprintf(stmt, sizeof(stmt), "%s = %s",
               code_print_expr(node->data.ASSIGN.identifyer),
               code_print_expr(node->data.ASSIGN.expression));
      break;
    case NODE_IF_CONDITION: {
      char *cond = code_print_expr(node->data.IF.condition);
      if (node->next_statement) {
        char *body = code_print_expr(node->next_statement);
        snprintf(stmt, sizeof(stmt), "if (%s) %s", cond, body);
      } else {
        snprintf(stmt, sizeof(stmt), "if (%s)", cond);
      }
      break;
    }
    case NODE_GOTO:
      snprintf(stmt, sizeof(stmt), "goto %s",
               code_print_expr(node->data.GOTO.expression));
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

void code_print_all(void) {
  if (!initialized || !head) {
    return;
  }

  code_line *tmp = head;
  while (tmp) {
    printf("%d: %s\n", tmp->line_count, code_print_statement(tmp->code));
    tmp = tmp->next;
  }
}
