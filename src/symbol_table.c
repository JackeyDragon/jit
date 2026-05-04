#include "symbol_table.h"
#include "ast.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void value_dtor(value val);
#define VAL_DTOR_FN value_dtor

#define NAME value_map
#define KEY_TY char *
#define VAL_TY value
#define HASH_FN vt_hash_string
#define CMPR_FN vt_cmpr_string
#include "verstable.h"

typedef struct stack {
  value_map *scope;
  struct stack *previous;
  struct stack *next;
} stack;

#define NAME function_map
#define KEY_TY char *
#define VAL_TY function *
#define HASH_FN vt_hash_string
#define CMPR_FN vt_cmpr_string
#include "verstable.h"

#define NAME lable_map
#define KEY_TY char *
#define VAL_TY ast *
#define HASH_FN vt_hash_string
#define CMPR_FN vt_cmpr_string
#include "verstable.h"

value_map global_scope;
function_map functions;
lable_map lables;

stack *bottom = NULL;
stack *top = NULL;

void init_table() {
  value_map_init(&global_scope);
  lable_map_init(&lables);
  function_map_init(&functions);
}

void reset_table() {
  value_map_cleanup(&global_scope);
  value_map_init(&global_scope);
}

void cleanup_all_scopes() {
  while (top) leave();
  value_map_cleanup(&global_scope);
  value_map_init(&global_scope);
  code_cleanup();
}

int insert(char *name, value val) {
  if (top && top->scope) {
    value_map_insert(top->scope, name, val);
  } else {
    value_map_insert(&global_scope, name, val);
  }
  return 0;
}

value *lookup(char *name) {
  stack *tmp = top;

  while (tmp != NULL) {
    value_map_itr it = value_map_get(tmp->scope, name);
    if (!value_map_is_end(it)) {
      return &it.data->val;
    }
    tmp = tmp->previous;
  }

  value_map_itr it = value_map_get(&global_scope, name);
  if (!value_map_is_end(it)) {
    return &it.data->val;
  }
  return NULL;
}

void print_table() {
  printf("=== Global Scope ===\n");
  for (value_map_itr it = value_map_first(&global_scope); !value_map_is_end(it);
       it = value_map_next(it)) {
    if (it.data->val.array) {
      printf("[%s: array of %s, size %d]\n", it.data->key,
             it.data->val.type == FLOAT ? "float" : "int", it.data->val.size);
    } else if (it.data->val.type == FLOAT) {
      printf("[%s: float %f]\n", it.data->key, it.data->val.value.f);
    } else {
      printf("[%s: int %d]\n", it.data->key, it.data->val.value.i);
    }
  }

  printf("=== Scope Stack ===\n");
  stack *tmp = bottom;
  int level = 0;
  while (tmp != NULL) {
    printf("Scope %d:\n", level);
    for (value_map_itr it = value_map_first(tmp->scope); !value_map_is_end(it);
         it = value_map_next(it)) {
      if (it.data->val.array) {
        printf("  [%s: array of %s, size %d]\n", it.data->key,
               it.data->val.type == FLOAT ? "float" : "int", it.data->val.size);
      } else if (it.data->val.type == FLOAT) {
        printf("  [%s: float %f]\n", it.data->key, it.data->val.value.f);
      } else {
        printf("  [%s: int %d]\n", it.data->key, it.data->val.value.i);
      }
    }
    tmp = tmp->next;
    level++;
  }
}

int enter(void) {
  value_map *new_scope = malloc(sizeof(value_map));
  if (!new_scope)
    return -1;
  value_map_init(new_scope);

  stack *node = malloc(sizeof(stack));
  if (!node) {
    free(new_scope);
    return -1;
  }
  node->scope = new_scope;
  node->next = NULL;

  if (!top) {
    bottom = top = node;
    node->previous = NULL;
  } else {
    node->previous = top;
    top->next = node;
    top = node;
  }
  return 0;
}

int leave(void) {
  if (!top)
    return -1;

  stack *prev = top->previous;
  value_map_cleanup(top->scope);
  free(top->scope);
  free(top);
  top = prev;

  if (top) {
    top->next = NULL;
  } else {
    bottom = NULL;
  }
  return 0;
}

static int count_parameters(ast *param) {
  int count = 0;
  while (param) {
    count++;
    param = param->data.PARAMETER_DECLARATION.next_param;
  }
  return count;
}

int insert_function(ast *func_ast) {
  if (func_ast->type != NODE_FUNCTION_DECLARATION) {
    return -1;
  }

  function *func = malloc(sizeof(function));
  func->name =
      func_ast->data.FUNCTION_DECLARATION.identifyer->data.IDENTIFYER.name;
  func->return_type = func_ast->data.FUNCTION_DECLARATION.return_type;
  func->code_block = func_ast->data.FUNCTION_DECLARATION.block;
  func->c_function = NULL;
  func->build_in = false;

  func->parameter_count =
      count_parameters(func_ast->data.FUNCTION_DECLARATION.parameter);
  func->parameter = malloc(sizeof(parameter) * func->parameter_count);

  ast *param_ast = func_ast->data.FUNCTION_DECLARATION.parameter;
  for (int i = 0; i < func->parameter_count; i++) {
    ast *param_type = param_ast->data.PARAMETER_DECLARATION.type;
    func->parameter[i].name =
        param_ast->data.PARAMETER_DECLARATION.name->data.IDENTIFYER.name;
    func->parameter[i].array = (param_type->data.TYPE.type == ARRAY);
    if (param_type->data.TYPE.type == ARRAY && param_type->data.TYPE.rhs) {
      func->parameter[i].type = param_type->data.TYPE.rhs->data.TYPE.type;
    } else {
      func->parameter[i].type = param_type->data.TYPE.type;
    }
    param_ast = param_ast->data.PARAMETER_DECLARATION.next_param;
  }

  function_map_insert(&functions, func->name, func);
  return 0;
}

int insert_function_struct(function *func) {
  if (!func || !func->name) {
    return -1;
  }

  function_map_insert(&functions, func->name, func);
  return 0;
}

function *lookup_function(char *name) {
  function_map_itr it = function_map_get(&functions, name);
  if (!function_map_is_end(it)) {
    return it.data->val;
  }
  return NULL;
}

int insert_lable(ast *statement) {
  if (statement->type != NODE_LABLE)
    return -1;
  lable_map_insert(&lables,
                   statement->data.LABLE.identifyer->data.IDENTIFYER.name,
                   statement);
  return 0;
}

ast *lookup_lable(char *name) {
  lable_map_itr it = lable_map_get(&lables, name);
  if (!lable_map_is_end(it)) {
    return it.data->val;
  }
  return NULL;
}

void value_dtor(value val) {
  if (val.array)
    free(val.value.data);

  return;
}
