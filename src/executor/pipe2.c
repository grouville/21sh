/* ************************************************************************** */
/*                                                          LE - /            */
/*                                                              /             */
/*   pipe2.c                                          .::    .:/ .      .::   */
/*                                                 +:+:+   +:    +:  +:+:+    */
/*   By: gurival- <marvin@le-101.fr>                +:+   +:    +:    +:+     */
/*                                                 #+#   #+    #+    #+#      */
/*   Created: 2019/02/12 22:17:57 by gurival-     #+#   ##    ##    #+#       */
/*   Updated: 2019/03/05 14:45:14 by gurival-    ###    #+. /#+    ###.fr     */
/*                                                         /                  */
/*                                                        /                   */
/* ************************************************************************** */

#include "../../inc/sh.h"
#include "../../inc/exec.h"

int		ret_nr_pipe(t_command cmd, int i)
{
	int nr_pipe;

	nr_pipe = 0;
	while (cmd.command[i].tok == T_PIPE)
	{
		nr_pipe++;
		i++;
	}
	return (nr_pipe + 1);
}

int		exec_in_child(t_env_tools *env, t_command cmd, int *i, t_exec_redir *t)
{
	char	**command;
	int		ret;

	ret = 0;
	command = expense_cmd(cmd, *env, *i);
	dup2(t->pipe_tools.fd_in, 0);
	if (t->pipe_tools.aux + 1 != t->pipe_tools.len_pipe)
		dup2(t->pipe_tools.fds[1], 1);
	if (cmd.command[*i].redirection.used_space > 0)
		ret = process_redirections(t, cmd.command[*i], env,
			(*i) - t->count_here);
	close(t->pipe_tools.fds[0]);
	ret = ft_exec_command(env, command, t->pipe_tools.pid);
	env->g_return_value = ret;
	if (is_built_in(command) || (env->g_return_value != 0))
		exit(EXIT_FAILURE);
	if (command[0] == NULL)
		exit(EXIT_FAILURE);
	free_str(command);
	return (ret);
}

void	norm_pipe_exec(t_exec_redir *t, t_command cmd, int i)
{
	t->pipe_tools.len_pipe = ret_nr_pipe(cmd, i);
	t->pipe_tools.aux = 0;
	t->pipe_tools.fd_in = 0;
}

void	ft_exec_command_norm(int *i)
{
	while (wait(NULL) > 0)
		;
	(*i)--;
}

int		ft_pipe_exec(t_env_tools *env, t_command cmd, int *i, t_exec_redir *t)
{
	int		status;

	norm_pipe_exec(t, cmd, *i);
	while (t->pipe_tools.aux < t->pipe_tools.len_pipe)
	{
		pipe(t->pipe_tools.fds);
		t->pipe_tools.pid = fork();
		if (t->pipe_tools.pid == -1)
			exit(EXIT_FAILURE);
		if (t->pipe_tools.pid == 0)
			exec_in_child(env, cmd, i, t);
		close(t->pipe_tools.fds[1]);
		if (t->pipe_tools.fd_in != 0)
			close(t->pipe_tools.fd_in);
		t->pipe_tools.fd_in = t->pipe_tools.fds[0];
		(*i)++;
		t->pipe_tools.aux++;
		if (t->pipe_tools.aux == t->pipe_tools.len_pipe)
			(*i)--;
	}
	waitpid(t->pipe_tools.pid, &status, 0);
	env->g_return_value = right_return(status);
	ft_exec_command_norm(i);
	return (env->g_return_value);
}
