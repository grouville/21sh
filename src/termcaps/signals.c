/* ************************************************************************** */
/*                                                          LE - /            */
/*                                                              /             */
/*   ft_strstr.c                                      .::    .:/ .      .::   */
/*                                                 +:+:+   +:    +:  +:+:+    */
/*   By: gurival- <marvin@le-101.fr>                +:+   +:    +:    +:+     */
/*                                                 #+#   #+    #+    #+#      */
/*   Created: 2018/04/19 18:02:22 by gurival-     #+#   ##    ##    #+#       */
/*   Updated: 2019/02/28 15:56:02 by gurival-    ###    #+. /#+    ###.fr     */
/*                                                         /                  */
/*                                                        /                   */
/* ************************************************************************** */

#include "../../inc/sh.h"

/*
*** - Aim of the function : int function to use putchar_fd inside if
*/

static int		ft_putc_if(char c)
{
	ft_putchar_fd(c, 1);
	return (0);
}

static void		ctrl_c_norm(t_tcap *caps)
{
	if (ft_strcmp(caps->prompt, "bash > ") && (caps->sz_str = 10))
	{
		free(caps->str[0]);
		g_keeprun = 2;
		if (!ft_strcmp(caps->prompt, "dquote > "))
			caps->str[0] = ft_strdup("\"");
		else if (!ft_strcmp(caps->prompt, "squote > "))
			caps->str[0] = ft_strdup("\'");
		else if (!ft_strcmp(caps->prompt, "Missing arguments > "))
			caps->str[0] = ft_strdup("oui");
		else if (!ft_strcmp(caps->prompt, "\nHeredoc > "))
			caps->str[0] = ft_strdup("pppppp");
		else if (!ft_strcmp(caps->prompt, "Heredoc > "))
			caps->str[0] = ft_strdup("pppppp");
		else if (!ft_strcmp(caps->prompt, "> "))
			caps->str[0] = ft_strdup("ppppppp");
	}
}

/*
*** - Aim of the function : manage the ctrl_c signal
*** - Handles the resizing of the terminal window.
*** - do not forget to free the copy of the prompt when exit termcaps
*/

int				ctrl_c(t_tcap *caps)
{
	char	c;

	c = 10;
	if (g_keeprun != 3 && !ft_putc_if('\n'))
		return (0);
	g_keeprun = 1;
	end_key(caps);
	if (caps->str[0])
		free(caps->str[0]);
	caps->str[0] = ft_memalloc(1);
	caps->sz_str = ft_strlen(caps->prompt);
	while (caps->history[0]->prev)
		caps->history[0] = caps->history[0]->prev;
	ctrl_c_norm(caps);
	ioctl(0, TIOCSTI, &c);
	return (0);
}

/*
*** - Aim of the function :
*** - To catch the ctrl signal when in the execution of a command
*/

static void		ctrl_clast(int sig)
{
	(void)sig;
}

/*
*** Handles the resizing of the terminal window and the ctrl_c signal
*/

int				initialize_signals(void)
{
	signal(SIGINT, ctrl_clast);
	return (1);
}
