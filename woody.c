/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 11:28:09 by droly             #+#    #+#             */
/*   Updated: 2018/05/09 15:54:47 by amaindro         ###   ########.fr       */
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

char			*create_opcode(char *key, size_t size_file, Elf64_Word jump, size_t code_size, size_t padding)
{
	char	*code;
	char	base_code[] = "\x68\x2e\x0a\x00\x00" //push "....WOODY....\n"
	/* 05*/				"\x68\x59\x2e\x2e\x2e"
	/* 10*/				"\x68\x57\x4f\x4f\x44"
	/* 15*/				"\x68\x2e\x2e\x2e\x2e"
	/* 20*/				"\x48\x89\xe6" //mov rsi, rsp
	/* 23*/				"\x48\xc7\xc7\x01\x00\x00\x00" //mov rdi, 1
	/* 30*/				"\x48\xc7\xc0\x01\x00\x00\x00" //mov rax, 1
	/* 37*/				"\x48\xc7\xc2\x1c\x00\x00\x00" //mov rdx, 14
	/* 44*/				"\x0f\x05" //Syscall
						//decryption
	/* 46*/				"\x58" // pop rax
	/* 47*/				"\x58" // pop rax
	/* 48*/				"\x58" // pop rax
	/* 49*/				"\x58" // pop rax
	/* 50*/				"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x68\x42\x42\x42\x42" // push placeholder for encryption key
						"\x48\x89\xe6" //mov rsi, rsp
						"\x48\xc7\xc0\x10\x04\x40\x00" //store entrypoint in rax
						"\x48\x33\xc9" //xor rcx, rcx
						"\x48\xc7\xc2\x00\x02\x00\x00" //mov rdx, 512
						//loop
	/* 01*/				"\x90"
	/* 04*/				"\x48\x8b\x1e" //mov rbx, ptr[rsi]
	/* 06*/				"\x31\x18" //xor WORD ptr[rax], ebx
	/* 13*/				"\x48\x81\xc0\x04\x00\x00\x00" //add rax, 4
	/* 20*/				"\x48\x81\xc6\x08\x00\x00\x00" //add rsi, 8
	/* 27*/				"\x48\x81\xc1\x08\x00\x00\x00" //add rcx, 8
	/* 30*/				"\x48\x39\xd1" //cmp rsi, rdx
	/* 32*/				"\x7c\x0e" //jmp 14
	/* 39*/				"\x48\x81\xc2\x00\x02\x00\x00" //add rdx, 512
	/* 46*/				"\x48\x81\xee\x00\x02\x00\x00" //sub rdi, 512

	/* 53*/				"\x48\x81\xf9\x00\x00\x00\x00" //cmp rcx, 0
	/* 55*/				"\x7c\xc9" //jmp -55
						//clean up
	/* 03*/				"\x48\x33\xf6" //xor rsi, rsi
	/* 06*/				"\x48\x33\xff" //xor rdi, rdi
						"\x48\x31\xc0"; //xor rax, rax
	int		i;
	int		j;

	code = ft_memalloc(sizeof(char) * PAGE_SIZE);
	ft_strncpy(code, base_code, code_size);
	i = 1;
	j = 0;
	while (i <= 64) //push of key in stack
	{
		*(int*)(code + 50 + i + j) = *(int*)(key - j + 252);
		j += 4;
		i++;
	}

	*(int*)(code + code_size - 15) = size_file * 2;

	code[code_size] = '\xe9'; //jump to main
	*(int*)(code + code_size + 1) = jump;
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

void			update_section_64(Elf64_Ehdr *header, Elf64_Off offset, size_t *crypt_offset, size_t *crypt_size)
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
		if (ft_strcmp(elf_lookup_string(header, section->sh_name), ".text") == 0)
		{
			*crypt_offset = section->sh_offset;
			*crypt_size = section->sh_size;
		}
	}
}

char			*Elf64(void *ptr, size_t *size, char *key, size_t *crypt_offset, size_t *crypt_size, int **tab, int **tab_rest)
{
	char		*code;
	char		*str;
	int			i_p;
	int			i_s;
	size_t		code_size;
	size_t		tmp_size;
	size_t		padding_size;
	Elf64_Ehdr	*header;
	Elf64_Phdr	*program;
	Elf64_Shdr	*section;
	Elf64_Addr	tmp_entry;

	header = ptr;

	//Patch the insertion code (parasite) to jump to the entry point (original)
	tmp_entry = header->e_entry;

	//Locate the text segment program header
	i_p = 0;
	while (i_p < header->e_phnum)
	{
		program = elf_program(header, i_p++);
		//printf("type = %d, flag = %d\noffset = %llu\n", program->p_type, program->p_flags, program->p_offset);
		if (program->p_type == PT_LOAD && program->p_flags & PF_X)
			break ;
	}
	tmp_size = program->p_offset + program->p_filesz;

	//Modify the entry point of the ELF header to point to the new code (p_vaddr + p_filesz)
	header->e_entry = program->p_vaddr + program->p_filesz;

	//printf("jump = %llx\n", (header->e_entry - tmp_entry + code_size - 1) ^ 0xffffffff);

	//change text segment access rights to be able to decrypt it later
	program->p_flags = program->p_flags | PF_W;

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
	update_section_64(header, section->sh_offset, crypt_offset, crypt_size);
	header->e_shoff += PAGE_SIZE;
	//printf("p_offset = %llx\nsh_offset = %llx\n", elf_program(header, i_p)->p_offset, section->sh_offset);

	//Create rumble_bits index table
	key_schedule(key, (*crypt_size - *crypt_size % 256), (*crypt_size % 256), tab, tab_rest);

	code_size = 46 + (24 + 64 * 5) + 55 + 14;
	code = create_opcode(key, *crypt_size, (header->e_entry - tmp_entry + code_size - 1) ^ 0xffffffff, code_size - 5, PAGE_SIZE);

	str = ft_memalloc(*size + PAGE_SIZE);
	ft_strncpy(str, ptr, tmp_size);
	//Physically insert the new code (parasite) and pad to PAGE_SIZE, into the file - text segment p_offset + p_filesz (original)
	ft_strncpy(str + tmp_size, code, PAGE_SIZE);
	ft_strncpy(str + tmp_size + PAGE_SIZE, ptr + tmp_size, *size - tmp_size);
	*size += PAGE_SIZE;
	return (str);
}

void			magic_number(void *ptr, size_t size, char *file_name)
{
	char		*str;

	char		*key;
	size_t		crypt_offset;
	size_t		crypt_size;
	int			i;

	str = ptr;
	if (*(int *)ptr == *(int *)ELFMAG && str[EI_CLASS] == ELFCLASS64)
	{
		/*str = Elf64(ptr, &size, key, &crypt_offset, &crypt_size);
		i = 0;
		while (i < size)
		{
			printf("%c", str[i]);
			i++;
		}*/
		rc4(str, size);
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
