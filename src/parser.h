#ifndef PARSER_H
#define PARSER_H

struct parsed {
  char is_label;
  char is_c_instruction;

  char params[3][128];

  char has_dest;
  char has_jmp;
  int line_num;
};
typedef struct parsed parsed;

typedef struct parsed_node parsed_node;
struct parsed_node {
  parsed_node* next;
  parsed data;
};

struct parsed_list {
  parsed_node* start;
  parsed_node* end;
};
typedef struct parsed_list parsed_list;

parsed_list* parse_asm(const char* filename);

#endif