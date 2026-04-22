#include "ast.h"
#include "types.h"

typedef struct parameter {
  char *name;
  build_in_types type;
} parameter;

typedef value (*builtin_fn)(value **args, int arg_count);

typedef struct function {
  char *name;
  build_in_types return_type;
  ast *code_block;
  parameter *parameter;
  int parameter_count;
  bool build_in;
  builtin_fn c_function;
} function;

void init_table();
void reset_table();
void reset_labels();
void print_table();
value *lookup(char *name);
int insert(char *name, value *val);
int enter(void);
int leave(void);
int insert_function(ast *func);
function *lookup_function(char *name);
ast *lookup_lable(char *name);
int insert_lable(ast *statement);
int insert_function_struct(function *func);
