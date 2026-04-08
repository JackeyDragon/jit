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
extern ast *parse_statement();
extern void print_ast(ast *node, int depth);

extern char *(*readline_func)(char *);

int main(int argc, char *argv[]) {
  char input[4096];
  
  while (1) {
    printf("> ");
    if (!fgets(input, sizeof(input), stdin)) {
      printf("\nGoodbye!\n");
      break;
    }
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) == 0) {
      continue;
    }
    
    struct token *head = tokinize(input);
    print_tokens(head);
    current = head;
    ast *tree = parse_statement();
    printf("\nAST:\n");
    print_ast(tree, 0);
    printf("\n");
  }
  return EXIT_SUCCESS;
}
