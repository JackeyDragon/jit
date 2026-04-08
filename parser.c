#include "symbol_table.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define INPUT_BUF_SIZE 4096

static struct termios orig_termios;

static void disable_raw_mode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enable_raw_mode(void) {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &orig_termios);
  raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static char *readline(char *prompt) {
  static char buf[INPUT_BUF_SIZE];
  int pos = 0;
  printf("%s", prompt);
  fflush(stdout);
  enable_raw_mode();
  while (1) {
    char c;
    if (read(STDIN_FILENO, &c, 1) <= 0) {
      disable_raw_mode();
      return NULL;
    }
    if (c == '\n') {
      printf("\n");
      break;
    }
    if (c == 127 || c == '\b') {
      if (pos > 0) {
        pos--;
        printf("\b \b");
        fflush(stdout);
      }
    } else if (c >= 32 && pos < INPUT_BUF_SIZE - 1) {
      buf[pos++] = c;
      printf("%c", c);
      fflush(stdout);
    }
  }
  disable_raw_mode();
  buf[pos] = '\0';
  return buf;
}

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
  while (1) {
    char *input = readline("> ");
    if (input == NULL) {
      printf("Goodbye!\n");
      break;
    }
    if (strlen(input) == 0) {
      continue;
    }
    parse(input);
  }
  return 0;
}
