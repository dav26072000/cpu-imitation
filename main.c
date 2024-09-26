#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ram.h"
#include "registers.h"
#include "instructions.h"

#define RAM_SIZE 256
#define REGISTERS_COUNT 9
#define INSTRUCTIONS_COUNT 8

int write_to_db(int fd, Register *registers, int registers_size, int *ram, int ram_size, Register *register_ip)
{
    for (int i = 0; i < registers_size; ++i)
    {
        write(fd, &registers[i], sizeof(registers[i]));
    }

    for (int i = 0; i < ram_size; ++i)
    {
        write(fd, &ram[i], sizeof(ram[i]));
    }

    registers_incriment(register_ip);
    return 1;
}

void arguments_init(char *line_buffer, char *instruction_name, char *arg_1, char *arg_2, char *arg_3)
{
    // Get the first token which is the instruction name
    char *each_token = strtok(line_buffer, " ");
    if (each_token == NULL)
    {
        perror("Instruction name is missing");
        return;
    }

    // Copy the instruction name
    if (instruction_name)
        *instruction_name = '\0';

    strcpy(instruction_name, each_token);
    instruction_name[strlen(instruction_name)] = '\0';

    // Initialize argument strings to empty
    if (arg_1)
        *arg_1 = '\0';
    if (arg_2)
        *arg_2 = '\0';
    if (arg_3)
        *arg_3 = '\0';

    // Start fetching arguments
    int i = 0;
    while ((each_token = strtok(NULL, " ")) != NULL)
    {
        if (i == 0 && arg_1)
        {
            strcpy(arg_1, each_token);
        }
        else if (i == 1 && arg_2)
        {
            strcpy(arg_2, each_token);
        }
        else if (i == 2 && arg_3)
        {
            strcpy(arg_3, each_token);
        }
        else if (i > 2)
        {
            printf("%s : ", instruction_name);
            perror("Too many arguments provided");
            exit(EXIT_FAILURE);
        }
        ++i;
    }
}

int is_instruction_exists(char *all_instructions[], int instructions_count, char *instruction_name)
{

    for (int i = 0; i < instructions_count; ++i)
    {
        if (strcmp(all_instructions[i], instruction_name) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    // ASM code path
    char * asm_code_path = argv[1];
    for (int i = 0; i < argc; ++i)
    {
        printf("%s \n", argv[i]);
    }

    // Init RAM
    int *ram = NULL;
    ram_init(&ram, RAM_SIZE);

    // Init Registers
    Register *registers = NULL;
    registers_init(&registers, REGISTERS_COUNT);
    Register *register_wsr = registers_get_by_name(registers, REGISTERS_COUNT, "WSR");
    Register *register_ip = registers_get_by_name(registers, REGISTERS_COUNT, "IP");

    // All instructions
    char *all_instructions[] = {"ADD", "SUB", "MOV", "LOAD", "STORE", "EXIT", "DISC", "LAYO"};

    // Open db for write and read
    int fd = open("db.txt", O_APPEND | O_CREAT | O_RDWR | O_TRUNC, 0666);

    // Fetch code file and execute line by line
    FILE *code_asm_file = fopen(asm_code_path, "r");
    if (code_asm_file == NULL)
    {
        perror("Failed to open code file [Incorrect path name]");
        exit(EXIT_FAILURE);
    }
    char line_buffer[100];

    int is_started = 0;
    // Fetching
    while (fgets(line_buffer, 100, code_asm_file) != NULL)
    {
        // Skeep spaces
        if (strlen(line_buffer) == 1 || line_buffer[0] == '\n')
            continue;

        // Remove newline character if it's present
        line_buffer[strcspn(line_buffer, "\n")] = 0;

        char instruction_name[100], arg_1[100], arg_2[100], arg_3[100];
        arguments_init(line_buffer, instruction_name, arg_1, arg_2, arg_3);

        int single_instruction_check = strlen(arg_1) + strlen(arg_2) + strlen(arg_3);
        int one_arg_instruction_check = strlen(arg_2) + strlen(arg_3);
        int two_arg_instruction_check = strlen(arg_3);

        int is_first_arg_set = strlen(arg_1) ? 1 : 0;
        int is_second_arg_set = strlen(arg_2) ? 1 : 0;
        int is_third_arg_set = strlen(arg_3) ? 1 : 0;

        // Check is code started
        is_started = registers_is_true(register_wsr);

        if (strcmp(instruction_name, "START") == 0)
        {
            if (single_instruction_check)
            {
                perror("START: Cannot have arguments");
                exit(EXIT_FAILURE);
                break;
            }

            if (handle_start_instruction(register_wsr) == -1)
            {
                break;
            };

            write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
        }

        if (is_started)
        {
            if (!is_instruction_exists(all_instructions, INSTRUCTIONS_COUNT, instruction_name))
            {
                printf("%s : ", instruction_name);
                perror("Does not exists");
                exit(EXIT_FAILURE);
                break;
            }

            // EXECUTION
            if (strcmp(instruction_name, "MOV") == 0)
            {
                if (!is_second_arg_set || !is_first_arg_set || two_arg_instruction_check)
                {
                    perror("MOV: 2 arguments must be set");
                    exit(EXIT_FAILURE);
                    break;
                }

                if (handle_mov_instruction(registers, REGISTERS_COUNT, arg_1, arg_2) == -1)
                {
                    break;
                };

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
            else if (strcmp(instruction_name, "ADD") == 0 || strcmp(instruction_name, "SUB") == 0)
            {
                if (!is_second_arg_set || !is_first_arg_set || !is_third_arg_set)
                {
                    perror("ADD: 3 arguments must be set");
                    exit(EXIT_FAILURE);
                    break;
                }

                if (handle_ALU_instruction(registers, REGISTERS_COUNT, arg_1, arg_2, arg_3, instruction_name) == -1)
                {
                    break;
                };

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
            else if (strcmp(instruction_name, "LOAD") == 0)
            {
                if (!is_second_arg_set || !is_first_arg_set || two_arg_instruction_check)
                {
                    perror("LOAD: 2 arguments must be set");
                    exit(EXIT_FAILURE);
                    break;
                }
                if (handle_load_instruction(registers, REGISTERS_COUNT, ram, RAM_SIZE, arg_1, arg_2) == -1)
                {
                    break;
                };

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
            else if (strcmp(instruction_name, "STORE") == 0)
            {
                if (!is_second_arg_set || !is_first_arg_set || two_arg_instruction_check)
                {
                    perror("STORE: 2 arguments must be set");
                    exit(EXIT_FAILURE);
                    break;
                }

                if (handle_store_instruction(registers, REGISTERS_COUNT, ram, RAM_SIZE, arg_1, arg_2) == -1)
                {
                    break;
                };

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
            else if (strcmp(instruction_name, "DISC") == 0)
            {
                if (!is_first_arg_set || one_arg_instruction_check)
                {
                    perror("DISC: 1 argument must be set");
                    exit(EXIT_FAILURE);
                    break;
                }
                if (handle_disc_instruction(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, arg_1) == -1)
                {
                    break;
                };

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
            else if (strcmp(instruction_name, "EXIT") == 0)
            {
                if (handle_exit_instruction(register_wsr) == -1)
                {
                    break;
                };

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
            else if (strcmp(instruction_name, "LAYO") == 0)
            {
                if (single_instruction_check)
                {
                    perror("LAYO: Cannot have arguments");
                    exit(EXIT_FAILURE);
                    break;
                }

                if (handle_layo_instruction(registers, REGISTERS_COUNT, ram, RAM_SIZE) == -1)
                {
                    break;
                }

                write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, register_ip);
            }
        }
    }

    // int *ram_cell_addr = ram_get_cell(ram, RAM_SIZE, "0x15");
    // if (ram_cell_addr)
    // {
    //     ram_set_cell_value(ram_cell_addr, 14);
    // }

    // Register *wsr = registers_get_by_name(registers, REGISTERS_COUNT, "WSR");
    // registers_set_value(wsr, 15);

    // write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE);

    // Register *r0 = registers_get_by_name(registers, REGISTERS_COUNT, "R0");
    // registers_set_value(r0, 90);

    // write_to_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE);

    // get_from_db(fd, registers, REGISTERS_COUNT, ram, RAM_SIZE, 1);

    return 0;
}