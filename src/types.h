#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define ERROR_VALUE (value){.type = ERROR, .size = -1, .value.i = -1}
#define VOID_VALUE                                                             \
  (value){.type = VOID, .size = 0, .array = false, .value.i = 0}
#define TRUE_VALUE (value){.type = INT, .size = sizeof(int), .value.i = 1}
#define FALSE_VALUE (value){.type = INT, .size = sizeof(int), .value.i = 0}
#define NULL_VALUE (value){.type = NULL_VAL, .size = 0, .value.i = 0}

typedef enum build_in_types {
  INT,
  FLOAT,
  ARRAY,
  FUNCTION,
  VOID,
  ERROR,
  NULL_VAL
} build_in_types;

typedef struct value {
  build_in_types type;
  bool array;
  int size;
  union {
    int i;
    float f;
    void *data;
  } value;
} value;

value *valuedup(value *value);
void overwrite_value(value src, value *target);
void free_value(value *val);
#endif // !TYPES_H
