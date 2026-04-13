#ifndef CODE_H
#define CODE_H

#include "ast.h"

// Initialize code storage
int code_init(void);

// Add a statement to code storage
int code_add(ast *statement);

// Get code line by number
ast *code_get_line(int line_number);

// Get current line number
int code_get_current_line(void);

// Print all stored code (debug)
void code_print_all(void);

// Expression to string (debug)
char *code_print_expr(ast *node);

// Statement to string (debug)
char *code_print_statement(ast *node);

// set current code line for goto (or jumps in general)
int set_current_line(int line);
#endif
