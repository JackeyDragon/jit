#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

value *valuedup(value *val) {
  struct value *tmp = malloc(sizeof(struct value));
  tmp->value = val->value;
  tmp->size = val->size;
  tmp->type = val->type;
  tmp->array = val->array;
  if (val->array && val->size > 0 && val->value.data) {
    tmp->value.data = malloc(sizeof(value) * val->size);
    memcpy(tmp->value.data, val->value.data, sizeof(value) * val->size);
  }
  return tmp;
}

void overwrite_value(value src, value *target) {
  target->value = src.value;
  target->size = src.size;
  target->type = src.type;
  target->array = src.array;
  if (src.array && src.size > 0 && src.value.data) {
    target->value.data = malloc(sizeof(value) * src.size);
    memcpy(target->value.data, src.value.data, sizeof(value) * src.size);
    free(src.value.data);
  }
}

void free_value(value *val) {
  if (val->array) {
    free(val->value.data);
  }
  free(val);
  return;
}
