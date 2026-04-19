#include "symbol_table.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

value builtin_print(value **args, int arg_count) {
  printf("hello world");
  return VOID_VALUE;
}

value buildin_add(value **args, int arg_count) {
  return (value){
      .array = false, .size = sizeof(int), .type = INT, .value.i = 3};
}

void register_functions() {
  function print_func = {.name = "print",
                         .return_type = VOID,
                         .parameter_count = 1,
                         .build_in = true,
                         .c_function = builtin_print};

  insert_function_struct(&print_func);

  function add = {.name = "add",
                  .return_type = INT,
                  .parameter_count = 0,
                  .build_in = true,
                  .c_function = buildin_add};

  insert_function_struct(&add);
}
