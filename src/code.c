#include "code.h"
#include "ast.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool initialized = false;

int code_init(void) {
  initialized = true;
  return 0;
}
static ast *head = NULL;
static int current_line = -1;

int code_add(ast *statement) {
  if (!head) {
    head = statement;
  } else {
    head->next_statement = statement;
  }
  return current_line;
}

ast *code_get_line(int line_number) {
  ast *tmp = head;
  int count = 0;
  while (tmp) {
    if (count == line_number) {
      return tmp;
    }
    tmp = tmp->next_statement;
  }
  return NULL;
}

int code_get_current_line(void) { return current_line; }
int set_current_line(int line) {
  current_line = line;
  return 0;
}

static char *print_expr_rec(ast *node, int depth);

char *code_print_expr(ast *node) { return print_expr_rec(node, 0); }

static char *print_expr_rec(ast *node, int depth) {
  static char buf[256];
  buf[0] = '\0';

  if (!node)
    return "";

  switch (node->type) {
  case NODE_INT:
    snprintf(buf, 256, "%d", node->data.LITTERAL.value.value.i);
    return buf;
  case NODE_FLOAT:
    snprintf(buf, 256, "%f", node->data.LITTERAL.value.value.f);
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
    // Use local variables to hold string results before composing
    char lhs_result[128];
    char rhs_result[128];
    snprintf(lhs_result, 128, "%s",
             print_expr_rec(node->data.EXPRESSION.lhs, depth + 1));
    snprintf(rhs_result, 128, "%s",
             print_expr_rec(node->data.EXPRESSION.rhs, depth + 1));
    snprintf(buf, 256, "%s %s %s", lhs_result, op_str, rhs_result);
    return buf;
  }
  default:
    snprintf(buf, 256, "<expr>");
    return buf;
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
    case NODE_ASSIGN: {
      char lhs[256];
      char rhs[256];
      snprintf(lhs, sizeof(lhs), "%s",
               code_print_expr(node->data.ASSIGN.identifyer));
      snprintf(rhs, sizeof(rhs), "%s",
               code_print_expr(node->data.ASSIGN.expression));
      snprintf(stmt, sizeof(stmt), "%s = %s", lhs, rhs);
      break;
    }
    case NODE_IF_CONDITION: {
      char *cond = code_print_expr(node->data.IF.condition);
      snprintf(stmt, sizeof(stmt), "if (%s)", cond);
      break;
    }
    case NODE_GOTO:
      snprintf(stmt, sizeof(stmt), "goto %s",
               code_print_expr(node->data.GOTO.identifyer));
      break;
    default:
      snprintf(stmt, sizeof(stmt), "<stmt>");
      break;
    }

    if (buf[0] != '\0')
      strcat(buf, "; ");
    strcat(buf, stmt);

    node = node->next_statement;
  }

  return buf;
}

void code_print_all(void) {
  if (!head)
    return;

  ast *tmp = head;
  int line_num = 0;
  while (tmp) {
    printf("%d: %s\n", line_num++, code_print_statement(tmp));
    tmp = tmp->next_statement;
  }
}
