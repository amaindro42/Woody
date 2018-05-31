/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: droly <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 11:28:09 by droly             #+#    #+#             */
/*   Updated: 2018/05/31 16:01:51 by amaindro         ###   ########.fr       */
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

char			*create_opcode(Elf64_Addr entrypoint, Elf64_Addr tmp_entry, char *key, Elf64_Addr text_addr,
		size_t crypt_size, size_t code_size, int *tab, int *tab_rest)
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
	/* 50*/				"\x48\xc7\xc6\x00\x00\x00\x00" //mov rsi, 0 <- placeholder
						"\x48\xc7\xc0\x00\x00\x00\x00" //mov rax, 0 <- placeholder
						"\x48\x33\xc9" //xor rcx, rcx
						"\x48\xc7\xc2\x00\x01\x00\x00" //mov rdx, 256

//loop xor
	/* 01*/				"\x90"
	/* 04*/				"\x48\x8b\x1e" //mov rbx, ptr[rsi]
	/* 06*/				"\x31\x18" //xor WORD ptr[rax], ebx
	/* 13*/				"\x48\x81\xc0\x04\x00\x00\x00" //add rax, 4
	/* 20*/				"\x48\x81\xc6\x04\x00\x00\x00" //add rsi, 4
	/* 27*/				"\x48\x81\xc1\x04\x00\x00\x00" //add rcx, 4
	/* 30*/				"\x48\x39\xd1" //cmp rsi, rdx
	/* 32*/				"\x7c\x0e" //jl 14
	/* 39*/				"\x48\x81\xc2\x00\x01\x00\x00" //add rdx, 256
	/* 46*/				"\x48\x81\xee\x00\x01\x00\x00" //sub rsi, 256

	/* 53*/				"\x48\x81\xf9\x00\x00\x00\x00" //cmp rcx, 0 <- placeholder
	/* 55*/				"\x7c\xc9" //jl -55

//loop rumble bits rest
	/* 07*/				"\x48\xc7\xc6\x00\x00\x00\x00" //mov rsi, 0 <- placeholder (rumble tab address)
						//mov rdi, rax - (crypt_size % 256)
	/* 10*/				"\x48\x89\xc7" //mov rdi, rax
	/* 17*/				"\x48\x81\xef\x00\x00\x00\x00" //sub rdi, 0 <- placeholder (end condition)

	/* 18*/				"\x90"
	/* 25*/				"\x48\x81\xe8\x01\x00\x00\x00" //sub rax, 1
	/* 28*/				"\x48\x31\xdb" //xor rbx, rbx
	/* 30*/				"\x8a\x1e" //mov rbx, BYTE ptr[rsi] <- get position
	/* 33*/				"\x48\x31\xd2" //xor rdx, rdx
	/* 35*/				"\x8a\x10" //mov rdx, BYTE ptr[rax] <- store [rax] in tmp
	/* 38*/				"\x48\x01\xfb" //add rbx, rdi
	/* 40*/				"\x8b\x0b" //mov rcx, ptr[rbx]
	/* 42*/				"\x88\x08" //mov BYTE ptr[rax], rcx
	/* 44*/				"\x88\x13" //mov BYTE ptr[rbx], rdx
	/* 51*/				"\x48\x81\xc6\x01\x00\x00\x00" //add rsi, 1
	/* 54*/				"\x48\x39\xf8" //cmp rax, rdi
	/* 56*/				"\x7f\xd9" //jg -39

//loop rumble bits
	/* 07*/				"\x48\xc7\xc6\x00\x00\x00\x00" //mov rsi, 0 <- placeholder (rumble tab address)
	/* 10*/				"\x49\x89\xf9" //mov r9, rdi
	/* 17*/				"\x49\x81\xe9\x00\x00\x00\x00" //sub r9, 0 <- placeholder (end condition)
	/* 24*/				"\x48\x81\xef\x00\x01\x00\x00" //sub rdi, 256
	/* 31*/				"\x49\xc7\xc0\x00\x00\x00\x00" //mov r8, 0

	/* 32*/				"\x90"
	/* 58*/				"\x48\x81\xe8\x01\x00\x00\x00" //sub rax, 1
	/* 35*/				"\x48\x31\xdb" //xor rbx, rbx
	/* 37*/				"\x8a\x1e" //mov rbx, BYTE ptr[rsi] <- get position
	/* 40*/				"\x48\x31\xd2" //xor rdx, rdx
	/* 42*/				"\x8a\x10" //mov rdx, BYTE ptr[rax] <- store [rax] in tmp
	/* 45*/				"\x48\x01\xfb" //add rbx, rdi
	/* 47*/				"\x8b\x0b" //mov rcx, ptr[rbx]
	/* 49*/				"\x88\x08" //mov BYTE ptr[rax], rcx
	/* 51*/				"\x88\x13" //mov BYTE ptr[rbx], rdx
	/* 65*/				"\x48\x81\xc6\x01\x00\x00\x00" //add rsi, 1
	/* 69*/				"\x49\x83\xc0\x01" //add r8, 1

	/* 76*/				"\x49\x81\xf8\x00\x01\x00\x00"
	/* 78*/				"\x7c\x15" //jl 21
	/* 85*/				"\x48\x81\xee\x00\x01\x00\x00" //sub rsi, 256
	/* 92*/				"\x49\xc7\xc0\x00\x00\x00\x00" //mov r8, 0
	/* 99*/				"\x48\x81\xef\x00\x01\x00\x00" //sub rdi, 256

	/*102*/				"\x4c\x39\xc8" //cmp rax, r9
	/*104*/				"\x7f\xb7" //jg -73

//clean up
	/* 03*/				"\x48\x33\xf6" //xor rsi, rsi
	/* 06*/				"\x48\x33\xff" //xor rdi, rdi
	/* 09*/				"\x48\x31\xc0" //xor rax, rax
	/* 12*/				"\x48\x31\xc9" //xor rcx, rcx
	/* 15*/				"\x48\x31\xd2" //xor rdx, rdx
	/* 18*/				"\x48\x31\xdb"; //xor rbx, rbx
	unsigned int		j;

	code = ft_memalloc(sizeof(char) * PAGE_SIZE);
	ft_strncpy(code, base_code, code_size);

	*(int *)(code + 50 + 3) = entrypoint + code_size + 5; //set key address

	*(int *)(code + code_size - 18 - 53 - 104) = entrypoint + code_size + 5 + 256; //set rumble tab_rest address
	*(int *)(code + code_size - 18 - 43 - 104) = crypt_size % 256; //set tab_rest begin

	*(int *)(code + code_size - 18 - 101) = entrypoint + code_size + 5 + 256 + (crypt_size % 256); //set rumble tab address
	*(int *)(code + code_size - 18 - 91) = crypt_size - (crypt_size % 256); //set tab begin

	*(int *)(code + 57 + 3) = text_addr; //set .text section address

	j = 0;
	while (j < 256) //set key at the end of opcode
	{
		*(int*)(code + code_size + 5 + j) = *(int*)(key + j);
		j += 4;
	}

	j = 1;
	while (j <= crypt_size % 256) //set rumble tab at the end of opcode
	{
		*(char*)(code + code_size + 5 + 256 + (j - 1)) = tab_rest[(crypt_size % 256) - j];
		j++;
	}

	j = 1;
	while (j <= 256) //set rumble tab at the end of opcode
	{
		*(char*)(code + code_size + 5 + 256 + (crypt_size % 256) + (j - 1)) = tab[256 - j];
		j++;
	}

	*(int*)(code + code_size - 24 - 56 - 104) = crypt_size; //set cmp for xor loop

	code[code_size] = '\xe9'; //jump to main
	*(int*)(code + code_size + 1) = (entrypoint - tmp_entry + code_size + 4) ^ 0xffffffff;
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

Elf64_Addr		update_section_64(Elf64_Ehdr *header, Elf64_Off offset, size_t *crypt_offset, size_t *crypt_size)
{
	Elf64_Shdr	*section;
	Elf64_Addr	ptr;
	int			i;

	i = 0;
	ptr = 0;
	while (i < header->e_shnum)
	{
		section = elf_section(header, i++);
		if (ft_strcmp(elf_lookup_string(header, section->sh_name), ".text") == 0)
		{
			*crypt_offset = section->sh_offset;
			*crypt_size = section->sh_size;
			ptr = section->sh_addr;
		}
		if (section->sh_offset > offset)
		{
			section->sh_offset += PAGE_SIZE;
		}
	}
	return (ptr);
}

char			*Elf64(void *ptr, size_t *size, char *key, size_t *crypt_offset, size_t *crypt_size, int **tab, int **tab_rest)
{
	char		*code;
	char		*str;
	int			i_p;
	int			i_s;
	size_t		code_size;
	size_t		tmp_size;
	Elf64_Ehdr	*header;
	Elf64_Phdr	*program;
	Elf64_Shdr	*section;
	Elf64_Addr	tmp_entry;
	Elf64_Addr	text_addr;

	header = ptr;

	//Patch the insertion code (parasite) to jump to the entry point (original)
	tmp_entry = header->e_entry;

	//Locate the text segment program header
	i_p = 0;
	while (i_p < header->e_phnum)
	{
		program = elf_program(header, i_p++);
		if (program->p_type == PT_LOAD && program->p_flags & PF_X)
			break ;
	}
	tmp_size = program->p_offset + program->p_filesz;

	//Modify the entry point of the ELF header to point to the new code (p_vaddr + p_filesz)
	header->e_entry = program->p_vaddr + program->p_filesz;

	//change text segment access rights to be able to decrypt it later
	program->p_flags = program->p_flags | PF_W;

	//Increase p_filesz by account for the new code (parasite)
	program->p_filesz += PAGE_SIZE;
	//Increase p_memsz to account for the new code (parasite)
	program->p_memsz += PAGE_SIZE;

	//For the last shdr in the text segment
	i_s = 0;
	while (i_s < header->e_shnum)
	{
		section = elf_section(header, i_s);
		if (elf_section(header, i_s + 1)->sh_addr > header->e_entry)
		{
	//increase sh_len by the parasite length
			section->sh_size += PAGE_SIZE;
			break ;
		}
		i_s++;
	}

	update_segment_64(header, elf_program(header, i_p)->p_offset);
	text_addr = update_section_64(header, section->sh_offset, crypt_offset, crypt_size);
	header->e_shoff += PAGE_SIZE;

	//Create rumble_bits index table
	key_schedule(key, (*crypt_size - *crypt_size % 256), (*crypt_size % 256), tab, tab_rest);

	code_size = 46 + 28 + 55 + 56 + 104 + 23;
	code = create_opcode(header->e_entry, tmp_entry, key, text_addr, *crypt_size, code_size - 5, *tab, *tab_rest);

	str = ft_memalloc(*size + PAGE_SIZE);
	ft_strncpy(str, ptr, tmp_size);
	//Physically insert the new code (parasite) and pad to PAGE_SIZE, into the file - text segment p_offset + p_filesz (original)
	ft_strncpy(str + tmp_size, code, PAGE_SIZE);
	ft_strncpy(str + tmp_size + PAGE_SIZE, ptr + tmp_size, *size - tmp_size);
	*size += PAGE_SIZE;
	return (str);
}

void			magic_number(void *ptr, size_t size)
{
	char		*str;

	str = ptr;
	if (*(int *)ptr == *(int *)ELFMAG && str[EI_CLASS] == ELFCLASS64)
	{
		rc4(str, size);
	}
	else
		ft_putstr("Wrong file signature\n");
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
	magic_number(ptr, size);
	if (munmap(ptr, size) < 0)
		return (0);
}
