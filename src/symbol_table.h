
struct entry {
  char *name;
  int val;
  struct entry *next;
};
void print_table();
int insert(char *name, int val);
struct entry *get_entry_by_name(char *name);
int set_entry_val(char *name, int value);
int lookup(char *name);
void reset_table();
