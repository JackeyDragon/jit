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
ast *parse_if();

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
  printf("DEBUG parse_expression: called, min_bp=%d, current=%s\n", min_bp,
         current ? current->value : "NULL");
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
    lhs->type = NODE_REFERENCE;
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
  if (!current)
    return NULL;

  ast *first = NULL;
  ast *tail = NULL;

  while (current) {
    ast *stmt = NULL;

    if (current->type == KEYWORD_VAR) {
      stmt = parse_declaration_ast();
    } else if (current->type == IDENTIFYER) {
      struct token *peek = current->next;
      if (peek && peek->type == TOKEN_ASSIGN) {
        stmt = parse_assignment_ast();
      } else {
        // can this go?
        stmt = parse_expression(0);
      }
    } else if (current->type == KEYWORD_IF) {
      stmt = parse_if();
    }

    if (!stmt)
      break;

    int requires_semicolon = 1;
    if (stmt->type == NODE_IF_CONDITION)
      requires_semicolon = 0;

    if (requires_semicolon && (current == NULL || current->type != SEMICOLON)) {
      print("missing semicolon");
      return NULL;
    } else if (requires_semicolon) {
      current = current->next;
    }

    if (!first) {
      first = tail = stmt;
    } else {
      tail->next_statement = stmt;
      tail = stmt;
    }
  }

  return first;
}

ast *parse_declaration_ast() {
  if (!current || current->type != KEYWORD_VAR)
    return NULL;
  current = current->next;

  if (!current || current->type != IDENTIFYER)
    return NULL;
  ast *decl = malloc(sizeof(ast));
  decl->type = NODE_DECLAR;
  decl->value = current->value;
  current = current->next;

  if (!current || current->type != TOKEN_ASSIGN)
    return NULL;
  current = current->next;

  decl->rhs = parse_expression(0);
  decl->lhs = NULL;
  decl->next_statement = NULL;

  return decl;
}

ast *parse_if() {
  printf("DEBUG parse_if: called, current token: %s\n",
         current ? current->value : "NULL");
  if (!current || current->type != KEYWORD_IF)
    return NULL;

  printf("DEBUG parse_if: after consuming IF, current token: %s\n",
         current ? current->value : "NULL");
  ast *stmt_if = malloc(sizeof(ast));
  stmt_if->type = NODE_IF_CONDITION;
  stmt_if->value = "if";
  printf("DEBUG parse_if: set NODE_IF_CONDITION, value=if\n");

  current = current->next;
  if (!current || current->type != BRACKET_OPEN)
    return NULL;

  stmt_if->rhs = parse_expression(0);
  stmt_if->lhs = NULL;
  stmt_if->next_statement = parse_statement();

  return stmt_if;
}

ast *parse_assignment_ast() {
  if (!current || current->type != IDENTIFYER)
    return NULL;
  ast *ident = malloc(sizeof(ast));
  ident->type = NODE_REFERENCE;
  ident->value = current->value;
  ident->lhs = NULL;
  ident->rhs = NULL;
  current = current->next;

  if (!current || current->type != TOKEN_ASSIGN)
    return NULL;
  current = current->next;

  ast *assign = malloc(sizeof(ast));
  assign->type = NODE_ASSIGN;
  assign->lhs = ident;
  assign->rhs = parse_expression(0);
  assign->value = NULL;
  assign->next_statement = NULL;

  return assign;
}
