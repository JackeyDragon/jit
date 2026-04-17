#include "ast.h"
#include "tokenizer.h"

struct token;
extern struct token *current;

typedef struct ast ast;
extern ast *parse_expression(int min_bp);
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast_type token_type_to_ast_type(enum token_type type) {
  switch (type) {
  case IDENTIFYER:
    return NODE_IDENTIFYER;
  case LITERAL_INT:
    return NODE_INT;
  case LITERAL_FLOAT:
    return NODE_FLOAT;
  case LITERAL_ARRAY_START:
    return NODE_ARRAY;
  case KEYWORD_IF:
  case KEYWORD_VAR:
    return NODE_KEYWORD;
  case ADD:
  case MINUS:
  case MULT:
  case DIV:
  case AND:
  case OR:
  case EQUAL:
  case NOT_EQUAL:
    return NODE_OPERATION;
  case TOKEN_ASSIGN:
    return NODE_EQUALS_SIGN;
  case BRACKET_OPEN:
    return NODE_ARRAY_ACCESS;
  case BRACKET_CLOSE:
    return NODE_BRACKET_CLOSE;
  case CURLY_BRACKET_OPEN:
    return NODE_BLOCK;
  case CURLY_BRACKET_CLOSE:
    return NODE_BRACKET_CLOSE;
  default:
    return -1;
  }
}

ast *clone_ast(ast *source) {
  if (!source)
    return NULL;
  ast *ast_new = malloc(sizeof(struct ast));
  ast_new->type = source->type;

  switch (source->type) {
  case NODE_INT:
  case NODE_FLOAT:
    ast_new->data.LITTERAL = source->data.LITTERAL;
    break;
  case NODE_IDENTIFYER:
    ast_new->data.IDENTIFYER.name = source->data.IDENTIFYER.name
                                        ? strdup(source->data.IDENTIFYER.name)
                                        : NULL;
    break;
  case NODE_OPERATION:
  case NODE_EXPRESION:
    ast_new->data.EXPRESSION.lhs = clone_ast(source->data.EXPRESSION.lhs);
    ast_new->data.EXPRESSION.rhs = clone_ast(source->data.EXPRESSION.rhs);
    ast_new->data.EXPRESSION.operaton = source->data.EXPRESSION.operaton;
    break;
  case NODE_DECLAR:
    ast_new->data.DECLARE.identifyer =
        clone_ast(source->data.DECLARE.identifyer);
    ast_new->data.DECLARE.type = source->data.DECLARE.type;
    ast_new->data.DECLARE.expression =
        clone_ast(source->data.DECLARE.expression);
    ast_new->next_statement = clone_ast(source->next_statement);
    break;
  case NODE_ASSIGN:
    ast_new->data.ASSIGN.identifyer = clone_ast(source->data.ASSIGN.identifyer);
    ast_new->data.ASSIGN.expression = clone_ast(source->data.ASSIGN.expression);
    ast_new->next_statement = clone_ast(source->next_statement);
    break;
  case NODE_IF_CONDITION:
    ast_new->data.IF.condition = clone_ast(source->data.IF.condition);
    ast_new->data.IF.if_body = clone_ast(source->data.IF.if_body);
    ast_new->data.IF.else_body = clone_ast(source->data.IF.else_body);
    ast_new->next_statement = clone_ast(source->next_statement);
    break;
  case NODE_GOTO:
    ast_new->data.GOTO.identifyer = clone_ast(source->data.GOTO.identifyer);
    ast_new->next_statement = clone_ast(source->next_statement);
    break;
  case NODE_LABLE:
    ast_new->data.LABLE.identifyer = clone_ast(source->data.LABLE.identifyer);
    ast_new->next_statement = clone_ast(source->next_statement);
    break;
  case NODE_ARRAY:
  case NODE_ARRAY_ACCESS:
  case NODE_BLOCK:
    if (source->data.BLOCK.array && source->data.BLOCK.count > 0) {
      ast_new->data.BLOCK.count = source->data.BLOCK.count;
      ast_new->data.BLOCK.array =
          malloc(sizeof(ast *) * source->data.BLOCK.count);
      for (int i = 0; i < source->data.BLOCK.count; i++) {
        ast_new->data.BLOCK.array[i] = clone_ast(source->data.BLOCK.array[i]);
      }
    }
    break;
  case NODE_ROOT:
  case NODE_KEYWORD:
  case NODE_BRACKET_OPEN:
  case NODE_BRACKET_CLOSE:
    break;
  }

  return ast_new;
}

binding_power get_binding_power(enum token_type type) {
  switch (type) {
  case EQUAL:
  case NOT_EQUAL:
    return (binding_power){.left = 1, .right = 0.9};
  case AND:
  case OR:
    return (binding_power){.left = 1, .right = 0.9};
  case ADD:
  case MINUS:
    return (binding_power){.left = 2, .right = 2.1};
  case MULT:
  case DIV:
    return (binding_power){.left = 3, .right = 3.1};
  default:
    return (binding_power){.left = -1, .right = -1};
  }
}

void print_ast(ast *node, int depth) {
  if (!node)
    return;
  for (int i = 0; i < depth; i++)
    printf("  ");
  switch (node->type) {
  case NODE_DECLAR:
    printf("DECLAR: %s\n", node->data.IDENTIFYER.name);
    break;
  case NODE_ASSIGN:
    printf("ASSIGN: %s\n", node->data.ASSIGN.identifyer->data.IDENTIFYER.name);
    break;
  case NODE_INT:
    printf("INT: %d\n", node->data.LITTERAL.value.value.i);
    break;
  case NODE_FLOAT:
    printf("FLOAT: %f\n", node->data.LITTERAL.value.value.f);
    break;
  case NODE_ARRAY:
    printf("ARRAY (%d elements)\n", node->data.BLOCK.count);
    break;
  case NODE_BLOCK:
    printf("BLOCK (%d statements)\n", node->data.BLOCK.count);
    break;
  case NODE_IDENTIFYER:
    printf("IDENTIFYER: %s\n", node->data.IDENTIFYER.name);
    break;
  case NODE_ARRAY_ACCESS:
    printf("ARRAY_ACCESS: %s\n", node->data.IDENTIFYER.name);
    break;
  case NODE_OPERATION:
    printf("OPERATION: %d\n", node->data.EXPRESSION.operaton);
    break;
  case NODE_KEYWORD:
    printf("KEYWORD\n");
    break;
  case NODE_ROOT:
    printf("ROOT\n");
    break;
  case NODE_TYPE:
    printf("TYPE: %s\n", node->data.TYPE.type == INT ? "int" : "float");
    break;
  case NODE_IF_CONDITION:
    printf("IF_CONDITION\n");
    break;
  case NODE_FUNCTION_DECLARATION:
    printf("FUNCTION_DECLARATION: %s\n",
           node->data.FUNCTION_DECLARATION.identifyer->data.IDENTIFYER.name);
    break;
  case NODE_PARAMETER_DECLARATION:
    printf("PARAMETER_DECLARATION: %s\n",
           node->data.PARAMETER_DECLARATION.name->data.IDENTIFYER.name);
    break;
  default:
    printf("UNKNOWN (type=%d)\n", node->type);
  }
  if (node->type == NODE_OPERATION || node->type == NODE_EXPRESION) {
    print_ast(node->data.EXPRESSION.lhs, depth + 1);
    print_ast(node->data.EXPRESSION.rhs, depth + 1);
  }
  if (node->type == NODE_BLOCK) {
    for (int i = 0; i < node->data.BLOCK.count; i++) {
      print_ast(node->data.BLOCK.array[i], depth + 1);
    }
  }
  if (node->type == NODE_FUNCTION_DECLARATION) {
    if (node->data.FUNCTION_DECLARATION.parameter) {
      print_ast(node->data.FUNCTION_DECLARATION.parameter, depth + 1);
    }
    if (node->data.FUNCTION_DECLARATION.block) {
      print_ast(node->data.FUNCTION_DECLARATION.block, depth + 1);
    }
  }
  if (node->type == NODE_PARAMETER_DECLARATION) {
    print_ast(node->data.PARAMETER_DECLARATION.type, depth + 1);
    if (node->data.PARAMETER_DECLARATION.next_param) {
      print_ast(node->data.PARAMETER_DECLARATION.next_param, depth + 1);
    }
  }
}
