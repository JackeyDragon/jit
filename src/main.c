#include "ast.h"
#include "buildin_functions.h"
#include "code.h"
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
  init_table();
  code_init();
  register_functions();

  if (argc > 1) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
      exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
      if (strlen(line) > 0 && line[strlen(line) - 1] == '\n')
        line[strlen(line) - 1] = '\0';

      if (strcmp(line, "list") == 0) {
        code_print_all();
        line = NULL;
        continue;
      }
      struct token *head = tokinize(line);
      current = head;

      ast *tree = parse_statement();
      if (!tree) {
        printf("continue\n");
        line = NULL;
        continue;
      }
      printf("=== AST ===\n");
      print_ast(tree, 0);
      printf("==========\n");
      exec(tree);
    }
    print_table();
    fclose(fp);
    if (line)
      free(line);

  } else {

    while (1) {
      if (input == NULL) {
        input = readline("> ");
        if (!input || strlen(input) == 0) {
          printf("Goodbye!\n");
          break;
        }
      }

      struct token *head = tokinize(input);
      // print_tokens(head);
      current = head;
      if (strcmp(input, "list") == 0) {
        code_print_all();
        input = NULL;
        continue;
      }
      ast *tree = parse_statement();
      if (!tree) {
        printf("syntax error\n");
        input = NULL;
        continue;
      }
      printf("=== AST ===\n");
      print_ast(tree, 0);
      printf("==========\n");
      fflush(stdout);

      // todo semantic analysis
      exec(tree);
      fflush(stdout);
      print_table();
      input = NULL;
    }
  }

  return EXIT_SUCCESS;
}