#include "symbol_table.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

value builtin_print(value **args, int arg_count) {
  for (int i = 0; i < arg_count; i++) {
    if (i > 0)
      printf(" ");
    value *v = args[i];
    if (v->type == INT) {
      printf("%d", v->value.i);
    } else if (v->type == FLOAT) {
      printf("%f", v->value.f);
    } else if (v->array) {
      printf("[array of %s, size %d]", v->type == FLOAT ? "float" : "int",
             v->size);
    }
  }
  printf("\n");
  return VOID_VALUE;
}

value buildin_add(value **args, int arg_count) {
  if (arg_count < 2) {
    return ERROR_VALUE;
  }
  int a = args[0]->value.i;
  int b = args[1]->value.i;
  return (value){
      .array = false, .size = sizeof(int), .type = INT, .value.i = a + b};
}

void register_functions() {
  static function print_func = {.name = "print",
                                .return_type = VOID,
                                .parameter_count = -1, // any ammount
                                .build_in = true,
                                .c_function = builtin_print};

  insert_function_struct(&print_func);

  static function add = {.name = "add",
                         .return_type = INT,
                         .parameter_count = 2,
                         .build_in = true,
                         .c_function = buildin_add};

  insert_function_struct(&add);
}
