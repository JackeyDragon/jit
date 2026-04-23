#include "symbol_table.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

value builtin_print(value **args, int arg_count) {
  for (int i = 0; i < arg_count; i++) {
    if (i > 0)
      printf(" ");
    value *v = args[i];
    if (v->array) {
      value *elems = (value*)v->value.data;
      printf("[");
      for(int j=0; j<v->size; j++) {
        if(j>0) printf(", ");
        if (v->type == FLOAT)
          printf("%f", elems[j].value.f);
        else
          printf("%d", elems[j].value.i);
      }
      printf("]");
    } else if (v->type == INT) {
      printf("%d", v->value.i);
    } else if (v->type == FLOAT) {
      printf("%f", v->value.f);
    }
  }
  printf("\n");
  return VOID_VALUE;
}

value buildin_exp(value **args, int arg_count) {
  (void)arg_count;
  float x = (args[0]->type == INT) ? (float)args[0]->value.i : args[0]->value.f;
  return (value){
      .array = false, .size = sizeof(float), .type = FLOAT, .value.f = exp(x)};
}

value buildin_array_length(value **args, int arg_count) {
  (void)arg_count;
  if (args[0]->array != true)
    return ERROR_VALUE;
  return (value){.array = false,
                 .size = sizeof(int),
                 .type = INT,
                 .value.i = args[0]->size};
}

void register_functions() {
  static function print_func = {.name = "print",
                                 .return_type = VOID,
                                 .parameter_count = -1,
                                 .build_in = true,
                                 .c_function = builtin_print};

  insert_function_struct(&print_func);

  static function exp_func = {.name = "exp",
                             .return_type = FLOAT,
                             .parameter_count = 1,
                             .build_in = true,
                             .c_function = buildin_exp};

  insert_function_struct(&exp_func);

  static function len_func = {.name = "len",
                              .return_type = INT,
                              .parameter_count = 1,
                              .build_in = true,
                              .c_function = buildin_array_length};

  insert_function_struct(&len_func);
}