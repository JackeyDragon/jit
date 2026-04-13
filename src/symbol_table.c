#include "symbol_table.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME value_map
#define KEY_TY char *
#define VAL_TY value *
#define HASH_FN vt_hash_string
#define CMPR_FN vt_cmpr_string
#include "verstable.h"

typedef struct block_table block_table;

#define NAME block_table_map
#define KEY_TY char *
#define VAL_TY block_table *
#define HASH_FN vt_hash_string
#define CMPR_FN vt_cmpr_string
#include "verstable.h"

typedef struct symbol {
  char *name;
  value value;
} symbol;

struct block_table {
  int size;
  void *array; // array of symbols
};

typedef struct stack {
  block_table *table;
  struct stack *previous;
  struct stack *next;
} stack;

value_map root_table;
block_table_map scope_map;

stack *bottom;
stack *top;

void init_table() {
  block_table_map_init(&scope_map);
  value_map_init(&root_table);
}

void reset_table() {
  value_map_cleanup(&root_table);
  value_map_init(&root_table);
}

int insert(char *name, value *val) {
  value_map_insert(&root_table, name, val);
  return 0;
}

value *lookup(char *name) {
  stack *tmp = top;

  while (tmp != NULL) {
    block_table *block = tmp->table;
    symbol *arr = (symbol *)block->array;
    for (int i = 0; i < block->size; i++) {
      if (arr[i].name && strcmp(arr[i].name, name) == 0) {
        return &arr[i].value;
      }
    }
    tmp = tmp->previous;
  }

  value_map_itr it = value_map_get(&root_table, name);
  if (!value_map_is_end(it)) {
    return it.data->val;
  }
  return NULL;
}

void print_table() {
  printf("=== Root Table ===\n");
  for (value_map_itr it = value_map_first(&root_table); !value_map_is_end(it);
       it = value_map_next(it)) {
    if (it.data->val->type == FLOAT) {
      printf("[%s: float %f]\n", it.data->key, it.data->val->value.f);
    } else {
      printf("[%s: int %d]\n", it.data->key, it.data->val->value.i);
    }
  }

  printf("=== Scope Stack ===\n");
  stack *tmp = bottom;
  int level = 0;
  while (tmp != NULL) {
    printf("Scope %d:\n", level);
    block_table *block = tmp->table;
    if (block && block->array) {
      symbol *arr = (symbol *)block->array;
      for (int i = 0; i < block->size; i++) {
        if (arr[i].name) {
          if (arr[i].value.type == FLOAT) {
            printf("  [%s: float %f]\n", arr[i].name, arr[i].value.value.f);
          } else {
            printf("  [%s: int %d]\n", arr[i].name, arr[i].value.value.i);
          }
        }
      }
    }
    tmp = tmp->next;
    level++;
  }
}

block_table *lookup_block_table(char *name) {
  block_table_map_itr it = block_table_map_get(&scope_map, name);
  if (!block_table_map_is_end(it)) {
    return it.data->val;
  }
  return NULL;
}

int add_block_table(struct block_table *table, char *name) {
  if (lookup_block_table(name))
    return -1;
  block_table_map_insert(&scope_map, name, table);
  return 0;
}

int enter(char *name) {
  block_table *entry = lookup_block_table(name);
  if (!entry)
    return -1;
  if (!bottom) {
    bottom = malloc(sizeof(struct stack));
    bottom->table = entry;
    top = bottom;
    bottom->next = NULL;
    bottom->previous = NULL;
    if (bottom)
      return 0;
    return -1;
  }

  top->next = malloc(sizeof(struct stack));
  top->next->previous = top;
  top = top->next;
  top->table = entry;
  return 0;
}

int leave() {
  if (!bottom)
    return -1;
  top = top->previous;
  top->next = NULL;
  return 0;
}
