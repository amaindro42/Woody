/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 10:20:34 by droly             #+#    #+#             */
/*   Updated: 2018/04/20 16:57:48 by droly            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

void	exitstr(char *str)
{
	ft_putstr(str);
	exit(0);
}


char	*create_key()
{
	int fd;
	ssize_t size;
	char *key;
	unsigned char ptr[50];
	int i;
	int i2;

	i2 = 0;
	i = 0;
	key = malloc(sizeof(50));
	if ((fd = open("/dev/random", O_RDONLY)) < 0)
		exitstr("/dev/random dont open\n");
	size = read(fd, ptr, sizeof(ptr));
	ft_printf("Key : ");
	while (i < 50)
	{
		key[i] = ptr[i] % 93 + 33;
		ft_printf("%c", key[i]);
		i++;
	}
	ft_putchar('\n');
	return (key);
}

void	rc4(void *ptr)
{
	char *key;

	key = create_key();
}
