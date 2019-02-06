#include "../../inc/sh.h"


void print_array(int size, char **cmd)
{
	int i;

	i = 0;
	while(i < size)
	{
		printf("%s\n",cmd[i]);
		i++;
	}
}

void print_array_token(int size, t_token_type *cmd)
{
	int i;

	i = 0;
	while(i < size)
	{
		printf("%d\n",cmd[i]);
		i++;
	}
}

void print_array_int(int size, int *cmd)
{
	(void)size;
	(void)cmd;
	 int i;

	 i = 0;
	 while(i < size)
	 {
	 	printf("%d\n",cmd[i]);
	 	i++;
	 }
}

void print_struct(t_command cmd)
{
	(void)cmd;
	 int j = 0;
	
	 while (j < cmd.used_space)
	 {
	 	printf("COMMAND : \n");
	 	print_array(cmd.command[j].used_space,cmd.command[j].cmd_simple);
		printf("tok : %d\n", cmd.command[j].tok);
		if (cmd.command[j].redirection.used_space > 0)
		{
			printf("REDIRECTIONS: \n");
			print_array_token(cmd.command[j].redirection.used_space, cmd.command[j].redirection.red);
			print_array(cmd.command[j].redirection.used_space, cmd.command[j].redirection.file);
			print_array_int(cmd.command[j].redirection.used_space, cmd.command[j].redirection.fd);	
		}
		j++;
	}
}
