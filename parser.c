#include "ast.h"
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
struct ast *ast_head;
struct ast *ast_tail;

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

ast *parse_expression(int min_bp) {
  if (!current) return NULL;
  ast *lhs;

  switch (current->type) {
  case NUMBER:
    lhs = malloc(sizeof(ast));
    lhs->type = NODE_NUMBER;
    lhs->value = current->value;
    lhs->lhs = NULL;
    lhs->rhs = NULL;
    current = current->next;
    break;
  case IDENTIFYER:
    lhs = malloc(sizeof(ast));
    lhs->type = NODE_IDENTIFIER;
    lhs->value = current->value;
    lhs->lhs = NULL;
    lhs->rhs = NULL;
    current = current->next;
    break;
  case BRACKET_OPEN:
    current = current->next;
    lhs = parse_expression(0);
    if (current->type != BRACKET_CLOSE) {
      print("expected )");
      return NULL;
    }
    current = current->next;
    break;
  default:
    printf("bad token type in prefix\n");
    return NULL;
  }

  while (true) {
    if (!current) return lhs;
    enum token_type op;
    switch (current->type) {
    case SEMICOLON:
    case BRACKET_CLOSE:
      return lhs;
    case ADD:
    case DIV:
    case MULT:
    case MINUS:
    case AND:
    case EQUAL:
    case NOT_EQUAL:
    case OR:
      op = current->type;
      break;
    default:
      return lhs;
    }

    binding_power power = get_binding_power(op);
    if (power.left < min_bp) {
      return lhs;
    }

    current = current->next;
    ast *rhs = parse_expression(power.right);
    ast *new_lhs = malloc(sizeof(ast));
    new_lhs->type = token_type_to_ast_type(op);
    new_lhs->lhs = lhs;
    new_lhs->rhs = rhs;
    new_lhs->value = token_type_to_string(op);
    lhs = new_lhs;
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
  struct ast *val = malloc(sizeof(struct ast));

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
