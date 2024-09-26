#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ram_init(int **ram, size_t size)
{
	int *tmp = (int *)malloc(sizeof(int) * size);
	for (int i = 0; i < size; ++i)
	{
		tmp[i] = 0;
		if (i == 15)
		{
			tmp[i] = 16;
		}
	}

	if (*ram != NULL)
		free(ram);

	*ram = tmp;
}

int ram_print(int *ram, size_t size)
{
	if (ram == NULL)
	{
		perror("Get: Invalid RAM Address");
		return -1;
	}

	for (int i = 0; i < size; ++i)
	{
		printf("0x%d = %d \n", i, ram[i]);
	}
	return 1;
}

int *ram_get_cell(int *ram, size_t size, char *addr)
{
	if (addr[0] != '0' || addr[1] != 'x')
	{
		return NULL;
	}

	int addr_num_str_len = strlen(addr) - 2;
	char *addr_num_str = (char *)malloc(sizeof(char) * addr_num_str_len + 1);
	if (addr_num_str == NULL)
	{
		return NULL;
	}

	addr_num_str[addr_num_str_len + 1] = '\0';
	for (int i = 0; i < addr_num_str_len; ++i)
	{
		addr_num_str[i] = addr[i + 2];
	}

	int addr_index = (int)atoi(addr_num_str);

	if (!addr || addr_index > size - 1 || addr_index < 0)
	{
		return NULL;
	}

	return &(ram[addr_index]);
}

int ram_set_cell_value(int *ram_cell_addr, int value)
{
	if (ram_cell_addr == NULL)
	{
		return -1;
	}

	*ram_cell_addr = value;
}

int ram_get_cell_value(int *ram_cell_addr)
{
	if (ram_cell_addr == NULL)
	{
		return -1;
	}

	return *ram_cell_addr;
}
