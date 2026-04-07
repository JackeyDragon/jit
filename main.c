#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct var {
  int val;
  char *name;
  struct var *next;
};

char *parse_name(char *line) {
  line = strdup(line + 4);
  int i = 0;
  while (strncmp(line + i, "=", 1) != 0)
    i++;
  char *name = strdup(line);
  name[i] = '\0';
  strncpy(name, line, i);
  name[strlen(name) - 1] = '\0';
  printf("%s\n", name);
  return name;
}

int parse_literal_int(char *line) {
  int val = 0;
  int i = 0;
  while (strncmp(line + i, "=", 1) != 0)
    i++;
  i++;
  sscanf(line + i, "%d", &val);
  return val;
}

int declare_var(char *line, struct var *head) {
  char *name = parse_name(line);
  int val = parse_literal_int(strdup(line));
  struct var *current = head;
  while (current->next != NULL) {
    printf("%s\n", current->name);
    current = current->next;
  }
  struct var *new = malloc(sizeof(struct var));
  current->next = new;
  new->name = strdup(name);
  new->val = val;
  return 0;
}

int print_var(char *line, struct var *head) {
  struct var *current = head;
  char *param = strdup(line + 6);
  int i = 0;
  while (strncmp(param + i, ")", 1) != 0) {
    i++;
  }
  param[i] = '\0';
  printf("param = %s\n", param);
  while (strcmp(current->name, param)) {
    if (current->next == NULL) {
      printf("no var found");
      return 1;
    }
    current = current->next;
    printf("moving on to %s\n", current->name);
  }
  printf("var val = %d\n", current->val);
  return 0;
}

int parse(char *line, struct var *head) {
  if (strncmp(line, "var ", 4) == 0) {
    declare_var(strdup(line), head);
  } else if (strncmp(line, "print(", 6) == 0) {
    printf("starting print funct call\n");
    print_var(line, head);
  } else {
  }
  return 0;
}

int main(int argc, char *argv[]) {
  struct var *head = malloc(sizeof(struct var));
  head->name = "root";
  head->val = 0;
  // parse("var a = 3", head);
  // parse("var b = 2", head);
  // parse("print(a)", head);

  // goto exit;
  while (true) {
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    parse(line, head);
  }
exit:
  return EXIT_SUCCESS;
}
