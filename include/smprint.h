#include "smmem.h"
#include <stdio.h>

void print_free_list();
void print_free_list_blocks();
void print_block(sblock *block);
size_t free_list_total_size();
void print_free_list_stats();
