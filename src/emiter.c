#include "emiter.h"
#include "ast.h"
#include "code.h"
#include "symbol_table.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static ast *current_statement = NULL;

ast *emiter_get_current_statement(void) { return current_statement; }

void emiter_set_current_statement(ast *stmt) { current_statement = stmt; }

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
    if (lhs.type != rhs.type) {
      (*status) = 1;
      printf("error: cannot mix int and float in expression\n");
      return ERROR_VALUE;
    }
    if (lhs.type == FLOAT) {
      float l = lhs.value.f;
      float r = rhs.value.f;
      return (value){.type = FLOAT, .size = sizeof(float), .value.f = l + r};
    }
    return (value){
        .type = INT, .size = sizeof(int), .value.i = lhs.value.i + rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == MINUS) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type != rhs.type) {
      (*status) = 1;
      printf("error: cannot mix int and float in expression\n");
      return ERROR_VALUE;
    }
    if (lhs.type == FLOAT) {
      float l = lhs.value.f;
      float r = rhs.value.f;
      return (value){.type = FLOAT, .size = sizeof(float), .value.f = l - r};
    }
    return (value){
        .type = INT, .size = sizeof(int), .value.i = lhs.value.i - rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == MULT) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type != rhs.type) {
      (*status) = 1;
      printf("error: cannot mix int and float in expression\n");
      return ERROR_VALUE;
    }
    if (lhs.type == FLOAT) {
      float l = lhs.value.f;
      float r = rhs.value.f;
      return (value){.type = FLOAT, .size = sizeof(float), .value.f = l * r};
    }
    return (value){
        .type = INT, .size = sizeof(int), .value.i = lhs.value.i * rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == DIV) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type != rhs.type) {
      (*status) = 1;
      printf("error: cannot mix int and float in expression\n");
      return ERROR_VALUE;
    }
    if (lhs.type == FLOAT) {
      float l = lhs.value.f;
      float r = rhs.value.f;
      return (value){.type = FLOAT, .size = sizeof(float), .value.f = l / r};
    }
    return (value){
        .type = INT, .size = sizeof(int), .value.i = lhs.value.i / rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == AND) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type != rhs.type) {
      (*status) = 1;
      printf("error: cannot mix int and float in expression\n");
      return ERROR_VALUE;
    }
    return (value){.type = INT,
                   .size = sizeof(int),
                   .value.i = lhs.value.i && rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == OR) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type != rhs.type) {
      (*status) = 1;
      printf("error: cannot mix int and float in expression\n");
      return ERROR_VALUE;
    }
    return (value){.type = INT,
                   .size = sizeof(int),
                   .value.i = lhs.value.i || rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == EQUAL) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type == FLOAT || rhs.type == FLOAT) {
      float l = lhs.type == FLOAT ? lhs.value.f : (float)lhs.value.i;
      float r = rhs.type == FLOAT ? rhs.value.f : (float)rhs.value.i;
      return (value){.type = INT, .size = sizeof(int), .value.i = l == r};
    }
    return (value){.type = INT,
                   .size = sizeof(int),
                   .value.i = lhs.value.i == rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == NOT_EQUAL) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type == FLOAT || rhs.type == FLOAT) {
      float l = lhs.type == FLOAT ? lhs.value.f : (float)lhs.value.i;
      float r = rhs.type == FLOAT ? rhs.value.f : (float)rhs.value.i;
      return (value){.type = INT, .size = sizeof(int), .value.i = l != r};
    }
    return (value){.type = INT,
                   .size = sizeof(int),
                   .value.i = lhs.value.i != rhs.value.i};
  }
  (*status) = 1;
  return ERROR_VALUE;
}

int exec_assign() {
  int status = 0;
  value tmp =
      eval_expression(current_statement->data.ASSIGN.expression, &status);
  if (status != 0) {
    return status;
  }
  char *name = current_statement->data.ASSIGN.identifyer->data.IDENTIFYER.name;
  value *existing = lookup(name);
  if (!existing) {
    printf("error: undeclared variable '%s'\n", name);
    return 1;
  }
  if (existing->type != tmp.type) {
    printf("error: cannot assign %s to %s variable '%s'\n",
           tmp.type == FLOAT ? "float" : "int",
           existing->type == FLOAT ? "float" : "int", name);
    return 1;
  }
  overwrite_value(tmp, existing);
  return status;
}

int exec_if() {
  int status = 0;
  value tmp = eval_expression(current_statement->data.IF.condition, &status);
  value *val = valuedup(&tmp);

  if (val->value.i == 0) {
    current_statement = current_statement->next_statement;
  }
  return status;
}

int exec_goto() {
  int status = 0;
  value tmp = eval_expression(current_statement->data.GOTO.expression, &status);
  if (tmp.type != INT) {
    printf("error: goto requires int\n");
    return 1;
  }
  int line = tmp.value.i;
  ast *line_code = code_get_line(line);
  if (!line_code) {
    printf("error: invalid goto target\n");
    return 1;
  }
  set_current_line(line);
  current_statement = line_code;
  return status;
}

int declare() {
  int status = 0;
  value tmp =
      eval_expression(current_statement->data.DECLARE.expression, &status);
  if (status != 0) {
    return status;
  }
  build_in_types declared_type = current_statement->data.DECLARE.type;
  if (declared_type != tmp.type) {
    printf("error: cannot initialize %s variable with %s value\n",
           declared_type == FLOAT ? "float" : "int",
           tmp.type == FLOAT ? "float" : "int");
    return 1;
  }
  value *val = valuedup(&tmp);
  insert(current_statement->data.DECLARE.identifyer->data.IDENTIFYER.name, val);
  return status;
}
int exec_declare_function() {
  // todo create and store function, caluclate and register new scope

  return 0;
}
int execute_statement() {
  while (1) {
    switch (current_statement->type) {
    case NODE_ROOT:
      code_init();
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
    case NODE_FUNCTION_DECLARATION:
      exec_declare_function();
      break;
    default:
      printf("bad statement\n");
      return 1;
    }

    if (current_statement != NULL &&
        current_statement->next_statement != NULL) {
      current_statement = current_statement->next_statement;
      continue;
    } else {
      int next_line_num = code_get_current_line() + 1;
      ast *next_line = code_get_line(next_line_num);
      if (next_line != NULL) {
        set_current_line(next_line_num);
        current_statement = next_line;
        continue;
      } else {
        break;
      }
    }
  }
  return 0;
}

int exec(ast *statement) {
  ast *clone = clone_ast(statement);
  current_statement = clone;
  code_add(clone);
  return execute_statement();
}
