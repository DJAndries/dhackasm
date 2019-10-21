#include <stdio.h>
#include "translator.h"

table* init_op_table() {
  table* table = table_init(128);

  if (table == 0) {
    return 0;
  }

  table_put(table, "0", 0, "0101010");
  table_put(table, "1", 0, "0111111");
  table_put(table, "-1", 0, "0111010");
  table_put(table, "D", 0, "0001100");
  table_put(table, "A", 0, "0110000");
  table_put(table, "!D", 0, "0001101");
  table_put(table, "!A", 0, "0110001");
  table_put(table, "-D", 0, "0001111");
  table_put(table, "-A", 0, "0110011");
  table_put(table, "D+1", 0, "0011111");
  table_put(table, "A+1", 0, "0110111");
  table_put(table, "D-1", 0, "0001110");
  table_put(table, "A-1", 0, "0110010");
  table_put(table, "D+A", 0, "0000010");
  table_put(table, "D-A", 0, "0010011");
  table_put(table, "A-D", 0, "0000111");
  table_put(table, "D&A", 0, "0000000");
  table_put(table, "D|A", 0, "0010101");
  table_put(table, "M", 0, "1110000");
  table_put(table, "!M", 0, "1110001");
  table_put(table, "-M", 0, "1110011");
  table_put(table, "M+1", 0, "1110111");
  table_put(table, "M-1", 0, "1110010");
  table_put(table, "D+M", 0, "1000010");
  table_put(table, "D-M", 0, "1010011");
  table_put(table, "M-D", 0, "1000111");
  table_put(table, "D&M", 0, "1000000");
  table_put(table, "D|M", 0, "1010101");

  table_put(table, "JGT", 0, "001");
  table_put(table, "JEQ", 0, "010");
  table_put(table, "JGE", 0, "011");
  table_put(table, "JLT", 0, "100");
  table_put(table, "JNE", 0, "101");
  table_put(table, "JLE", 0, "110");
  table_put(table, "JMP", 0, "111");

  return table;
}

char process_a_instruction(FILE* fp, parsed_node* inst, table* symbol_table) {
  char output[17] = "0";
  symbol_entry* table_entry;
  unsigned short addr;
  char i;
  if (inst->data.params[0][0] < '0' || inst->data.params[0][0] > '9') {
    table_entry = table_get(symbol_table, inst->data.params[0]);

    if (table_entry == 0) {
      fprintf(stderr, "Internal error: cannot find symbol: %s\n", inst->data.params[0]);
      return 1;
    }
    addr = table_entry->address;
  } else {
    if (sscanf(inst->data.params[0], "%hu", &addr) == 0) {
      fprintf(stderr, "Cannot parse address: %s\n", inst->data.params[0]);
      return 1;
    }
  }

  for (i = 0; i < 15; i++) {
    output[i + 1] = ((addr >> (14 - i)) & 0x1) ? '1' : '0';
  }

  output[16] = '\0';

  fprintf(fp, "%s\n", output);

  return 0;
}

char process_c_instruction(FILE* fp, parsed_node* inst, table* symbol_table, table* op_table) {
  char dest[] = "000";
  const char* cmd;
  const char* jmp = "000";
  symbol_entry* table_entry;
  int i;
  char param_i = 0;

  if (inst->data.has_dest) {
    for (i = 0; inst->data.params[0][i] != '\0'; i++) {
      switch (inst->data.params[0][i]) {
        case 'A':
          dest[0] = '1';
          break;
        case 'D':
          dest[1] = '1';
          break;
        case 'M':
          dest[2] = '1';
          break;
      }
    }
    param_i++;
  }

  table_entry = table_get(op_table, inst->data.params[param_i]);

  if (table_entry == 0) {
    fprintf(stderr, "Cannot translate command: %s\n", inst->data.params[param_i]);
    return 1;
  }

  cmd = table_entry->data;
  param_i++;
  
  if (inst->data.has_jmp) {
    table_entry = table_get(op_table, inst->data.params[param_i]);

    if (table_entry == 0) {
      fprintf(stderr, "Cannot translate jump: %s\n", inst->data.params[param_i]);
      return 1;
    }

    jmp = table_entry->data;
  }

  fprintf(fp, "111%s%s%s\n", cmd, dest, jmp);
  
  return 0;
}

char translate(const char* out_filename, parsed_list* list, table* symbol_table) {
  table* op_table;
  FILE* fp;
  parsed_node* node = list->start;
  char result;

  op_table = init_op_table();

  if (op_table == 0) {
    return 1;
  }

  fp = fopen(out_filename, "w");

  if (fp == 0) {
    fprintf(stderr, "Failed to open output file\n");
    return 1;
  }

  while (node) {
    if (node->data.is_label == 0) {
      if (node->data.is_c_instruction) {
        result = process_c_instruction(fp, node, symbol_table, op_table);
      } else {
        result = process_a_instruction(fp, node, symbol_table);
      }
      if (result != 0) {
        fprintf(stderr, "Error on line %d\n", node->data.line_num);
        fclose(fp);
        return 1;
      }
    }

    node = node->next;
  }

  fclose(fp);

  return 0;
}