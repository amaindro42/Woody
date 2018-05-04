/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 10:20:34 by droly             #+#    #+#             */
/*   Updated: 2018/05/04 16:17:23 by droly            ###   ########.fr       */
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

void	rumble_bits(char *key, char *ptr, int tab[256], int tab_rest[256], size_t size, size_t size_file, char *str, size_t rest_size)
{
	char tmp;
	int i;
	int j;
	int p;


//encrypt :
	i = 0;
//	printf("yomdr size : %lu, rest_szie : %lu, string : >%s<\n", size, rest_size, ptr);
	while (i < size && size >= 256)
	{
		j = 0;
		while (j < 256 && i  < size)
		{
//			printf("mdr : size : %lu, tab[j]: %d, i : %d, j : %d\n", size, tab[j], i , j);
//			printf("lol\n");
//			printf("tmp : >%c<\n", ptr[0]);
//			printf("lil\n");
			tmp = ptr[(tab[j]) + i - j];
//			printf("tmp : >%c<\n", tmp);
//			ptr[32] = ptr[i];
//			printf("ptr[i] : >%c<\n", ptr[i]);
//			printf("ptr[tab[j]] : >%c<\n", ptr[(tab[j]) + i - j]);
//			printf("mdr2\n");
			ptr[(tab[j]) + i - j] = ptr[i];
//			printf("mdr3\n");
			ptr[i] = tmp;
			j++;
			i++;
		}
	}
	while (i < rest_size + size)
	{
//		printf("eh ca rentre\n");
		j = 0;
		while (j < rest_size)
		{
//			printf("mdr : size : %lu, tab[j]: %d, i : %d, j : %d\n", size, tab[j], i , j);
			tmp = ptr[(tab_rest[j]) + i - j];
//			printf("mdr2\n");
			ptr[(tab_rest[j]) + i - j] = ptr[i];
//			printf("mdr3\n");
			ptr[i] = tmp;
			j++;
			i++;
		}
	}
	printf("\n___________________________________________________________________________________________\n\n");
	i = 0;
	while (i < size && size >= 256)
	{
//		printf("loluilol\n");
		j = 0;
		while (j < 256 && i < size)
		{
//			printf("compare : %c ^ %c", ptr[i], key[j]);
//			printf("mdr\n");
			ptr[i] = ptr[i] ^ key[j];
//			printf("yo\n");
//			printf(" = %c\n", ptr[i]);
			i++;
			j++;
		}
	}
//	printf("loluilol\n");
	while (i < rest_size + size)
	{
		j = 0;
		while (j < rest_size)
		{
//			printf("compare : %c ^ %c", ptr[i], key[j]);
			ptr[i] = ptr[i] ^ key[j];
//			printf(" = %c\n", ptr[i]);
			i++;
			j++;
		}
	}
	i = 0;
//	printf("size_file: %zu >\n", size_file);
	while (i < size_file)
	{
//		printf("wesh\n");
		printf("%c", str[i]);
		i++;
	}
//	printf("<\n");
	printf("\n___________________________________________________________________________________________\n\n");
// reverse encrypt 
	i = 0;
	while (i < size && size >= 256)
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
	while (i < rest_size + size)
	{
		j = 0;
		while (j < rest_size)
		{
//			printf("compare : %c ^ %c", ptr[i], key[j]);
			ptr[i] = ptr[i] ^ key[j];
//			printf(" = %c\n", ptr[i]);
			i++;
			j++;
		}
	}
	i = rest_size + size - 1;
	while (i >= size)
	{
		j = rest_size - 1;
		while (j >= 0 && i >= 0)
		{
			tmp = ptr[(tab_rest[j]) + i - j];
			ptr[(tab_rest[j]) + i - j] = ptr[i];
			ptr[i] = tmp;
			j--;
			i--;
		}
	}
	i = size - 1;
	while (i >= 0 && size >= 256)
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
	while (i < size_file)
	{
		printf("%c", str[i]);
		i++;
	}
}

void	key_schedule(char *key, void *ptr, size_t size, size_t size_file, void *str, size_t rest_size)
{
	int i;
	int j;
	int tab[256];
	int tab_rest[256];
	int tmp;

	i = 0;
	j = 0;
//	printf("yomdr\n");
	while (i < 256 && size >= 256)
	{
		tab[i] = i;
		i++;
	}
	i = 0;
	while (i < rest_size)
	{
		tab_rest[i] = i;
		i++;
	}
	i = 0;
	while (i < 256 && size >= 256)
	{
		j = (j + tab[i] + key[i % 256]) % 256;
		tmp = tab[j];
		tab[j] = tab[i];
		tab[i] = tmp;
		i++;
	}
	i = 0;
	j = 0;
	while (i < rest_size)
	{
		j = (j + tab_rest[i] + key[i % 256]) % rest_size;
		tmp = tab_rest[j];
		tab_rest[j] = tab_rest[i];
		tab_rest[i] = tmp;
		i++;
	}
/*	i = 0;
	j = 0;
	while (i < rest_size)
	{
		if (j == tab_rest[i])
		{
			printf("tab_rest[%d] : %d\n", i, tab_rest[i]);
			j++;
			i = -1;
		}
		i++;
		if ( i == 256 && j != 256)
			i = 0;
	}
	i = 0;
	j = 0;
	while (i < size)
	{
		if (j == tab[i])
		{
			printf("tab[%d] : %d\n", i, tab[i]);
			j++;
			i = -1;
		}
		i++;
		if ( i == 256 && j != 256)
			i = 0;
	}*/
	rumble_bits(key, ptr, tab, tab_rest,  size, size_file, str, rest_size);
}


char	*create_key(char *str, size_t size_file)
{
	int				fd;
	int				i;
	ssize_t			size_key;
	size_t			rest_size;
	size_t			crypt_size;
	size_t			crypt_offset;
	unsigned char	ptr[256];
	char			*key;
	char			*tmp2;
	char			*crypt;

//	printf("yomdr\n");
	key = ft_memalloc(sizeof(char) * 256);
//	printf("yomdr2\n");
	if ((fd = open("/dev/random", O_RDONLY)) < 0)
		exitstr("/dev/random dont open\n");
//	printf("yomdr3\n");
	size_key = read(fd, ptr, sizeof(ptr));
//	printf("yomdr4\n");
	i = 0;
	printf("Key : ");
	while (i < 256)
	{
		key[i] = ptr[i] % 93 + 33;
		printf("%c", key[i]);
		i++;
	}
//	printf("\n");
	str = Elf64(str, &size_file, key, &crypt_offset, &crypt_size);
	tmp2 = malloc(size_file);
	i = 0;
	while (i < size_file)
	{
		tmp2[i] = str[i];
		i++;
	}
//	crypt_offset = 5;
//	crypt_size = size_file - 12;
//	printf("size crypt off : %lu\n", crypt_offset);
	crypt = tmp2 + crypt_offset;
//	printf("str = %p, crypt = %p >%c<\n", str, crypt, crypt[200]);
	rest_size = crypt_size % 256;
	crypt_size -= rest_size;
//	printf("size crypt  : %lu, rest : %lu\n", crypt_size, rest_size);
/*	i = 0;
	while (i < size_file)
	{
		tmp2[i] = str[i];
		i++;
	}
	while (i < global_size)
	{
		tmp2[i] = 0;
		i++;
	}*/
	ft_putchar('\n');
	key_schedule(key, crypt, crypt_size, size_file, tmp2, rest_size);
	return (key);
}

void	rc4(char *ptr, size_t size)
{
	char *key;
	int i;
	i = 0;
//	printf("\n");
	while (i < size)
	{
		printf("%c", ptr[i]);
		i++;
	}
//	printf("hey2lol\n");
	key = create_key(ptr, size);
}
