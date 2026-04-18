#include "ast.h"
#include "symbol_table.h"
#include "tokenizer.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast *parse_statement();
ast *parse_declaration_ast();
ast *parse_assignment_ast();
ast *parse_if();
ast *parse_function_declaration();
ast *parse_block();
ast *parse_function_call();
ast *parse_function_call_arguments();
ast *parse_identifyer();
ast *parse_lable();
ast *parse_return();

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
  case LITERAL_FLOAT: {
    lhs = malloc(sizeof(ast));
    lhs->type = NODE_FLOAT;
    lhs->data.LITTERAL.value.type = FLOAT;
    lhs->data.LITTERAL.value.value.f = atof(current->value);
    current = current->next;
    break;
  }
  case LITERAL_INT: {
    lhs = malloc(sizeof(ast));
    lhs->type = NODE_INT;
    lhs->data.LITTERAL.value.type = INT;
    lhs->data.LITTERAL.value.value.i = atoi(current->value);
    current = current->next;
  } break;
  case IDENTIFYER:
    if (current->next && current->next->type == BRACKET_OPEN) {
      char *fn_name = current->value;
      current = current->next; // consume function name
      current = current->next; // consume (
      ast *args = parse_function_call_arguments();
      if (!current || current->type != BRACKET_CLOSE) {
        print("expected )");
        return NULL;
      }
      current = current->next;

      lhs = malloc(sizeof(ast));
      lhs->type = NODE_FUNCTION_CALL;
      lhs->data.FUNCTION_CALL.name = fn_name;
      lhs->data.FUNCTION_CALL.params = args;
    } else {
      lhs = malloc(sizeof(ast));
      lhs->type = NODE_IDENTIFYER;
      lhs->data.IDENTIFYER.name = current->value;
      current = current->next;
    }
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
    case COMMA:
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
    new_lhs->data.EXPRESSION.lhs = lhs;
    new_lhs->data.EXPRESSION.rhs = rhs;
    new_lhs->data.EXPRESSION.operaton = op;
    lhs = new_lhs;
  }
}

ast *parse_goto() {
  ast *ast_goto = malloc(sizeof(struct ast));
  if (!current || current->type != KEYWORD_GOTO)
    return NULL;
  ast_goto->type = NODE_GOTO;

  current = current->next;

  ast_goto->data.GOTO.identifyer = parse_identifyer();
  ast_goto->next_statement = NULL;

  if (current == NULL || current->type != SEMICOLON) {
    print("missing semicolon");
    return NULL;
  }

  current = current->next;

  return ast_goto;
}

ast *parse_statement() {
  if (!current)
    return NULL;

  ast *first = NULL;
  ast *tail = NULL;

  while (current && current->type != CURLY_BRACKET_CLOSE) {
    ast *stmt = NULL;

    if (current->type == TYPE_INT || current->type == TYPE_FLOAT) {
      struct token *peek = current->next->next;
      if (peek->type == TOKEN_ASSIGN) {
        stmt = parse_declaration_ast();
      } else if (peek->type == BRACKET_OPEN) {
        stmt = parse_function_declaration();
      }
    } else if (current->type == IDENTIFYER) {
      struct token *peek = current->next;
      if (peek && peek->type == TOKEN_ASSIGN) {
        stmt = parse_assignment_ast();
      } else if (peek && peek->type == BRACKET_OPEN) {
        stmt = parse_function_call();
      } else if (peek && peek->type == COLON) {
        stmt = parse_lable();
      } else {
        // can this go?
        stmt = parse_expression(0);
      }
    } else if (current->type == KEYWORD_IF) {
      stmt = parse_if();
    } else if (current->type == KEYWORD_GOTO) {
      stmt = parse_goto();
    } else if (current->type == CURLY_BRACKET_OPEN) {
      stmt = parse_block();
    } else if (current->type == KEYWORD_RETURN) {
      stmt = parse_return();
    }

    if (!stmt)
      break;

    if (!first) {
      first = tail = stmt;
    } else {
      tail->next_statement = stmt;
      tail = stmt;
    }
  }

  return first;
}

ast *parse_type() {
  if (!current)
    return NULL;

  ast *type = malloc(sizeof(ast));
  type->type = NODE_TYPE;
  switch (current->type) {
  case TYPE_FLOAT:
    type->data.TYPE.type = FLOAT;
    type->data.TYPE.rhs = NULL;
    current = current->next;
    break;
  case TYPE_INT:
    type->data.TYPE.type = INT;
    type->data.TYPE.rhs = NULL;
    current = current->next;
    break;
  default:
    free(type);
    return NULL;
  }
  return type;
}

ast *parse_identifyer() {
  if (!current || current->type != IDENTIFYER)
    return NULL;
  ast *identifyer = malloc(sizeof(ast));

  identifyer->type = NODE_IDENTIFYER;
  identifyer->data.IDENTIFYER.name = current->value;

  current = current->next;
  return identifyer;
}

ast *parse_declaration_ast() {
  ast *type = parse_type();
  if (!type)
    return NULL;

  ast *name = parse_identifyer();
  if (!name) {
    free(type);
    return NULL;
  }

  ast *decl = malloc(sizeof(ast));
  decl->type = NODE_DECLAR;
  decl->data.DECLARE.type = type->data.TYPE.type;
  decl->data.DECLARE.identifyer = name;
  decl->data.DECLARE.array = false;
  if (current && current->type == SQUARE_BRACKET_OPEN) {
    decl->data.DECLARE.array = true;
    decl->data.DECLARE.size = parse_expression(0);
  }

  if (!current || current->type != TOKEN_ASSIGN ||
      (decl->data.DECLARE.array && !decl->data.DECLARE.size) ||
      (decl->data.DECLARE.array && expect(SQUARE_BRACKET_CLOSE))) {
    free(name);
    free(decl);
    free(type);
    return NULL;
  }
  current = current->next;

  decl->data.DECLARE.expression = parse_expression(0);
  decl->next_statement = NULL;

  if (current == NULL || current->type != SEMICOLON) {
    free(decl);
    free(name);
    free(type);
    return NULL;
  }
  current = current->next;

  free(type);
  return decl;
}

ast *parse_if() {
  if (!current || current->type != KEYWORD_IF)
    return NULL;

  ast *stmt_if = malloc(sizeof(ast));
  stmt_if->type = NODE_IF_CONDITION;

  current = current->next;
  if (!current || current->type != BRACKET_OPEN)
    return NULL;

  stmt_if->data.IF.condition = parse_expression(0);
  stmt_if->data.IF.if_body = NULL;
  stmt_if->next_statement = parse_statement();
  stmt_if->data.IF.else_body = NULL;

  return stmt_if;
}

ast *parse_assignment_ast() {
  ast *name = parse_identifyer();
  if (!current || !name)
    return NULL;

  if (!current || current->type != TOKEN_ASSIGN)
    return NULL;
  current = current->next;

  ast *assign = malloc(sizeof(ast));
  assign->type = NODE_ASSIGN;
  assign->data.ASSIGN.identifyer = name;
  assign->data.ASSIGN.expression = parse_expression(0);
  assign->next_statement = NULL;

  if (current == NULL || current->type != SEMICOLON) {
    print("missing semicolon");
    return NULL;
  }
  current = current->next;
  return assign;
}

ast *parse_parameter_declaration() {
  ast *type = parse_type();
  ast *name = parse_identifyer();

  if (!name || !type) {
    free(type);
    free(name);
    return NULL;
  }

  ast *param = malloc(sizeof(ast));
  param->type = NODE_PARAMETER_DECLARATION;
  param->data.PARAMETER_DECLARATION.type = type;
  param->data.PARAMETER_DECLARATION.name = name;
  param->data.PARAMETER_DECLARATION.next_param = NULL;

  ast *tail = param;

  while (current && current->type == COMMA) {
    current = current->next;

    type = parse_type();
    name = parse_identifyer();

    if (!name || !type) {
      free(type);
      free(name);
      ast *tmp = param;
      while (tmp) {
        ast *next = tmp->data.PARAMETER_DECLARATION.next_param;
        free(tmp);
        tmp = next;
      }
      return NULL;
    }

    ast *next_param = malloc(sizeof(ast));
    next_param->type = NODE_PARAMETER_DECLARATION;
    next_param->data.PARAMETER_DECLARATION.type = type;
    next_param->data.PARAMETER_DECLARATION.name = name;
    next_param->data.PARAMETER_DECLARATION.next_param = NULL;

    tail->data.PARAMETER_DECLARATION.next_param = next_param;
    tail = next_param;
  }

  return param;
}

ast *parse_function_declaration() {
  ast *type = parse_type();
  if (!type)
    return NULL;

  ast *name = parse_identifyer();
  if (!name) {
    free(type);
    return NULL;
  }
  if (expect(BRACKET_OPEN)) {
    free(type);
    free(name);
    return NULL;
  }

  ast *parameters;
  if (!current || current->type == BRACKET_CLOSE) {
    parameters = NULL;
    current = current->next;
  } else {
    parameters = parse_parameter_declaration();
    if (!parameters || expect(BRACKET_CLOSE)) {
      free(type);
      free(name);
      return NULL;
    }
  }

  ast *block = parse_block();
  if (!block) {
    free(parameters);
    free(name);
    return NULL;
  }

  ast *func = malloc(sizeof(ast));
  func->type = NODE_FUNCTION_DECLARATION;
  func->data.FUNCTION_DECLARATION.return_type = type;
  func->data.FUNCTION_DECLARATION.identifyer = name;
  func->data.FUNCTION_DECLARATION.parameter = parameters;
  func->data.FUNCTION_DECLARATION.block = block;

  return func;
}

ast *parse_block() {
  if (expect(CURLY_BRACKET_OPEN)) {
    return NULL;
  }

  if (!current || current->type == CURLY_BRACKET_CLOSE)
    return NULL;

  ast *node = parse_statement();

  if (!node) {
    return NULL;
  }

  ast *tmp = node;
  int count = 0;
  while (tmp) {
    count++;
    tmp = tmp->next_statement;
  }

  if (expect(CURLY_BRACKET_CLOSE)) {
    return NULL;
  }

  ast *block = malloc(sizeof(ast));
  block->type = NODE_BLOCK;
  block->data.BLOCK.count = count;
  block->data.BLOCK.array = malloc(sizeof(ast *) * (count > 0 ? count : 1));
  ast **array = (ast **)block->data.BLOCK.array;

  for (int i = 0; i < count; i++) {
    array[i] = node;
    node = node->next_statement;
  }
  return block;
}

ast *parse_function_call_arguments() {
  if (!current || current->type == BRACKET_CLOSE)
    return NULL;

  ast *first = NULL;
  ast *tail = NULL;

  ast *expr = parse_expression(0);
  if (!expr)
    return NULL;

  first = tail = malloc(sizeof(ast));
  first->type = NODE_PARAMETER;
  first->data.PARAMETER.expression = expr;
  first->data.PARAMETER.next_param = NULL;
  first->next_statement = NULL;

  while (current && current->type == COMMA) {
    current = current->next;
    expr = parse_expression(0);
    if (!expr)
      return NULL;
    tail->data.PARAMETER.next_param = malloc(sizeof(ast));
    tail = tail->data.PARAMETER.next_param;
    tail->type = NODE_PARAMETER;
    tail->next_statement = NULL;
    tail->data.PARAMETER.next_param = NULL;
    tail->data.PARAMETER.expression = expr;
  }

  return first;
}

ast *parse_function_call() {
  ast *name = parse_identifyer();
  if (!name || expect(BRACKET_OPEN)) {
    if (name)
      free(name);
    return NULL;
  }

  ast *args = parse_function_call_arguments();
  if (!args && (!current || current->type != BRACKET_CLOSE)) {
    free(name);
    return NULL;
  }
  if (current && current->type == BRACKET_CLOSE)
    current = current->next;

  ast *call = malloc(sizeof(ast));
  call->type = NODE_FUNCTION_CALL;
  call->data.FUNCTION_CALL.name = name->data.IDENTIFYER.name;
  call->data.FUNCTION_CALL.params = args;
  free(name);
  return call;
}

ast *parse_return() {
  if (expect(KEYWORD_RETURN))
    return NULL;

  ast *expression = parse_expression(0);
  if (!expression)
    return NULL;
  ast *node_return = malloc(sizeof(ast));
  node_return->type = NODE_RETURN;
  node_return->data.RETURN.expression = expression;

  if (current == NULL || current->type != SEMICOLON) {
    print("missing semicolon");
    return NULL;
  }
  current = current->next;

  return node_return;
}

ast *parse_lable() {
  ast *name = parse_identifyer();
  if (expect(COLON)) {
    free(name);
    return NULL;
  }
  ast *lable = malloc(sizeof(ast));
  lable->type = NODE_LABLE;
  lable->data.LABLE.identifyer = name;
  return lable;
}
