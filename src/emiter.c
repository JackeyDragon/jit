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

static value exec_block(ast *block);
static int exec_if(void);
static int exec_assign(void);
static int exec_goto(void);
static int declare(void);
static int exec_declare_function(void);
static int exec_lable(void);
static int execute_program(void);
static int exec_return(void);

#define EXEC_STATEMENT(statement)                                              \
  switch (statement->type) {                                                   \
  case NODE_ROOT:                                                              \
    code_init();                                                               \
    break;                                                                     \
  case NODE_DECLAR:                                                            \
    declare();                                                                 \
    break;                                                                     \
  case NODE_IF_CONDITION:                                                      \
    exec_if();                                                                 \
    break;                                                                     \
  case NODE_ASSIGN:                                                            \
    exec_assign();                                                             \
    break;                                                                     \
  case NODE_GOTO:                                                              \
    exec_goto();                                                               \
    continue;                                                                  \
  case NODE_FUNCTION_DECLARATION:                                              \
    exec_declare_function();                                                   \
    break;                                                                     \
  case NODE_LABLE:                                                             \
    exec_lable();                                                              \
    break;                                                                     \
  case NODE_RETURN:                                                            \
    exec_return();                                                             \
    break;                                                                     \
  case NODE_BLOCK:                                                             \
    exec_block(current_statement);                                             \
    break;                                                                     \
  default:                                                                     \
    printf("bad statement\n");                                                 \
    break;                                                                     \
  }

static ast *current_statement = NULL;
static value *return_value;

ast *emiter_get_current_statement(void) { return current_statement; }

void emiter_set_current_statement(ast *stmt) { current_statement = stmt; }

static int exec_return(void) {
  int status = 0;
  value expression =
      eval_expression(current_statement->data.RETURN.expression, &status);
  if (status)
    return 1;
  value *tmp = valuedup(&expression);
  return_value = tmp;
  return 0;
}

value exec_block(ast *block) {
  ast *tmp = current_statement;
  ast *block_start = block->data.BLOCK.array[0];
  return_value = NULL;

  for (int i = 0; i < block->data.BLOCK.count; i++) {
    switch (block->data.BLOCK.array[i]->type) {
    case NODE_LABLE:
      print("we are inserting some lable");
      insert_lable(block->data.BLOCK.array[i]);
      continue;
    default:
      continue;
    }
  }

  enter();
  current_statement = block_start;
  while (current_statement) {
    EXEC_STATEMENT(current_statement);
    if (return_value)
      break;
    current_statement = current_statement->next_statement;
  }
  leave();
  current_statement = tmp;
  return *return_value;
}

value exec_function_call(function *function, ast *call) {
  enter();
  ast *provided = call->data.FUNCTION_CALL.params;
  int status = 0;
  for (int i = 0; i < function->parameter_count && provided; i++) {
    value val = eval_expression(provided->data.PARAMETER.expression, &status);
    // val.type = function->parameter[i].type;
    if (val.type != function->parameter[i].type)
      return ERROR_VALUE;
    insert(function->parameter[i].name, valuedup(&val));
    provided = provided->data.PARAMETER.next_param;
  }

  if (status)
    return ERROR_VALUE;

  value ret_val = exec_block(function->code_block);

  leave();
  return ret_val;
}

value eval_expression(ast *expresion, int *status) {
  (*status) = 0;
  if (expresion->type == NODE_INT || expresion->type == NODE_FLOAT) {
    return expresion->data.LITTERAL.value;
  }
  if (expresion->type == NODE_FUNCTION_CALL) {
    function *fn = lookup_function(expresion->data.FUNCTION_CALL.name);
    if (!fn) {
      (*status) = 1;
      return ERROR_VALUE;
    }
    return exec_function_call(fn, expresion);
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
  ast *stmt = lookup_lable(
      current_statement->data.GOTO.identifyer->data.IDENTIFYER.name);
  if (!stmt) {
    printf("error: invalid goto target\n");
    return 1;
  }
  if (!stmt->next_statement) {
    print("No statement after lable");
    return 1;
  }
  current_statement = stmt->next_statement;
  return status;
}

int declare() {
  int status = 0;
  build_in_types declared_type = current_statement->data.DECLARE.type;
  
  if (current_statement->data.DECLARE.array) {
    int array_count = current_statement->data.DECLARE.array_count;
    if (array_count == 0) {
      printf("error: array must have at least one element\n");
      return 1;
    }
    
    value *elements = malloc(sizeof(value) * array_count);
    for (int i = 0; i < array_count; i++) {
      elements[i] = eval_expression(current_statement->data.DECLARE.array_values[i], &status);
      if (status != 0) {
        free(elements);
        return status;
      }
      if (elements[i].type != declared_type) {
        printf("error: array element %d type mismatch\n", i);
        free(elements);
        return 1;
      }
    }
    
    value arr_val = (value){
      .type = declared_type,
      .array = true,
      .size = array_count,
      .value.array = elements
    };
    value *val = valuedup(&arr_val);
    insert(current_statement->data.DECLARE.identifyer->data.IDENTIFYER.name, val);
    free(elements);
    return 0;
  }
  
  value tmp = eval_expression(current_statement->data.DECLARE.expression, &status);
  if (status != 0) {
    return status;
  }
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
  if (lookup_function(current_statement->data.FUNCTION_DECLARATION.identifyer
                          ->data.IDENTIFYER.name))
    return 1;

  insert_function(current_statement);

  return 0;
}

int exec_lable() { return insert_lable(current_statement); }

int execute_program() {
  while (1) {
    EXEC_STATEMENT(current_statement);

    current_statement = current_statement->next_statement;

    if (current_statement != NULL) {
      continue;
    } else {
      break;
    }
  }

  return 0;
}

int exec(ast *statement) {
  current_statement = statement;
  code_add(statement);
  return execute_program();
}
