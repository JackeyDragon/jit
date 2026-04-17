#include "ast.h"
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
        fprintf(stderr, "DEBUG: file mode parse returned NULL\n");
        printf("continue\n");
        line = NULL;
        continue;
      }
      fprintf(stderr, "DEBUG: file mode parsed ast: %s\n", code_print_statement(tree));
      exec(tree);
    }
    print_table();
    fclose(fp);
    if (line)
      free(line);

  } else {
    char buffer[8192] = {0};
    int brace_depth = 0;

    while (1) {
      if (input == NULL) {
        char *prompt = brace_depth > 0 ? "| " : "> ";
        input = readline(prompt);
        if (!input || strlen(input) == 0) {
          if (brace_depth > 0) {
            printf("error: unclosed block\n");
            input = NULL;
            buffer[0] = '\0';
            brace_depth = 0;
            continue;
          }
          printf("Goodbye!\n");
          break;
        }
      }

      int open_braces = 0;
      int close_braces = 0;
      for (int i = 0; input[i]; i++) {
        if (input[i] == '{') open_braces++;
        if (input[i] == '}') close_braces++;
      }
      brace_depth += open_braces - close_braces;

      if (buffer[0] == '\0') {
        strncpy(buffer, input, 8191);
      } else {
        strncat(buffer, "\n", 8191);
        strncat(buffer, input, 8191);
      }

      if (brace_depth > 0) {
        input = NULL;
        continue;
      }

      char combined[8192] = {0};
      int pos = 0;
      for (int i = 0; buffer[i]; i++) {
        if (buffer[i] != '\n' && buffer[i] != '\r') {
          combined[pos++] = buffer[i];
        }
      }

      struct token *head = tokinize(combined);
      current = head;
      fprintf(stderr, "DEBUG: input='%s'\n", combined);
      while (current) {
        fprintf(stderr, "DEBUG: token type=%s value=%s\n", token_type_to_string(current->type), current->value);
        current = current->next;
      }
      current = head;
      if (strcmp(combined, "list") == 0) {
        code_print_all();
        input = NULL;
        buffer[0] = '\0';
        continue;
      }
      ast *tree = parse_statement();
      if (!tree) {
        fprintf(stderr, "DEBUG: parse_statement returned NULL\n");
        printf("syntax error\n");
        input = NULL;
        buffer[0] = '\0';
        continue;
      }

      fprintf(stderr, "DEBUG: parsed ast: %s\n", code_print_statement(tree));
      exec(tree);
      print_table();
      input = NULL;
      buffer[0] = '\0';
    }
  }

  return EXIT_SUCCESS;
}
