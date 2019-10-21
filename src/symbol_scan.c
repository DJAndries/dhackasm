#include "table.h"
#include "parser.h"

void load_builtin_symbols(table* table) {
  table_put(table, "SP", 0, 0);
  table_put(table, "LCL", 1, 0);
  table_put(table, "ARG", 2, 0);
  table_put(table, "THIS", 3, 0);
  table_put(table, "THAT", 4, 0);
  table_put(table, "R0", 0, 0);
  table_put(table, "R1", 1, 0);
  table_put(table, "R2", 2, 0);
  table_put(table, "R3", 3, 0);
  table_put(table, "R4", 4, 0);
  table_put(table, "R5", 5, 0);
  table_put(table, "R6", 6, 0);
  table_put(table, "R7", 7, 0);
  table_put(table, "R8", 8, 0);
  table_put(table, "R9", 9, 0);
  table_put(table, "R10", 10, 0);
  table_put(table, "R11", 11, 0);
  table_put(table, "R12", 12, 0);
  table_put(table, "R13", 13, 0);
  table_put(table, "R14", 14, 0);
  table_put(table, "R15", 15, 0);
  table_put(table, "SCREEN", 16384, 0);
  table_put(table, "KBD", 24576, 0);
}

void scan_symbols(parsed_list* list, table* table) {
  int next_var_addr = 16;
  int next_inst_addr = 0;
  parsed_node* node = list->start;

  load_builtin_symbols(table);

  /* first pass for finding labels */
  while (node) {
    if (node->data.is_label == 1) {
      table_put(table, node->data.params[0], next_inst_addr, 0);
    } else {
      next_inst_addr++;
    }
    node = node->next;
  }

  node = list->start;
  /* second pass to find variables */
  while (node) {
    if (node->data.is_c_instruction == 0) {
      if (node->data.params[0][0] < '0' || node->data.params[0][0] > '9') {
        /* if first char is not number, we can assume variable */
        if (table_get(table, node->data.params[0]) == 0) {
          table_put(table, node->data.params[0], next_var_addr++, 0);
        }
      }
    }

    node = node->next;
  }
}