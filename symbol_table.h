
struct entry {
  char *name;
  int val;
  struct entry *next;
};

int insert(char *name, int val);
struct entry *get_entry_by_name(char *name);
int set_entry_val(char *name, int value);
