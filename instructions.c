#include "instructions.h"
#include "registers.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int handle_start_instruction(Register *register_wsr)
{
    int val = registers_is_true(register_wsr);

    if (val == 1)
    {
        perror("Code has been already started");

        return -1;
    }

    if (registers_set_value(register_wsr, 1) == -1)
    {
        return -1;
    };

    return 1;
}

int handle_exit_instruction(Register *register_wsr)
{
    int val = registers_is_true(register_wsr);

    if (val == 0)
    {
        perror("Code has been already exited");

        return -1;
    }

    if (registers_set_value(register_wsr, 0) == -1)
    {
        return -1;
    };

    return 1;
}

int handle_disc_instruction(int fd, Register *registers, int registers_size, int *ram, int ram_size, char *disk)
{
    char *end_ptr;
    int literal_value = strtol(disk, &end_ptr, 10);
    int is_literal = strlen(end_ptr) > 0 ? 0 : 1;
    if (!is_literal)
    {
        perror("DISC: Argument must be literal integer");
        return -1;
    }

    int disk_number = literal_value;

    int disk_size = ((sizeof(Register) * registers_size) + (sizeof(int) * ram_size)) * (disk_number + 1);
    lseek(fd, -disk_size, SEEK_END);

    for (int i = 0; i < registers_size; ++i)
    {
        read(fd, &registers[i], sizeof(registers[i]));
    }

    for (int i = 0; i < ram_size; ++i)
    {
        read(fd, &ram[i], sizeof(ram[i]));
    }

    struct stat st;
    stat("db.txt", &st);

    ftruncate(fd, st.st_size - disk_size);
    return 1;
}

int handle_load_instruction(Register *registers, size_t registers_size, int *ram, size_t ram_size, char *store_register_name, char *ram_addr)
{
    Register *store_register = registers_get_by_name(registers, registers_size, store_register_name);
    if (store_register == NULL)
    {
        perror("LOAD: Faild to get register (store register)");

        return -1;
    }

    int *ram_cell_addr = ram_get_cell(ram, ram_size, ram_addr);
    if (ram_cell_addr == NULL)
    {
        perror("LOAD: Faild to get ram cell address");

        return -1;
    }

    int ram_cell_value = ram_get_cell_value(ram_cell_addr);
    if (ram_cell_value == -1)
    {
        perror("LOAD: faild to get cell value");
        return -1;
    }

    if (registers_set_value(store_register, ram_cell_value) == -1)
    {
        return -1;
    };

    return 1;
}

int handle_store_instruction(Register *registers, size_t registers_size, int *ram, size_t ram_size, char *store_ram_addr, char *register_1_name)
{
    int *store_ram_cell_addr = ram_get_cell(ram, ram_size, store_ram_addr);
    if (store_ram_cell_addr == NULL)
    {
        perror("STORE: Faild to get ram cell address");
        return -1;
    }

    Register *register_1 = registers_get_by_name(registers, registers_size, register_1_name);
    if (register_1 == NULL)
    {
        perror("STORE: Faild to get register");
        return -1;
    }

    int register_1_value;
    if ((register_1_value = registers_get_value(register_1)) == -1)
    {
        perror("ALU - ADD: Faild to get register value");

        return -1;
    }

    if (ram_set_cell_value(store_ram_cell_addr, register_1_value) == -1)
        return -1;

    return 1;
}

// ALU
int handle_ALU_instruction(Register *registers, size_t registers_size, char *store_register_name, char *register_1_name, char *register_2_name, char *mode)
{
    if (strcmp(mode, "ADD") != 0 && strcmp(mode, "SUB") != 0)
    {
        perror("ALU: Undefined operation");
        return -1;
    }

    // ADD R0 R0 R1
    Register *store_register = registers_get_by_name(registers, registers_size, store_register_name);
    if (store_register == NULL)
    {
        perror("ALU - ADD: Faild to get register (store register)");

        return -1;
    }
    Register *register_1 = registers_get_by_name(registers, registers_size, register_1_name);
    if (register_1 == NULL)
    {
        perror("ALU - ADD: Faild to get register (first argument)");

        return -1;
    }
    int register_1_value;
    if ((register_1_value = registers_get_value(register_1)) == -1)
    {
        perror("ALU - ADD: Faild to get register value");

        return -1;
    }

    char *end_ptr;
    int literal_value = strtol(register_2_name, &end_ptr, 10);
    int is_literal = strlen(end_ptr) > 0 ? 0 : 1;
    Register *register_2 = NULL;
    int register_2_value;
    if (!is_literal)
    {
        Register *register_2 = registers_get_by_name(registers, registers_size, register_2_name);
        if (register_2 == NULL)
        {
            perror("ALU - ADD: Faild to get register (second argument)");
            return -1;
        }

        if ((register_2_value = registers_get_value(register_2)) == -1)
        {
            perror("ALU - ADD: Faild to get register value");
            return -1;
        }
    }
    else
    {
        register_2_value = literal_value;
    }

    if (strcmp(mode, "ADD") == 0)
    {
        if (registers_set_value(store_register, register_1_value + register_2_value) == -1)
        {
            return -1;
        };
    }
    else if (strcmp(mode, "SUB") == 0)
    {
        if (registers_set_value(store_register, register_1_value - register_2_value) == -1)
        {
            return -1;
        };
    }

    return 1;
}

int handle_mov_instruction(Register *registers, size_t registers_size, char *store_register_name, char *register_1_name)
{
    Register *store_register = registers_get_by_name(registers, registers_size, store_register_name);
    if (store_register == NULL)
    {
        perror("MOV: Faild to get register (store register)");
        return -1;
    }

    char *end_ptr;
    int literal_value = strtol(register_1_name, &end_ptr, 10);
    int is_literal = strlen(end_ptr) > 0 ? 0 : 1;
    Register *register_1 = NULL;
    int register_1_value;
    if (!is_literal)
    {
        Register *register_1 = registers_get_by_name(registers, registers_size, register_1_name);
        if (register_1 == NULL)
        {
            perror("ALU - ADD: Faild to get register (second argument)");
            return -1;
        }

        if ((register_1_value = registers_get_value(register_1)) == -1)
        {
            perror("ALU - ADD: Faild to get register value");
            return -1;
        }
    }
    else
    {
        register_1_value = literal_value;
    }

    if (registers_set_value(store_register, register_1_value) == -1)
    {
        return -1;
    };
}

int handle_layo_instruction(Register *registers, size_t registers_size, int *ram, size_t ram_size)
{
    if (registers_print(registers, registers_size) == -1)
    {
        return -1;
    }
    if (ram_print(ram, ram_size) == -1)
    {
        return -1;
    }
    return 1;
}