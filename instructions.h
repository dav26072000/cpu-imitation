#ifndef INSTRUCTIONS_FUNCTIONS
#define INSTRUCTIONS_FUNCTIONS
#include <stdlib.h>
#include "registers.h"
#include "ram.h"

int handle_ALU_instruction(Register *registers, size_t registers_size, char *store_register_name, char *register_1_name, char *register_2_name, char *mode);
int handle_mov_instruction(Register *registers, size_t registers_size, char *store_register_name, char *register_1_name);
int handle_load_instruction(Register *registers, size_t registers_size, int *ram, size_t ram_size, char *store_register_name, char *ram_addr);
int handle_store_instruction(Register *registers, size_t registers_size, int *ram, size_t ram_size, char *store_ram_addr, char *register_1_name);
int handle_start_instruction(Register *register_wsr);
int handle_exit_instruction(Register *register_wsr);
int handle_layo_instruction(Register *registers, size_t registers_size, int *ram, size_t ram_size);
int handle_disc_instruction(int fd, Register *registers, int registers_size, int *ram, int ram_size, char *disc);

#endif