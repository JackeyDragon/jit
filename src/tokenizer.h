
#ifndef TOKENIZER_H
#define TOKENIZER_H

enum token_type {
  IDENTIFYER,
  TYPE_INT,
  TYPE_ARRAY,
  TYPE_FLOAT,
  KEYWORD_VAR,
  KEYWORD_IF,
  KEYWORD_GOTO,
  TOKEN_ASSIGN,
  LITERAL_INT,
  LITERAL_FLOAT,
  LITERAL_ARRAY_START,
  LITERAL_ARRAY_END,
  SEMICOLON,
  COLON,
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
