#ifndef RAM_FUNCTIONS
#define RAM_FUNCTIONS

#include <stdlib.h>

void ram_init(int **ram, size_t size);
int ram_print(int *ram, size_t size);
int *ram_get_cell(int *ram, size_t size, char *addr);
int ram_get_cell_value(int *ram_cell_addr);
int ram_set_cell_value(int *ram_cell_addr, int value);

#endif
