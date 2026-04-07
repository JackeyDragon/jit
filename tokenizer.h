
enum token_type { IDENTIFYER, KEYWORD_VAR, TOKEN_ASSIGN, NUMBER, SEMICOLON };

struct token {
  enum token_type type;
  char *value;
  struct token *next;
};
void print_tokens(struct token *head);
struct token *tokinize(char *string);

const char *token_type_to_string(enum token_type type);

#define print(string) printf("%s\n", string)
