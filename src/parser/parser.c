/* ************************************************************************** */
/*                                                          LE - /            */
/*                                                              /             */
/*   ft_strstr.c                                      .::    .:/ .      .::   */
/*                                                 +:+:+   +:    +:  +:+:+    */
/*   By: gurival- <marvin@le-101.fr>                +:+   +:    +:    +:+     */
/*                                                 #+#   #+    #+    #+#      */
/*   Created: 2018/04/19 18:02:22 by gurival-     #+#   ##    ##    #+#       */
/*   Updated: 2018/04/19 18:02:22 by gurival-    ###    #+. /#+    ###.fr     */
/*                                                         /                  */
/*                                                        /                   */
/* ************************************************************************** */

#include "../../inc/sh.h"
#include "../../inc/expansion.h"
#include "../../inc/builtin.h"

void assign_tok(t_command *cmd, t_lexer lex, int *j, int val_tok)
{
	// (void)lex;// ATTENTION INUTILISE
	simple_cmd_assign(cmd, *cmd->command);
	(*j)++;
	tab_init(&cmd->command[*j]);
	tab_red_init(&cmd->command[*j].redirection);
	cmd->command[*j].tok = val_tok;
}

void    tab_io_assign(t_red *redir, t_lexer lex, int j)
{
	int *temp;
	int i;

	i = -1;	
	temp = redir->fd;
	if (!(redir->fd = (int *)malloc(sizeof(int) * redir->av_space + 1)))
		return ;
	if (redir->fd == NULL)
		exit(EXIT_FAILURE);
		while(++i < redir->used_space)
			redir->fd[i] = temp[i];
		free(temp);
	if (j >= 0 && lex.tokens[j].type == T_IO_NUMB)
		redir->fd[redir->used_space] =  ft_atoi(lex.tokens[j].content);
	else
		redir->fd[redir->used_space] = 1;
}

/*
	add element to the dynamic table of simple commands according to 
	existing operators
	res of this function:
	a dynamic array with operands assigned but without commands yet
	ex: ls | echo hei && diff blabla
	[0]{tok = | ; cmd_simple = NULL}
	[1]{tok = && ; cmd_simple = NULL}
	[2]{tok = -1 ; cmd_simple = NULL}
*/

void    add_token_val(t_command *cmd, t_lexer lex, int *j)
{
	int i;
	
	i = -1;
	*j = -1;
	while (++i < lex.used_size)
	{	
		if (lex.tokens[i].type == T_SEMI)
			assign_tok(cmd, lex, j, T_SEMI);
		if (lex.tokens[i].type == T_PIPE)
			assign_tok(cmd, lex, j, T_PIPE);
		else if(lex.tokens[i].type == T_DBLAND)
			assign_tok(cmd, lex, j, T_DBLAND);
		else if(lex.tokens[i].type == T_DBLOR)
			assign_tok(cmd, lex, j, T_DBLOR);
		else if(lex.tokens[i].type == T_AND)
			assign_tok(cmd, lex, j, T_AND);
		else if (i + 1 == lex.used_size)
			assign_tok(cmd, lex, j, -1);
	}
}

// function tells if the word after the >& && <& is a redir number

int		ft_isnumber_redir(char *str)
{
	dprintf(2, "rentre dans ft_isnumber_redir\n");
	int i;

	i = -1;
	if (!ft_strcmp(str, "-"))
		return (0);
	while (str[++i])
		if (!ft_isdigit(str[i]))
			return (1);
	return (0);
}

/* (1) and (2) below take dynamic array created in "add_token_val" function 
	and complete array of commands giving values to cmd_simple 
	and redirections if existes*/
/*(1)*/

void	complete_simple_command_and_red(t_command *cmd, t_lexer lex, int i,
			 int *j)
{
	if (i == 0 && lex.tokens[i].type == T_WORD)
		tab_assign(&cmd->command[*j], lex, i);
	else if (lex.tokens[i].type == T_WORD && !is_red(lex, i - 1) &&
			lex.tokens[i - 1].type != T_IO_NUMB)
			tab_assign(&cmd->command[*j], lex, i);
		//
	else if (lex.tokens[i].type == T_REDIR && lex.tokens[i + 1].type == T_WORD && !ft_isnumber_redir(lex.tokens[i + 1].content))
	{
		//test
		dprintf(2, "rentre ma poule dans le T_REDIR\n");
	}
	//
	else if (is_red(lex, i) && lex.tokens[i + 1].type == T_WORD)
	{
		tab_io_assign(&cmd->command[*j].redirection, lex, i - 1);
		tab_red_assign(&cmd->command[*j].redirection, lex, i, i + 1);
	}
	else if(lex.tokens[i + 1].type != T_WORD && is_op(lex,i))
		*cmd->command[*j + 1].cmd_simple = NULL;
	else if (!is_red(lex, i) && lex.tokens[i].type != T_IO_NUMB &&
			lex.tokens[i].type != T_WORD)
			(*j)++;
}

/*(2)*/
void    add_simple_command(t_command *cmd, t_lexer lex, t_dlist **history,
			 char ***heredoc)
{
	int size_simple_cmd;
	int i;
	int j;

	i = -1;
	j = 0;
	size_simple_cmd = 0;
	ft_manage_heredoc(&lex, heredoc, history);
	if (parse_errors(lex, -1))
	{
		add_token_val(cmd, lex, &size_simple_cmd);
		while (++i < lex.used_size && j <= size_simple_cmd)
			complete_simple_command_and_red(cmd, lex, i, &j);
	}
}