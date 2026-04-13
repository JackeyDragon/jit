#include "types.h"
#include <stdbool.h>
#include <stdlib.h>

value *valuedup(value *value) {
  struct value *tmp = malloc(sizeof(struct value));
  tmp->value = value->value;
  tmp->size = value->size;
  tmp->type = value->type;
  return tmp;
}

void overwrite_value(value src, value *target) {
  target->value = src.value;
  target->size = src.size;
  target->type = src.type;
}
