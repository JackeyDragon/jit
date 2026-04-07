#include "symbol_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry *head;

void print_entry(struct entry *entry) {
  printf("[%s: %d]", entry->name, entry->val);
}

int insert(char *name, int val) {
  if (head == NULL) {
    head = malloc(sizeof(struct entry));
    head->val = val;
    head->name = strdup(name);
    return 0;
  }

  struct entry *current = head;

  while (current->next != NULL) {
    if (strcmp(current->name, name) == 0)
      return -1;
    current = current->next;
  }

  current->next = malloc(sizeof(struct entry));
  current = current->next;
  current->name = strdup(name);
  current->val = val;
  current->next = NULL;
  return 0;
}

struct entry *get_entry_by_name(char *name) {
  struct entry *current = head;

  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

int set_entry_val(char *name, int value) {
  struct entry *entry = get_entry_by_name(name);
  if (entry) {
    entry->val = value;
    return 0;
  }
  return -1;
}

void print_table() {
  struct entry *current = head;

  while (current != NULL) {
    print_entry(current);
    printf("\n");
    current = current->next;
  }

  return;
}
