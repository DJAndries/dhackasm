#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include "table.h"
#include "parser.h"

char translate(const char* out_filename, parsed_list* list, table* symbol_table);

#endif