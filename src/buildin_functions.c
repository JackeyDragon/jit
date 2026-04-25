#include "symbol_table.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char *saves[] = {"/tmp/data.bin"};

value builtin_print(value **args, int arg_count) {
  for (int i = 0; i < arg_count; i++) {
    if (i > 0)
      printf(" ");
    value *v = args[i];
    if (v->array) {
      value *elems = (value *)v->value.data;
      printf("[");
      for (int j = 0; j < v->size; j++) {
        if (j > 0)
          printf(", ");
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

value build_in_load_array_float(value **args, int arg_count) {
  if (arg_count != 1 || args[0]->type != INT)
    return ERROR_VALUE;

  int index = args[0]->value.i;
  int saves_count = sizeof(saves) / sizeof(saves[0]);

  if (index < 0 || index >= saves_count)
    return ERROR_VALUE;

  FILE *f = fopen(saves[index], "rb");
  if (!f)
    return ERROR_VALUE;

  fseek(f, 0, SEEK_END);
  long file_size = ftell(f);
  rewind(f);

  if (file_size <= 0 || file_size % sizeof(float) != 0) {
    fclose(f);
    return ERROR_VALUE;
  }

  int count = file_size / sizeof(float);

  float *tmp = malloc(sizeof(float) * count);
  if (!tmp) {
    fclose(f);
    return ERROR_VALUE;
  }

  if (fread(tmp, sizeof(float), count, f) != (size_t)count) {
    free(tmp);
    fclose(f);
    return ERROR_VALUE;
  }
  fclose(f);

  // ✅ Convert to value[]
  value *data = malloc(sizeof(value) * count);
  if (!data) {
    free(tmp);
    return ERROR_VALUE;
  }

  for (int i = 0; i < count; i++) {
    data[i].type = FLOAT;
    data[i].array = false;
    data[i].size = sizeof(float);
    data[i].value.f = tmp[i];
  }

  free(tmp);

  value result;
  result.type = FLOAT;
  result.array = true;
  result.size = count;
  result.value.data = data;

  return result;
}

value build_in_save_array_float(value **args, int arg_count) {
  if (arg_count != 2 || args[0]->type != FLOAT || !args[0]->array ||
      args[1]->type != INT)
    return ERROR_VALUE;

  int index = args[1]->value.i;
  int saves_count = sizeof(saves) / sizeof(saves[0]);

  if (index < 0 || index >= saves_count)
    return ERROR_VALUE;

  const char *path = saves[index];
  value *array = args[0];

  FILE *f = fopen(path, "wb");
  if (!f)
    return ERROR_VALUE;

  // array data is value[]
  value *arr = (value *)array->value.data;

  // temporary float buffer
  float *tmp = malloc(sizeof(float) * array->size);
  if (!tmp) {
    fclose(f);
    return ERROR_VALUE;
  }

  // convert value[] -> float[]
  for (int i = 0; i < array->size; i++) {
    if (arr[i].type != FLOAT) { // safety check
      free(tmp);
      fclose(f);
      return ERROR_VALUE;
    }
    tmp[i] = arr[i].value.f;
  }

  size_t written = fwrite(tmp, sizeof(float), array->size, f);

  free(tmp);
  fclose(f);

  if (written != (size_t)array->size)
    return ERROR_VALUE;

  return TRUE_VALUE;
}

void register_functions() {
  static ast void_type = {
    .type = NODE_TYPE,
    .data.TYPE.type = VOID,
    .data.TYPE.rhs = NULL
  };

  static ast int_type = {
    .type = NODE_TYPE,
    .data.TYPE.type = INT,
    .data.TYPE.rhs = NULL
  };

  static ast float_type = {
    .type = NODE_TYPE,
    .data.TYPE.type = FLOAT,
    .data.TYPE.rhs = NULL
  };

  static function print_func = {.name = "print",
                                .return_type = &void_type,
                                .parameter_count = -1,
                                .build_in = true,
                                .c_function = builtin_print};

  insert_function_struct(&print_func);

  static function exp_func = {.name = "exp",
                              .return_type = &float_type,
                              .parameter_count = 1,
                              .build_in = true,
                              .c_function = buildin_exp};

  insert_function_struct(&exp_func);

  static function len_func = {.name = "len",
                              .return_type = &int_type,
                              .parameter_count = 1,
                              .build_in = true,
                              .c_function = buildin_array_length};

  insert_function_struct(&len_func);

  static function save_array = {.name = "save_array",
                                .return_type = &void_type,
                                .parameter_count = 2,
                                .build_in = true,
                                .c_function = build_in_save_array_float};

  insert_function_struct(&save_array);

  static ast array_float_type = {
    .type = NODE_TYPE,
    .data.TYPE.type = ARRAY,
    .data.TYPE.rhs = &float_type
  };

  static function load_array = {.name = "load_array",
                                .return_type = &array_float_type,
                                .build_in = true,
                                .c_function = build_in_load_array_float};

  insert_function_struct(&load_array);
}
