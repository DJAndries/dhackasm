#ifndef TABLE_H
#define TABLE_H

struct symbol_entry {
  const char* symbol;
  unsigned short address;
  const char* data;
};
typedef struct symbol_entry symbol_entry;

struct table {
  symbol_entry* map;
  unsigned int size;
};
typedef struct table table;

table* table_init(int size);

symbol_entry* table_get(table* table, const char* symbol);

symbol_entry* table_put(table* table, const char* symbol, unsigned short address, const char* data);

#endif