/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 10:20:34 by droly             #+#    #+#             */
/*   Updated: 2018/04/24 13:18:40 by droly            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

void	exitstr(char *str)
{
	ft_putstr(str);
	exit(0);
}

/*pour i de 0 à 255
    S[i] := i
	finpour
	j := 0
	pour i de 0 à 255
	    j := (j + S[i] + clé[i mod longueur_clé]) mod 256
		    échanger(S[i], S[j])
	finpour*/

void	rumble_bits(char *key, char *ptr, int *tab,int size)
{
	char tmp;
	int i;
	int j;
	int p;
	int g;

	g = 0;
	p = 0;
	j = 0;
	i = 0;
	while (i + 256 < size)
	{
		while (j < 256 && i + 256 < size)
		{
			tmp = ptr[(tab[j]) * p];
			ptr[(tab[j]) * p] = ptr[(j * p)];
			ptr[(j * p)] = tmp;
			j++;
			i++;
		}
		p++;
		j = 0;
	}
	g = size - i;
	printf("size restante : %d, size : %d\n", g, size);
//	while (i < size)
//	{
		//fill ptr avec des 0 pour etre multiple de 256
//	}
	printf("test : \n");
	i = 0;
	j = 0;
	while (i + 256 < size)
	{
		while (j < 256 && i + 256 < size)
		{
			ptr[i] = ptr[i] ^ key[j];
			i++;
			j++;
		}
		j = 0;
	}
	i = 0;
	while (i < size)
	{
		printf("%c", ptr[i]);
		i++;
	}
}

void	key_schedule(char *key, void *ptr , int size)
{
	int i;
	int j;
	int tab[256];
	int tmp;

	i = 0;
	j = 0;
	while (i < 256)
	{
		tab[i] = i;
		i++;
	}
	i = 0;
	while (i < 256)
	{
		j = (j + tab[i] + key[i % 50]) % 256;
		tmp = tab[j];
		tab[j] = tab[i];
		tab[i] = tmp;
		i++;
	}
	i = 0;
	j = 0;
	while (i < 256)
	{
		if (j == tab[i])
		{
//			printf("tab[%d] : %d\n", i, tab[i]);
			j++;
			i = -1;
		}
		i++;
//		if ( i == 256 && j != 256)
//			i = 0;
	}
	rumble_bits(key, ptr, tab, size);
}


char	*create_key(void *tmp ,int size2)
{
	int fd;
	ssize_t size;
	char *key;
	unsigned char ptr[256];
	int i;
	int i2;

	i2 = 0;
	i = 0;
	key = malloc(sizeof(256));
	if ((fd = open("/dev/random", O_RDONLY)) < 0)
		exitstr("/dev/random dont open\n");
	size = read(fd, ptr, sizeof(ptr));
	printf("Key : ");
	while (i < 256)
	{
		key[i] = ptr[i] % 93 + 33;
		printf("%c", key[i]);
		i++;
	}
	ft_putchar('\n');
	key_schedule(key, tmp, size2);
	return (key);
}

void	rc4(char *ptr, int size)
{
	char *key;
	int i;
	i = 0;
	while (i < size)
	{
		printf("%c", ptr[i]);
		i++;
	}
	key = create_key(ptr, size);
}
