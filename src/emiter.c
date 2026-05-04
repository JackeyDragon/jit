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
static int exec_function_call_statement(void);
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
  case NODE_TYPE:                                                              \
  case NODE_IDENTIFYER:                                                        \
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
  case NODE_FUNCTION_CALL:                                                     \
    exec_function_call_statement();                                            \
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
  fflush(stdout);
  return 0;
}

value exec_block(ast *block) {
  ast *tmp = current_statement;
  ast *block_start = block->data.BLOCK.array[0];
  return_value = NULL;

  // reset_labels();  // labels are global, don't reset
  for (int i = 0; i < block->data.BLOCK.count; i++) {
    switch (block->data.BLOCK.array[i]->type) {
    case NODE_LABLE:
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
  value *return_tmp = return_value;
  return_value = NULL;
  return *return_tmp;
}

value exec_function_call(function *function, ast *call) {
  if (function->build_in) {
    ast *param_ast = call->data.FUNCTION_CALL.params;
    int actual_count = 0;
    for (ast *p = param_ast; p; p = p->data.PARAMETER.next_param) {
      actual_count++;
    }

    value **params = NULL;
    if (actual_count > 0) {
      params = malloc(sizeof(value *) * actual_count);
      int status = 0;
      int i = 0;
      for (param_ast = call->data.FUNCTION_CALL.params; param_ast;
           param_ast = param_ast->data.PARAMETER.next_param) {
        value tmp =
            eval_expression(param_ast->data.PARAMETER.expression, &status);
        if (status) {
          print("failed to eval param expression in buildin function");
          printf("on param num: %d\n", i);
          free(params);
          return ERROR_VALUE;
        }

        params[i++] = valuedup(&tmp);
      }
    }
    value result = function->c_function(params, actual_count);
    for (int i = 0; i < actual_count; i++) {
      free_value(params[i]);
    }
    free(params);
    return result;
  }
  enter();
  ast *provided = call->data.FUNCTION_CALL.params;
  int status = 0;
  for (int i = 0; i < function->parameter_count && provided; i++) {
    value val = eval_expression(provided->data.PARAMETER.expression, &status);
    if (function->parameter[i].array) {
      if (!val.array || val.type != function->parameter[i].type) {
        print("type miss match in params");
        return ERROR_VALUE;
      }
    } else {
      if (val.type != function->parameter[i].type || val.array) {
        print("type miss match in params");
        return ERROR_VALUE;
      }
    }
    insert(function->parameter[i].name, val);
    provided = provided->data.PARAMETER.next_param;
  }

  if (status)
    return ERROR_VALUE;

  value ret_val = exec_block(function->code_block);
  fflush(stdout);

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
  if (expresion->type == NODE_ARRAY_ACCESS) {
    value *arr =
        lookup(expresion->data.ARRAY_ACCESS.identifyer->data.IDENTIFYER.name);
    if (!arr) {
      (*status) = 1;
      printf("error: array '%s' not found\n",
             expresion->data.ARRAY_ACCESS.identifyer->data.IDENTIFYER.name);
      return ERROR_VALUE;
    }
    if (!arr->array) {
      (*status) = 1;
      printf("error: '%s' is not an array\n",
             expresion->data.ARRAY_ACCESS.identifyer->data.IDENTIFYER.name);
      return ERROR_VALUE;
    }
    value index_val =
        eval_expression(expresion->data.ARRAY_ACCESS.index, status);
    if (*status != 0) {
      return ERROR_VALUE;
    }
    if (index_val.type != INT) {
      (*status) = 1;
      printf("error: array index must be int\n");
      return ERROR_VALUE;
    }
    if (0) {
      (*status) = 1;
      printf(
          "debug: array access out of bounds - name='%s', index=%d, size=%d\n",
          expresion->data.ARRAY_ACCESS.identifyer->data.IDENTIFYER.name,
          index_val.value.i, arr->size);
      printf("error: array index out of bounds\n");
      return ERROR_VALUE;
    }
    value *elements = (value *)arr->value.data;
    return elements[index_val.value.i];
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
  if (expr->data.EXPRESSION.operaton == GREATER_THAN) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type == FLOAT || rhs.type == FLOAT) {
      float l = lhs.type == FLOAT ? lhs.value.f : (float)lhs.value.i;
      float r = rhs.type == FLOAT ? rhs.value.f : (float)rhs.value.i;
      return (value){.type = INT, .size = sizeof(int), .value.i = l > r};
    }
    return (value){
        .type = INT, .size = sizeof(int), .value.i = lhs.value.i > rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == LESS_THAN) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type == FLOAT || rhs.type == FLOAT) {
      float l = lhs.type == FLOAT ? lhs.value.f : (float)lhs.value.i;
      float r = rhs.type == FLOAT ? rhs.value.f : (float)rhs.value.i;
      return (value){.type = INT, .size = sizeof(int), .value.i = l < r};
    }
    return (value){
        .type = INT, .size = sizeof(int), .value.i = lhs.value.i < rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == GREATER_EQUAL) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type == FLOAT || rhs.type == FLOAT) {
      float l = lhs.type == FLOAT ? lhs.value.f : (float)lhs.value.i;
      float r = rhs.type == FLOAT ? rhs.value.f : (float)rhs.value.i;
      return (value){.type = INT, .size = sizeof(int), .value.i = l >= r};
    }
    return (value){.type = INT,
                   .size = sizeof(int),
                   .value.i = lhs.value.i >= rhs.value.i};
  }
  if (expr->data.EXPRESSION.operaton == LESS_EQUAL) {
    value lhs = eval_expression(expr->data.EXPRESSION.lhs, status);
    value rhs = eval_expression(expr->data.EXPRESSION.rhs, status);
    if (lhs.type == FLOAT || rhs.type == FLOAT) {
      float l = lhs.type == FLOAT ? lhs.value.f : (float)lhs.value.i;
      float r = rhs.type == FLOAT ? rhs.value.f : (float)rhs.value.i;
      return (value){.type = INT, .size = sizeof(int), .value.i = l <= r};
    }
    return (value){.type = INT,
                   .size = sizeof(int),
                   .value.i = lhs.value.i <= rhs.value.i};
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

  // Check if this is an array assignment (arr[i] = value)
  if (current_statement->data.ASSIGN.identifyer->type == NODE_ARRAY_ACCESS) {
    ast *arr_access = current_statement->data.ASSIGN.identifyer;
    char *name = arr_access->data.ARRAY_ACCESS.identifyer->data.IDENTIFYER.name;
    value *arr = lookup(name);
    if (!arr) {
      printf("error: undeclared variable '%s'\n", name);
      return 1;
    }
    if (!arr->array) {
      printf("error: '%s' is not an array\n", name);
      return 1;
    }
    value index_val =
        eval_expression(arr_access->data.ARRAY_ACCESS.index, &status);
    if (status != 0) {
      return 1;
    }
    if (index_val.type != INT) {
      printf("error: array index must be int\n");
      return 1;
    }
    if (index_val.value.i < 0 || index_val.value.i > arr->size) {
      printf(
          "debug: array assign out of bounds - name='%s', index=%d, size=%d\n",
          name, index_val.value.i, arr->size);
      printf("error: array index out of bounds\n");
      return 1;
    }
    if (arr->value.data) {
      value *elements = (value *)arr->value.data;
      elements[index_val.value.i] = tmp;
    }
    return 0;
  }

  // Regular assignment
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

  if (tmp.value.i == 0) {
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

    // Handle array from expression (e.g., int x[] = foo(); or int x[100] =
    // foo();)
    if (array_count == 0 || current_statement->data.DECLARE.expression) {
      if (!current_statement->data.DECLARE.expression) {
        printf("error: array must have at least one element\n");
        return 1;
      }
      value val =
          eval_expression(current_statement->data.DECLARE.expression, &status);
      if (status != 0)
        return status;
      if (val.type == ERROR) {
        printf("error: failed to evaluate expression\n");
        return 1;
      }

      insert(current_statement->data.DECLARE.identifyer->data.IDENTIFYER.name,
             val);
      return 0;
    }

    value *elements = malloc(sizeof(value) * array_count);
    for (int i = 0; i < array_count; i++) {
      elements[i] = eval_expression(
          current_statement->data.DECLARE.array_values[i], &status);
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

    value val = (value){.type = declared_type,
                        .array = true,
                        .size = array_count,
                        .value.data = elements};
    insert(current_statement->data.DECLARE.identifyer->data.IDENTIFYER.name,
           val);
    return 0;
  }

  value val =
      eval_expression(current_statement->data.DECLARE.expression, &status);
  if (status != 0) {
    return status;
  }
  if (val.type == ERROR) {
    printf("error: failed to evaluate expression\n");
    return 1;
  }
  if (declared_type != val.type) {
    printf("error: cannot initialize %s variable with %s value\n",
           declared_type == FLOAT ? "float" : "int",
           val.type == FLOAT ? "float" : "int");
    return 1;
  }
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

int exec_function_call_statement() {
  function *fn = lookup_function(current_statement->data.FUNCTION_CALL.name);
  if (!fn) {
    printf("error: function '%s' not found\n",
           current_statement->data.FUNCTION_CALL.name);
    return 1;
  }
  value result = exec_function_call(fn, current_statement);
  if (result.type == ERROR) {
    printf("error: function call failed\n");
    return 1;
  }
  return 0;
}

int exec_lable() { return insert_lable(current_statement); }

int execute_program() {
  while (1) {
    EXEC_STATEMENT(current_statement);
    fflush(stdout);

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
