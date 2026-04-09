#include "ast.h"
#include "emiter.h"
#include "symbol_table.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
extern struct token *current;
extern ast *parse_expression(int min_bp);
extern ast *parse_statement();
extern int exec(ast *statement);

static struct termios orig_termios;

static void disable_raw_mode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enable_raw_mode(void) {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &orig_termios);
  raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_iflag |= IUTF8;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static char *readline(char *prompt) {
  static char buf[4096];
  int pos = 0;
  buf[0] = '\0';

  printf("%s", prompt);
  fflush(stdout);
  enable_raw_mode();

  while (1) {
    char c;
    if (read(STDIN_FILENO, &c, 1) <= 0) {
      disable_raw_mode();
      return NULL;
    }
    if (c == '\n' || c == '\r') {
      printf("\n");
      break;
    }
    if (c == 127 || c == '\b') {
      if (pos > 0) {
        pos--;
        fputs("\b \b", stdout);
      }
    } else if (c >= 32 && pos < 4095) {
      buf[pos++] = c;
      putchar(c);
    }
    fflush(stdout);
  }

  disable_raw_mode();
  buf[pos] = '\0';
  return buf;
}

int main(int argc, char *argv[]) {
  char *input = NULL;

  if (argc > 1) {
    input = argv[1];
  }

  while (1) {
    if (input == NULL) {
      input = readline("> ");
      if (!input || strlen(input) == 0) {
        printf("Goodbye!\n");
        break;
      }
    }

    struct token *head = tokinize(input);
    current = head;
    ast *tree = parse_statement();
    if (!tree) {
      print("continue");
      input = NULL;
      continue;
    }
    exec(tree);
    print_table();
    input = NULL;
  }
  return EXIT_SUCCESS;
}
