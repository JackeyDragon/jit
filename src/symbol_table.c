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

stack *head;
stack *tail;

void init_table() {
  block_table_map_init(&scope_map);
  value_map_init(&root_table);
}

void reset_table() {
  value_map_cleanup(&root_table);
  value_map_init(&root_table);
}

int insert(char *name, value *val) {
  if (lookup(name))
    return -1;
  value_map_insert(&root_table, name, val);
  return 0;
}

value *lookup(char *name) {
  value_map_itr it = value_map_get(&root_table, name);
  if (!value_map_is_end(it)) {
    return it.data->val;
  }
  return NULL;
}

void print_table() {
  for (value_map_itr it = value_map_first(&root_table); !value_map_is_end(it);
       it = value_map_next(it)) {
    printf("[%s: %d]\n", it.data->key, it.data->val->value.i);
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
  if (!head) {
    head = malloc(sizeof(struct stack));
    head->table = entry;
    tail = head;
    head->next = NULL;
    head->previous = NULL;
    if (head)
      return 0;
    return -1;
  }

  tail->next = malloc(sizeof(struct stack));
  tail->next->previous = tail;
  tail = tail->next;
  tail->table = entry;
  return 0;
}

int leave() {
  if (!head)
    return -1;
  tail = tail->previous;
  tail->next = NULL;
  return 0;
}
