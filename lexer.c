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
  case KEYWORD_VAR:
    return "KEYWORD_VAR";
  case TOKEN_ASSIGN:
    return "TOKEN_ASSIGN";
  case NUMBER:
    return "NUMBER";
  case SEMICOLON:
    return "SEMICOLON";
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
  if (strcmp(value, "var") == 0) {
    token->type = KEYWORD_VAR;
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

char *is_operator(char *string) {
  if (strncmp(string, "=", 1) == 0)
    return "=";
  return NULL;
}

int read_digit(char *string, struct token *token) {
  int count = 0;
  while (isdigit(string[count])) {
    count++;
  }
  char *val = malloc(count + 1);
  strncpy(val, string, count);
  val[count] = '\0';

  token->value = val;
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

    } else if (is_operator(string + pos) != NULL) {

      char *operator = is_operator(string + pos);
      token->type = TOKEN_ASSIGN;
      token->value = operator;
      pos += strlen(operator);
      break;

    } else if (isdigit(string[pos])) {

      pos += read_digit(string + pos, token);
      token->type = NUMBER;
      break;
    } else if (strncmp(string + pos, ";", 1) == 0) {

      token->type = SEMICOLON;
      token->value = ";";
      pos++;

      break;
    }
  }

  if (string[pos] == '\0') {
    token->next = NULL;
  } else {
    token->next = tokinize(string + pos);
  }
  return token;
}
