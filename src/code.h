#ifndef CODE_H
#define CODE_H

#include "ast.h"

int code_init(void);
int code_add(ast *statement);
ast *code_get_line(int line_number);
int code_get_current_line(void);
int set_current_line(int line);

char *code_print_expr(ast *node);
char *code_print_statement(ast *node);
void code_print_all(void);
void code_cleanup(void);

#endif