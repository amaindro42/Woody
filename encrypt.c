/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 10:20:34 by droly             #+#    #+#             */
/*   Updated: 2018/04/26 17:16:03 by droly            ###   ########.fr       */
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

void	rumble_bits(char *key, char *ptr, int tab[256], size_t size)
{
	char tmp;
	int i;
	int j;
	int p;

	i = 0;
//encrypt :
	while (i < size)
	{
		j = 0;
		while (j < 256 && i  < size)
		{
			tmp = ptr[(tab[j]) + i - j];
			ptr[(tab[j]) + i - j] = ptr[i];
			ptr[i] = tmp;
			j++;
			i++;
		}
	}
	printf("\n___________________________________________________________________________________________\n\n");
	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < 256 && i < size)
		{
//			printf("compare : %c ^ %c", ptr[i], key[j]);
			ptr[i] = ptr[i] ^ key[j];
//			printf(" = %c\n", ptr[i]);
			i++;
			j++;
		}
	}
	i = 0;
	while (i < size)
	{
		printf("%c", ptr[i]);
		i++;
	}
	printf("\n___________________________________________________________________________________________\n\n");
// reverse encrypt 
	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < 256 && i < size)
		{
//			printf("compare : %c ^ %c", ptr[i], key[j]);
			ptr[i] = ptr[i] ^ key[j];
//			printf(" = %c\n", ptr[i]);
			i++;
			j++;
		}
	}
	i = size - 1;
	while (i >= 0)
	{
		j = 255;
		while (j >= 0 && i >= 0)
		{
			tmp = ptr[(tab[j]) + i - j];
			ptr[(tab[j]) + i - j] = ptr[i];
			ptr[i] = tmp;
			j--;
			i--;
		}
	}
	i = 0;
	while (i < size)
	{
		printf("%c", ptr[i]);
		i++;
	}
}

void	key_schedule(char *key, void *ptr, size_t size)
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


char	*create_key(char *str, size_t size_file)
{
	int				fd;
	int				i;
	ssize_t			size_key;
	size_t			global_size;
	size_t			crypt_size;
	unsigned char	ptr[256];
	char			*key;
	char			*tmp2;
	char			*crypt;

	key = malloc(sizeof(256));
	if ((fd = open("/dev/random", O_RDONLY)) < 0)
		exitstr("/dev/random dont open\n");
	size_key = read(fd, ptr, sizeof(ptr));
	i = 0;
//	printf("Key : ");
	while (i < 256)
	{
		key[i] = ptr[i] % 93 + 33;
//		printf("%c", key[i]);
		i++;
	}
	str = Elf64(str, size_file, &size_file, &crypt, &crypt_size);
	global_size = (size_file % 256 == 0) ? size_file : (size_file / 256 + 1) * 256;
	tmp2 = malloc(global_size);
	i = 0;
	while (i < size_file)
	{
		tmp2[i] = str[i];
		i++;
	}
	while (i < global_size)
	{
		tmp2[i] = 0;
		i++;
	}
	ft_putchar('\n');
	key_schedule(key, tmp2, global_size);
	return (key);
}

void	rc4(char *ptr, size_t size)
{
	char *key;
	int i;
	i = 0;
	printf("\n");
	while (i < size)
	{
		printf("%c", ptr[i]);
		i++;
	}
	printf("hey2\n");
	key = create_key(ptr, size);
}
