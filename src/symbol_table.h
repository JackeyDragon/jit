#include "types.h"

void init_table();
void reset_table();
void print_table();
value *lookup(char *name);
int insert(char *name, value *val);
int enter(char *name);
int leave();
