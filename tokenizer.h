
#ifndef TOKENIZER_H
#define TOKENIZER_H

enum token_type {
  IDENTIFYER,
  KEYWORD_VAR,
  KEYWORD_IF,
  TOKEN_ASSIGN,
  NUMBER,
  SEMICOLON,
  ADD,
  MINUS,
  MULT,
  DIV,
  AND,
  OR,
  EQUAL,
  NOT_EQUAL,
  BRACKET_OPEN,
  BRACKET_CLOSE,
  CURLY_BRACKET_OPEN,
  CURLY_BRACKET_CLOSE,
  UNKNOWN
};

struct token {
  enum token_type type;
  char *value;
  struct token *next;
};
void print_tokens(struct token *head);
struct token *tokinize(char *string);

const char *token_type_to_string(enum token_type type);

#endif

#define print(string) printf("%s\n", string)
