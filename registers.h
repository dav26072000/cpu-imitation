#ifndef REGISTERS_FUNCTIONS
#define REGISTERS_FUNCTIONS
#include <stdlib.h>

typedef struct
{
    char *name;
    int value;
} Register;

void registers_init(Register **registers, size_t size);
int registers_print(Register *registers, size_t size);
Register *registers_get_by_name(Register *registers, size_t size, char *name);
int registers_set_value(Register *register_addr, int value);
int registers_get_value(Register *register_addr);
int registers_is_true(Register *register_addr);
void registers_incriment(Register *register_addr);

#endif