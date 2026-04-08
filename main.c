#include "ast.h"
#include "tokenizer.h"
#include "symbol_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token;
extern struct token *current;
extern ast *parse_expression(int min_bp);
extern void print_ast(ast *node, int depth);

int main(int argc, char *argv[]) {
  char *input = "1 + 2 * 3";
  printf("Input: %s\n\n", input);
  struct token *head = tokinize(input);
  print_tokens(head);
  current = head;
  ast *tree = parse_expression(0);
  printf("\nAST:\n");
  print_ast(tree, 0);
  return EXIT_SUCCESS;
}
