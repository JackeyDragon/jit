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

int main(int argc, char *argv[]) {
  insert("a", 1);
  insert("b", 2);
  insert("c", 3);

  print_entry(head);
  printf("\n");
  print_entry(head->next);
  printf("\n");
  print_table();

  print_entry(get_entry_by_name("b"));
  set_entry_val("b", 69);
  printf("\n");
  print_entry(get_entry_by_name("b"));
  return 0;
}
