/* ************************************************************************** */
/*                                                          LE - /            */
/*                                                              /             */
/*   ft_strstr.c                                      .::    .:/ .      .::   */
/*                                                 +:+:+   +:    +:  +:+:+    */
/*   By: gurival- <marvin@le-101.fr>                +:+   +:    +:    +:+     */
/*                                                 #+#   #+    #+    #+#      */
/*   Created: 2018/04/19 18:02:22 by gurival-     #+#   ##    ##    #+#       */
/*   Updated: 2019/02/09 18:05:33 by gurival-    ###    #+. /#+    ###.fr     */
/*                                                         /                  */
/*                                                        /                   */
/* ************************************************************************** */

#include "../../inc/sh.h"
#include "../../inc/builtin.h"

/*
*** - Aim of the function :
*** - Part one of the change dir and pwd function : created for the norm
*** - as a first step, we copy the proper string, which means that we manage
*** - the cd .. command, as the cd ../../ is automatically managed
*** - Then, we stat the string in order to collect the infos for the errors
*/

static void			ft_copy_and_stat(t_norm_pwd *n, char *av)
{
	if (n->dash == 0 && ft_strcmp(av, ".."))
	{
		getcwd(n->buf, sizeof(n->buf));
		n->s1 = ft_strjoin(n->buf, "/");
		n->s2 = ft_strjoin(n->s1, av);
		free(n->s1);
	}
	else
		n->s2 = ft_strdup(av);
	stat(n->s2, &n->buf2);
}

/*
*** - Aim of the function :
*** - Modify if str == " cd ////tmp/////test////" => it should work
*** - finds the current PATH and writes it
*/

static void			ft_multiple_slash_av(char **av)
{
	int		i;
	int		j;
	char	buf[ft_strlen(*av)];

	i = -1;
	j = -1;
	ft_bzero(buf, ft_strlen(*av));
	while ((*av)[++i])
	{
		if ((*av)[i] == '/' && (i >= 1 ? (*av)[i - 1] != '/' : 1))
			buf[++j] = (*av)[i];
		else if ((*av)[i] != '/')
			buf[++j] = (*av)[i];
	}
	free(*av);
	*av = ft_strndup(buf, j + 1);
}

/*
*** - Aim of the function :
*** - Changes the OLDPWD with the current PWD, or, if PWD unset
*** - finds the current PATH and writes it
*** - The last part (if n->dash = 0) if for a command with - such as cd -
*** - We prepare the proper string for the lstat command,
*** - in order to differenciate
*** - the behavior when Symbolic link or not
*/

static void			ft_manage_oldpwd_and_prepare_pwd(char *av, char ***c_env,
						t_env_tools *env, t_norm_pwd *n)
{
	if (n->tmp2)
		free(n->tmp2);
	while ((*c_env)[n->i] && ft_strncmp((*c_env)[n->i], "PWD=", 4))
		n->i++;
	if ((*c_env)[n->i])
		n->tmp2 = ft_strjoin("OLDPWD=", (*c_env)[n->i] + 4);
	else
	{
		getcwd(n->buf, sizeof(n->buf));
		n->tmp2 = ft_strjoin("OLDPWD=", n->buf);
	}
	ft_builtin_setenv_2(n->tmp2, c_env, &(env->paths), env);
	if (n->dash == 0)
	{
		free(n->tmp2);
		getcwd(n->buf, sizeof(n->buf));
		n->tmp = ft_strjoin(n->buf, "/");
		n->tmp2 = av[0] == '.' && av[1] == '/' ?
			ft_strjoin(n->tmp, ft_skip_slash(av)) : ft_strjoin(n->tmp, av);
		free(n->tmp);
		n->tmp = NULL;
	}
}

/*
*** - Aim of the Fonction : Changes the PWD in the env
*** - The function also changes the current directory
*** - We copy the bash behavior of the cd command for
*** - symbolic links and physical links
*/

static void			ft_norm_change_dir_and_pwds(char **av, char ***c_env,
						t_env_tools *env, t_norm_pwd *n)
{
	free(n->s2);
	ft_manage_oldpwd_and_prepare_pwd(*av, c_env, env, n);
	n->dash == 0 ? ft_multiple_slash_av(av) : 1;
	n->dash == 0 ? lstat(n->tmp2, &n->buf2) : lstat(*av, &n->buf2);
	if (!S_ISLNK(n->buf2.st_mode) || n->p == 0)
	{
		chdir(*av);
		getcwd(n->buf, sizeof(n->buf));
		if (n->tmp)
			free(n->tmp);
		n->tmp = ft_strjoin("PWD=", n->buf);
		ft_builtin_setenv_2(n->tmp, c_env, &(env->paths), env);
	}
	else if (S_ISLNK(n->buf2.st_mode))
	{
		chdir(*av);
		if (n->dash == 0)
			n->tmp = ft_strjoin("PWD=", n->tmp2);
		else
			n->tmp = ft_strjoin("PWD=", *av);
		ft_builtin_setenv_2(n->tmp, c_env, &(env->paths), env);
	}
}

/*
*** - Aim of the Fonction : Manage the errors of "cd -"" && "PWD && OLDPWD
*** - non set". We first manage the string properly, then we print the errors
*** - if necessary. Otherwise, execute the following part of the function
*/

int					ft_change_dir_and_pwds(char **av, char ***c_env,
						t_env_tools *env, t_norm_cd *n_cd)
{
	t_norm_pwd	n;

	n.i = 0;
	n.dash = n_cd->dash;
	n.p = n_cd->p;
	n.tmp2 = NULL;
	n.tmp = NULL;
	ft_copy_and_stat(&n, *av);
	if ((access(*av, F_OK)) == -1 && !ft_free(n.s2))
		return (ft_print_error(*av, ": No such file or directory.\n"));
	else if (ft_strcmp(*av, "..") && !S_ISDIR(n.buf2.st_mode) && !ft_free(n.s2))
		return (ft_print_dir_error("bash: cd: ", *av, ": Not a directory", 1));
	else if ((access(*av, X_OK)) == -1 && !ft_free(n.s2))
		return (ft_print_error(*av, ": Permission denied.\n"));
	else
		ft_norm_change_dir_and_pwds(av, c_env, env, &n);
	if (n.tmp2)
		free(n.tmp2);
	if (n.tmp)
		free(n.tmp);
	return (0);
}
