#include <stdio.h>
#include <sys/time.h>

#include "table.h"
#include "parser.h"
#include "symbol_scan.h"
#include "translator.h"

int main(int argc, char** argv) {
  table* symbol_table;
  parsed_list* p_list;
  struct timeval start_time, end_time;

  if (argc < 2) {
    fprintf(stderr, "usage: dhackasm <input asm file> <output hack file>\n", argv[0]);
    return 1;
  }

  gettimeofday(&start_time, 0);
  
  symbol_table = table_init(100000);

  if (symbol_table == 0) {
    return 1;
  }

  p_list = parse_asm(argv[1]);
  if (p_list == 0) {
    return 1;
  }

  scan_symbols(p_list, symbol_table);

  if (translate(argv[2], p_list, symbol_table) != 0) {
    return 1;
  }

  gettimeofday(&end_time, 0);

  printf("Execution: %f ms\n", (double)(end_time.tv_usec - start_time.tv_usec) / 1000.0);

  return 0;
}

