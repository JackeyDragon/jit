#include "tokenizer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *token_type_to_string(enum token_type type) {
  switch (type) {
  case IDENTIFYER:
    return "IDENTIFIER";
  case KEYWORD_GOTO:
    return "KEYWORD_GOTO";
  case KEYWORD_VAR:
    return "KEYWORD_VAR";
  case KEYWORD_IF:
    return "KEYWORD_IF";
  case TOKEN_ASSIGN:
    return "TOKEN_ASSIGN";
  case LITERAL_INT:
    return "LITERAL_INT";
  case LITERAL_FLOAT:
    return "LITERAL_FLOAT";
  case TYPE_INT:
    return "TYPE_INT";
  case TYPE_FLOAT:
    return "TYPE_FLOAT";
  case SEMICOLON:
    return "SEMICOLON";
  case ADD:
    return "ADD";
  case MINUS:
    return "MINUS";
  case MULT:
    return "MULT";
  case DIV:
    return "DIV";
  case AND:
    return "AND";
  case OR:
    return "OR";
  case EQUAL:
    return "EQUAL";
  case NOT_EQUAL:
    return "NOT_EQUAL";
  case BRACKET_OPEN:
    return "BRACKET_OPEN";
  case BRACKET_CLOSE:
    return "BRACKET_CLOSE";
  case CURLY_BRACKET_OPEN:
    return "CURLY_BRACKET_OPEN";
  case CURLY_BRACKET_CLOSE:
    return "CURLY_BRACKET_CLOSE";
  default:
    return "UNKNOWN";
  }
}

void print_tokens(struct token *head) {
  struct token *current = head;

  while (current != NULL) {
    printf("[%s](%s) ", token_type_to_string(current->type),
           current->value ? current->value : "NULL");

    current = current->next;
  }
}

void is_keyword(struct token *token) {
  char *value = token->value;
  if (strcmp(value, "float") == 0) {
    token->type = TYPE_FLOAT;
  } else if (strcmp(value, "int") == 0) {
    token->type = TYPE_INT;
  } else if (strcmp(value, "if") == 0) {
    token->type = KEYWORD_IF;
  } else if (strcmp(value, "goto") == 0) {
    token->type = KEYWORD_GOTO;
  } else {
    token->type = IDENTIFYER;
  }
  return;
}

int read_identifyer(char *string, struct token *token) {
  int pos = 0;
  while (isalpha(string[pos])) {
    pos++;
  }
  token->value = malloc(pos + 1);
  strncpy(token->value, string, pos);
  token->value[pos] = '\0';
  return pos;
}

char *is_operator(char *string, struct token *token, int *pos) {
  if (strncmp(string, "==", 2) == 0)
    token->type = EQUAL;
  else if (strncmp(string, "!=", 1) == 0)
    token->type = NOT_EQUAL;
  else if (strncmp(string, "||", 1) == 0)
    token->type = OR;
  else if (strncmp(string, "&&", 1) == 0)
    token->type = AND;
  // else if (strncmp(string, "!", 1) == 0)
  //   token->type
  else if (strncmp(string, "=", 1) == 0)
    token->type = TOKEN_ASSIGN;
  else if (strncmp(string, "+", 1) == 0)
    token->type = ADD;
  else if (strncmp(string, "-", 1) == 0)
    token->type = MINUS;
  else if (strncmp(string, "*", 1) == 0)
    token->type = MULT;
  else if (strncmp(string, "/", 1) == 0)
    token->type = DIV;
  else
    token->type = UNKNOWN;

  switch (token->type) {
  case EQUAL:
    token->value = strdup("==");
    break;
  case NOT_EQUAL:
    token->value = strdup("!=");
    break;
  case OR:
    token->value = strdup("||");
    break;
  case AND:
    token->value = strdup("&&");
    break;
  case TOKEN_ASSIGN:
    token->value = strdup("=");
    break;
  case ADD:
    token->value = strdup("+");
    break;
  case MINUS:
    token->value = strdup("-");
    break;
  case MULT:
    token->value = strdup("*");
    break;
  case DIV:
    token->value = strdup("/");
    break;
  default:
    return NULL;
  }

  (*pos) += strlen(token->value);

  return token->value;
}

int read_digit(char *string, struct token *token) {
  // TODO test
  int count = 0;
  while (isdigit(string[count])) {
    count++;
  }
  if (string[count] != '.') {
    char *val = malloc(count + 1);
    strncpy(val, string, count);
    val[count] = '\0';

    token->value = val;
    token->type = LITERAL_INT;
    return count;
  }
  count++;
  while (isdigit(string[count])) {
    count++;
  }
  char *val = malloc(count + 1);
  strncpy(val, string, count);
  val[count] = '\0';

  token->value = val;
  token->type = LITERAL_FLOAT;
  return count;
}

struct token *tokinize(char *string) {
  struct token *token = malloc(sizeof(struct token));
  // remove leading spaces
  while (strncmp(string, " ", 1) == 0 || strncmp(string, "\t", 1) == 0) {
    string++;
  }

  int pos = 0;
  while (string[pos] != '\0') {
    if (isspace(string[pos])) {
      pos++;
      continue;

    } else if (isalpha(string[pos])) {

      pos += read_identifyer(string + pos, token);
      is_keyword(token);
      break;

    } else if (is_operator(string + pos, token, &pos) != NULL) {
      break;

    } else if (isdigit(string[pos])) {

      pos += read_digit(string + pos, token);
      break;
    } else if (strncmp(string + pos, ";", 1) == 0) {

      token->type = SEMICOLON;
      token->value = ";";
      pos++;

      break;
    } else if (strncmp(string + pos, "(", 1) == 0) {
      token->type = BRACKET_OPEN;
      token->value = strdup("(");
      pos++;
      break;
    } else if (strncmp(string + pos, ")", 1) == 0) {
      token->type = BRACKET_CLOSE;
      token->value = strdup(")");
      pos++;
      break;
    } else if (strncmp(string + pos, "{", 1) == 0) {
      token->type = CURLY_BRACKET_OPEN;
      token->value = strdup("{");
      pos++;
      break;
    } else if (strncmp(string + pos, "}", 1) == 0) {
      token->type = CURLY_BRACKET_CLOSE;
      token->value = strdup("}");
      pos++;
      break;
    } else {
      exit(1);
    }
  }

  if (string[pos] == '\0') {
    token->next = NULL;
  } else {
    token->next = tokinize(string + pos);
  }
  return token;
}
