#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

unsigned int generate_hash(const char* symbol) {
  int i;
  int symbol_length = strlen(symbol);
  int hash = 7;
  for (i = 0; i < symbol_length; i++) {
    hash = hash * 31 + symbol[i];
  }
  return hash;
}

table* table_init(int size) {
  table* table = malloc(sizeof(table));

  if (table == 0) {
    fprintf(stderr, "Symbol table memory alloc error\n");
    return 0;
  }

  table->size = size;

  table->map = (symbol_entry*)malloc(sizeof(symbol_entry) * size);

  if (table->map == 0) {
    fprintf(stderr, "Symbol table memory alloc error\n");
    return 0;
  }

  memset(table->map, 0, sizeof(symbol_entry) * size);

  return table;
}

symbol_entry* table_get(table* table, const char* symbol) {
  int hash, i;
  unsigned int map_index;

  hash = generate_hash(symbol);

  map_index = hash % table->size;
  for (i = 0;
    i < table->size && table->map[map_index].symbol != 0;
    i++) {
    if (strcmp(symbol, table->map[map_index].symbol) == 0) {
      return &table->map[map_index];
    }
    map_index = (map_index + 1) % table->size;
  }

  return 0;
}

symbol_entry* table_put(table* table, const char* symbol, unsigned short address, const char* data) {
  int hash, i;
  unsigned int map_index;

  hash = generate_hash(symbol);
  map_index = hash % table->size;
  for (i = 0; i < table->size; i++) {
    if (table->map[map_index].symbol == 0) {
      table->map[map_index].symbol = symbol;
      table->map[map_index].address = address;
      table->map[map_index].data = data;
      return &table->map[map_index];
    }
    map_index = (map_index + 1) % table->size;
  }

  return 0;
}