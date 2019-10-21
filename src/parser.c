#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void insert_list_node(parsed_list* list, parsed_node* node) {
  node->next = 0;
  if (list->start == 0) {
    list->start = node;
  }
  if (list->end) {
    list->end->next = node;
  }
  list->end = node;
}

parsed_list* init_list() {
  parsed_list* list = (parsed_list*)malloc(sizeof(parsed_list));

  if (list == 0) {
    fprintf(stderr, "Failed to allocate parsed list\n");
    return 0;
  }

  list->start = list->end = 0;

  return list;
}

parsed_node* init_node() {
  parsed_node* node = (parsed_node*)malloc(sizeof(parsed_node));

  if (node == 0) {
    fprintf(stderr, "Failed to allocate parsed node\n");
    return 0;
  }
  
  memset(node, 0, sizeof(parsed_node));

  return node;
}

char check_end_of_line(char c) {
  return c == '/' || c == '\r' || c == '\n' || c == '\0';
}

char check_whitespace_or_delimiter(char c) {
  return c == ' ' || c == ';' || c == ')' || c == '=';
}

char is_copying_param(int param_i) {
  return param_i > -1 && param_i < 3;
}

parsed_node* parse_line(char* line) {
  parsed_node* node = 0;
  int len = strlen(line);
  int i;
  int param_i = -1;
  int dest_i = 0;
  char c;

  for (i = 0; i < len; i++) {
    c = line[i];
    if (check_end_of_line(c) || check_whitespace_or_delimiter(c)) {
      if (is_copying_param(param_i)) {
        /* deal with end of param */
        node->data.params[param_i++][dest_i] = '\0';
        dest_i = 0;
        if (c == '=') {
          node->data.has_dest = 1;
        } else if (c == ';') {
          node->data.has_jmp = 1;
        }
      }
      if (check_end_of_line(c)) {
        /* finished processing line */
        return node;
      } else {
        continue;
      }
    }
    if (node == 0) {
      node = init_node();
      if (node == 0) {
        return (parsed_node*)-1;
      }
      param_i = 0;
      if (c == '@') {
        /* a instruction, start copying on next char*/
        continue;
      } else if (c == '(') {
        /* label, start copying on next char*/
        node->data.is_label = 1;
        continue;
      } else {
        /* c instruction */
        node->data.is_c_instruction = 1;
      }
    }
    if (is_copying_param(param_i)) {
      node->data.params[param_i][dest_i++] = c;
    }
  }

  return node;
}

parsed_list* parse_asm(const char* filename) {
  char line[1024];
  FILE* file;
  parsed_list* list;
  parsed_node* node;
  int curr_line_num = 0;

  list = init_list();
  if (list == 0) {
    return 0;
  }

  file = fopen(filename, "r");
  if (file == 0) {
    fprintf(stderr, "Failed to open ASM file\n");
    return 0;
  }

  while (fgets(line, sizeof(line), file)) {
    curr_line_num++;
    node = parse_line(line);
    if (node == (parsed_node*)-1) {
      return 0;
    } else if (node == 0) {
      continue;
    } else {
      node->data.line_num = curr_line_num;
      insert_list_node(list, node);
    }
  }

  fclose(file);

  return list;
}
