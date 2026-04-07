#include "symbol_table.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct token *current;

int expect(enum token_type type) {
  if (!current) {
    return 1;
  }
  if (current->type == type) {
    current = current->next;
    return 0;
  } else {
    return 1;
  }
}

int parse_declare() {
  if (expect(KEYWORD_VAR)) {
    printf("Error: expected 'var'\n");
    return 1;
  }
  struct token *name = current;
  if (expect(IDENTIFYER)) {
    printf("Error: expected identifier after 'var'\n");
    return 1;
  }

  if (expect(TOKEN_ASSIGN)) {
    printf("Error: expected '=' after identifier\n");
    return 1;
  }
  struct token *val = current;
  if (expect(NUMBER)) {
    printf("Error: expected number after '='\n");
    return 1;
  }

  if (expect(SEMICOLON)) {
    printf("Error: expected ';' at end of declaration\n");
    return 1;
  }

  insert(name->value, atoi(val->value));

  return 0;
}

int parse_assign() {
  struct token *identifier = current;
  if (expect(IDENTIFYER)) {
    printf("Error: expected identifier\n");
    return 1;
  }

  if (expect(TOKEN_ASSIGN)) {
    printf("Error: expected '=' after identifier\n");
    return 1;
  }
  struct token *nummber = current;
  if (expect(NUMBER)) {
    printf("Error: expected number after '='\n");
    return 1;
  }

  if (expect(SEMICOLON)) {
    printf("Error: expected ';' at end of assignment\n");
    return 1;
  }
  set_entry_val(identifier->value, atoi(nummber->value));
  return 0;
}

int parse_S() {
  int error = 0;
  printf("\n");
  while (current != NULL) {
    if (current->type == KEYWORD_VAR) {
      error += parse_declare();
    } else if (current->type == IDENTIFYER) {
      error += parse_assign();
    } else {
      return 1;
    }
  }
  return error;
}

int parse(char *input) {
  struct token *head;
  head = tokinize(input);
  print_tokens(head);
  current = head;
  if (parse_S() == 0) {
    print("\nparsing successfull");
  } else {
    print("\nparsing failed");
    return -1;
  }
  print_table();
  return 0;
}

int main(int argc, char *argv[]) {
  goto test;
  char input[100];
  printf("please input your string: ");
  fgets(input, sizeof(input), stdin);
test:
  parse("          var a = 123; var b = 555;");
  print("changing b");
  parse("b = 3;");
  return 0;
}
