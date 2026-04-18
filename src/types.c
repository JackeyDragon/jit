#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

value *valuedup(value *value) {
  struct value *tmp = malloc(sizeof(struct value));
  tmp->value = value->value;
  tmp->size = value->size;
  tmp->type = value->type;
  tmp->array = value->array;
  if (value->array && value->size > 0 && value->value.array) {
    tmp->value.array = malloc(sizeof(value) * value->size);
    memcpy(tmp->value.array, value->value.array, sizeof(value) * value->size);
  }
  return tmp;
}

void overwrite_value(value src, value *target) {
  target->value = src.value;
  target->size = src.size;
  target->type = src.type;
}
