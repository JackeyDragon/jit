#ifndef EMITER_H
#define EMITER_H

#include "ast.h"
#include "types.h"

// Execute a statement (adds to code, then runs)
int exec(ast *statement);

// Execute stored code (main loop)
int execute_statement(void);

// Evaluate expression to value
value eval_expression(ast *expression, int *status);

// Get current statement being executed
ast *emiter_get_current_statement(void);

// Set current statement
void emiter_set_current_statement(ast *stmt);

#endif