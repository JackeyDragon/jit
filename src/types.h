#ifndef TYPES_H
#define TYPES_H

#define ERROR_VALUE (value){.type = ERROR, .size = -1, .value.i = -1}
typedef enum build_in_types { INT, FLOAT, ARRAY, ERROR } build_in_types;
typedef struct value {
  build_in_types type;
  int size;
  union {
    int i;
    float f;
    void *array;
  } value;
} value;

value *valuedup(value *value);
void overwrite_value(value src, value *target);
#endif // !TYPES_H
