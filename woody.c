/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 11:28:09 by droly             #+#    #+#             */
/*   Updated: 2018/04/24 13:12:06 by amaindro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

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

char			*create_opcode(char *str, size_t code_size, size_t padding)
{
	char	*code;

	code = ft_memalloc(sizeof(char) * PAGE_SIZE);
	ft_strncpy(code, str, code_size);
	return (code);
}

void			update_segment_64(Elf64_Ehdr *header, Elf64_Off offset)
{
	Elf64_Phdr	*program;
	int			i;

	i = 0;
	while (i < header->e_phnum)
	{
		program = elf_program(header, i++);
		if (program->p_offset >= offset)
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
	char		*code;
	size_t		code_size;
	int			i_p;
	int			i_s;
	Elf64_Ehdr	*header;
	Elf64_Phdr	*program;
	Elf64_Shdr	*section;
	int			fd;
	Elf64_Addr	tmp_entry;
	size_t		tmp_size;
	size_t		padding_size;

	code_size = 57;
	code = create_opcode("\x68\x2e\x0a\x00\x00" //push "....WOODY....\n"
						"\x68\x59\x2e\x2e\x2e"
						"\x68\x57\x4f\x4f\x44"
						"\x68\x2e\x2e\x2e\x2e"
						"\x48\x89\xe6" //mov rsi, rsp
						"\x48\xc7\xc7\x01\x00\x00\x00" //mov rdi, 1
						"\x48\xc7\xc0\x01\x00\x00\x00" //mov rax, 1
						"\x48\xc7\xc2\x1c\x00\x00\x00" //mov rdx, 14
						"\x0f\x05" //Syscall
						"\x48\x33\xc0" //xor rax, rax
						"\x48\x33\xd2" //xor rdx, rdx
						"\xe9\xfb\xfc\xff\xff", code_size, PAGE_SIZE); //jump to main

	header = ptr;

	//Patch the insertion code (parasite) to jump to the entry point (original)
	tmp_entry = header->e_entry;

	//Locate the text segment program header
	i_p = 0;
	while (i_p < header->e_phnum)
	{
		program = elf_program(header, i_p++);
		printf("type = %d, flag = %d\noffset = %llu\n", program->p_type, program->p_flags, program->p_offset);
		if (program->p_type == PT_LOAD && program->p_flags & PF_X)
			break ;
	}
	tmp_size = program->p_offset + program->p_filesz;

	//Modify the entry point of the ELF header to point to the new code (p_vaddr + p_filesz)
	header->e_entry = program->p_vaddr + program->p_filesz;

	printf("jump = %llx\n", (header->e_entry - tmp_entry + code_size - 1) ^ 0xffffffff);

	//Increase p_filesz by account for the new code (parasite)
	program->p_filesz += code_size;
	//Increase p_memsz to account for the new code (parasite)
	program->p_memsz += code_size;

	//For the last shdr in the text segment
	i_s = 0;
	while (i_s < header->e_shnum)
	{
		section = elf_section(header, i_s);
		if (elf_section(header, i_s + 1)->sh_addr > header->e_entry)
		{
	//increase sh_len by the parasite length
			section->sh_size += code_size;
			break ;
		}
		i_s++;
	}

	update_segment_64(header, elf_program(header, i_p)->p_offset);
	update_section_64(header, section->sh_offset);
	header->e_shoff += PAGE_SIZE;
	printf("p_offset = %llx\nsh_offset = %llx\n", elf_program(header, i_p)->p_offset, section->sh_offset);

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
		rc4(ptr);
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
