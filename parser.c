#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct token *current;

int expect(enum token_type type) {
  if (!current)
    return 1;
  if (current->type == type) {
    current = current->next;
    return 0;
  } else {
    return 1;
  }
}

int parse_declare() {
  if (!expect(KEYWORD_VAR)) {
    printf("Error: expected 'var' but got %s with val %s\n",
           token_type_to_string(current->type), current->value);
    return 1;
  }

  if (!expect(IDENTIFYER)) {
    printf("Error: expected identifier after 'var'\n");
    return 1;
  }

  if (!expect(TOKEN_ASSIGN)) {
    printf("Error: expected '=' after identifier\n");
    return 1;
  }

  if (!expect(NUMBER)) {
    printf("Error: expected number after '='\n");
    return 1;
  }

  if (!expect(SEMICOLON)) {
    printf("Error: expected ';' at end of declaration\n");
    return 1;
  }

  return 0;
}

int parse_assign() {
  if (expect(IDENTIFYER)) {
    printf("Error: expected identifier\n");
    return 1;
  }

  if (!expect(TOKEN_ASSIGN)) {
    printf("Error: expected '=' after identifier\n");
    return 1;
  }

  if (!expect(NUMBER)) {
    printf("Error: expected number after '='\n");
    return 1;
  }

  if (!expect(SEMICOLON)) {
    printf("Error: expected ';' at end of assignment\n");
    return 1;
  }

  return 0;
}

int parse() {
  int error = 0;
  printf("\n");
  while (current->next != NULL) {
    if (current->type == KEYWORD_VAR) {
      error += parse_declare();
    } else if (current->type == TOKEN_ASSIGN) {
      error += parse_assign();
    } else {
      return 1;
    }
  }
  return error;
}

int main(int argc, char *argv[]) {
  struct token *head;
  goto test;
  char input[100];
  printf("please input your string: ");
  fgets(input, sizeof(input), stdin);
test:
  head = tokinize("          var a = 123; var var b = 555;");
  print_tokens(head);
  current = head;
  if (parse() == 0) {
    print("\nparsing successfull");
  } else {
    print("\nparsing failed");
  }
  return 0;
}
