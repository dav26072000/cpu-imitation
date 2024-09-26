#include "registers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void registers_init(Register **registers, size_t size)
{
    Register *tmp = (Register *)malloc(sizeof(Register) * size);

    for (int i = 0; i < size; ++i)
    {
        if (i >= 0 && i <= 6)
        {
            tmp[i].name = (char *)malloc(sizeof(char) * 3);
            tmp[i].name[0] = 'R';
            tmp[i].name[1] = i + 48;
            tmp[i].name[2] = '\0';
        }
        else if (i == 7)
        {
            tmp[i].name = (char *)malloc(sizeof(char) * 4);
            strcpy(tmp[i].name, "WSR");
            tmp[i].name[4] = '\0';
        }
        else if (i == 8)
        {
            tmp[i].name = (char *)malloc(sizeof(char) * 3);
            strcpy(tmp[i].name, "IP");
            tmp[i].name[4] = '\0';
        }
    }

    if (*registers != NULL)
        free(*registers);

    *registers = tmp;
}

int registers_print(Register *registers, size_t size)
{
    if (registers == NULL)
    {
        perror("Get: Invalid Register Address");
        return -1;
    }
    

    for (int i = 0; i < size; i++)
    {
        printf("Register Name = %s, Register Val = %d \n", registers[i].name, registers[i].value);
    }

    return 1;
}

Register *registers_get_by_name(Register *registers, size_t size, char *name)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(registers[i].name, name) == 0)
        {
            return &(registers[i]);
        }
    }

    return NULL;
}

int registers_set_value(Register *register_addr, int value)
{
    if (register_addr != NULL)
    {
        register_addr->value = value;
        return 1;
    }
    perror("Get: Invalid Register Address");
    return -1;
}

int registers_get_value(Register *register_addr)
{
    if (register_addr != NULL)
    {
        return register_addr->value;
    }

    perror("Get: Invalid Register Address");
    return -1;
}

int registers_is_true(Register *register_addr)
{
    if (register_addr != NULL)
    {
        return register_addr->value ? 1 : 0;
    }

    return -1;
}

void registers_incriment(Register *register_addr)
{
    if (register_addr != NULL)
    {
        (register_addr->value)++;
        return;
    }
    perror("Inc: Invalid Register Address");
    return;
}