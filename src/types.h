#ifndef TYPES_H
#define TYPES_H

typedef enum build_in_types { INT, FLOAT, ARRAY } build_in_types;
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
#endif // !TYPES_H
