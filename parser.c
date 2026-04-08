#include "ast.h"
#include "symbol_table.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast *parse_statement();
ast *parse_declaration_ast();
ast *parse_assignment_ast();

struct ast *ast_head;
struct ast *ast_tail;

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
  if (!current)
    return NULL;
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
    if (!current)
      return lhs;
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
    new_lhs->value = strdup(token_type_to_string(op));
    lhs = new_lhs;
  }
}

ast *parse_statement() {
  if (!current) return NULL;
  
  ast *first = NULL;
  ast *tail = NULL;
  
  while (current && current->type != SEMICOLON) {
    ast *stmt = NULL;
    
    if (current->type == KEYWORD_VAR) {
      stmt = parse_declaration_ast();
    } else if (current->type == IDENTIFYER) {
      struct token *peek = current->next;
      if (peek && peek->type == TOKEN_ASSIGN) {
        stmt = parse_assignment_ast();
      } else {
        stmt = parse_expression(0);
      }
    } else {
      stmt = parse_expression(0);
    }
    
    if (!stmt) break;
    
    if (!first) {
      first = tail = stmt;
    } else {
      tail->next_statement = stmt;
      tail = stmt;
    }
    
    if (current && current->type == SEMICOLON) {
      current = current->next;
    }
  }
  
  return first;
}

ast *parse_declaration_ast() {
  if (!current || current->type != KEYWORD_VAR) return NULL;
  current = current->next;
  
  if (!current || current->type != IDENTIFYER) return NULL;
  ast *decl = malloc(sizeof(ast));
  decl->type = NODE_DECLAR;
  decl->value = current->value;
  current = current->next;
  
  if (!current || current->type != TOKEN_ASSIGN) return NULL;
  current = current->next;
  
  decl->rhs = parse_expression(0);
  decl->lhs = NULL;
  decl->next_statement = NULL;
  
  return decl;
}

ast *parse_assignment_ast() {
  if (!current || current->type != IDENTIFYER) return NULL;
  ast *ident = malloc(sizeof(ast));
  ident->type = NODE_IDENTIFIER;
  ident->value = current->value;
  ident->lhs = NULL;
  ident->rhs = NULL;
  current = current->next;
  
  if (!current || current->type != TOKEN_ASSIGN) return NULL;
  current = current->next;
  
  ast *assign = malloc(sizeof(ast));
  assign->type = NODE_ASSIGN;
  assign->lhs = ident;
  assign->rhs = parse_expression(0);
  assign->value = NULL;
  assign->next_statement = NULL;
  
  return assign;
}

int parse_declare() {
  struct ast *val = malloc(sizeof(struct ast));
  val->type = NODE_DECLAR;
  if (expect(KEYWORD_VAR)) {
    printf("Error: expected 'var'\n");
    return 1;
  }
  struct token *name = current;
  val->value = current->value;

  if (expect(IDENTIFYER)) {
    printf("Error: expected identifier after 'var'\n");
    return 1;
  }

  if (expect(TOKEN_ASSIGN)) {
    printf("Error: expected '=' after identifier\n");
    return 1;
  }

  val->rhs = parse_expression(0);

  if (expect(SEMICOLON)) {
    printf("Error: expected ';' at end of declaration\n");
    return 1;
  }
  ast_tail->next_statement = val;
  ast_tail = val;
  return 0;
}

int parse_assign() {
  struct ast *val = malloc(sizeof(struct ast));
  val->type = NODE_ASSIGN;
  if (expect(IDENTIFYER)) {
    printf("Error: expected identifier\n");
    return 1;
  }

  if (expect(TOKEN_ASSIGN)) {
    printf("Error: expected '=' after identifier\n");
    return 1;
  }

  val->rhs = parse_expression(0);

  if (expect(SEMICOLON)) {
    printf("Error: expected ';' at end of assignment\n");
    return 1;
  }
  ast_tail->next_statement = val;
  ast_tail = val;
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
  ast_head = malloc(sizeof(ast));
  ast_head->rhs = ast_head->lhs = ast_head->next_statement = NULL;
  ast_head->value = NULL;
  ast_head->type = NODE_ROOT;

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
