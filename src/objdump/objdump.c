/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** objdump.c
*/

#include <ctype.h>
#include <elf.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <tek.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

static void print_err(const char *file, const char *msg)
{
    fprintf(stderr, "my_objdump: %s: %s\n", file, msg);
}

static Elf64_Ehdr *load_elf(const char *filepath, char **buff, struct stat *s,
    int *fd)
{
    *fd = open(filepath, O_RDONLY);
    if (*fd == -1) {
        print_err(filepath, "cannot open file");
        return NULL;
    }
    fstat(*fd, s);
    *buff = mmap(NULL, s->st_size, PROT_READ, MAP_PRIVATE, *fd, 0);
    if (*buff == MAP_FAILED) {
        print_err(filepath, "map failed");
        return NULL;
    }
    if ((*buff)[0] != ELFMAG0 || (*buff)[1] != ELFMAG1 ||
        (*buff)[2] != ELFMAG2 || (*buff)[3] != ELFMAG3) {
        print_err(filepath, "file format not recognized");
        return NULL;
        }
    return (void *) *buff;
}

static bool should_print_section(const Elf64_Shdr *shdr,
    const char *shstrtab)
{
    if (shdr->sh_type == SHT_NOBITS
    || strlen(&shstrtab[shdr->sh_name]) == 0
    || strcmp(&shstrtab[shdr->sh_name], ".shstrtab") == 0
    || strcmp(&shstrtab[shdr->sh_name], ".strtab") == 0
    || strcmp(&shstrtab[shdr->sh_name], ".symtab") == 0)
        return false;
    return true;
}

static void print_section_content_hex(const unsigned char *content,
    const size_t size)
{
    size_t idx;

    for (idx = 0; idx < size; idx++) {
        if (idx % 4 == 0)
            printf(" ");
        printf("%02x", content[idx]);
    }
    for (idx = size; idx < 16; idx++) {
        if (idx % 4 == 0)
            printf(" ");
        printf("  ");
    }
}

static void print_section_content_ascii(const unsigned char *content,
    const size_t size)
{
    printf("  ");
    for (size_t idx = 0; idx < size; idx++)
        printf("%c", isprint(content[idx]) ? content[idx] : '.');
}

static void print_section(const Elf64_Shdr *shdr,
    const char *shstrtab, const char *buff)
{
    const unsigned char *content_addr =
        (unsigned char *) buff + shdr->sh_offset;

    if (!should_print_section(shdr, shstrtab))
        return;
    printf("Contents of section %s:\n", &shstrtab[shdr->sh_name]);
    for (size_t i = 0; i < shdr->sh_size; i += 16) {
        printf(" %04lx", shdr->sh_addr + i);
        if (i + 16 > shdr->sh_size) {
            print_section_content_hex(content_addr + i, shdr->sh_size - i);
            print_section_content_ascii(content_addr + i, shdr->sh_size - i);
        } else {
            print_section_content_hex(content_addr + i, 16);
            print_section_content_ascii(content_addr + i, 16);
        }
        printf("\n");
    }
}

static void iterate_sections(const Elf64_Ehdr *elf, const char *buff)
{
    const Elf64_Shdr *shdr = (Elf64_Shdr *) (buff + elf->e_shoff);
    const char *shstrtab = buff + shdr[elf->e_shstrndx].sh_offset;

    for (int i = 0; i < elf->e_shnum; i++)
        print_section(&shdr[i], shstrtab, buff);
}

static const char *get_file_format(const Elf64_Half machine_type)
{
    switch (machine_type) {
        case EM_386:
            return "i386";
        case EM_X86_64:
            return "x86-64";
        default:
            return "???";
    }
}

static int iterate_on_files(const char *const *files, const int size)
{
    int fd = -1;
    char *buff = NULL;
    struct stat s;
    const Elf64_Ehdr *elf = NULL;

    for (int i = 0; i < size; i++) {
        elf = load_elf(files[i], &buff, &s, &fd);
        if (elf == NULL)
            return EXIT_FAILURE_TEK;
        printf("\n%s:     file format elf%d-%s\n\n",
            files[i],
            ((char *)elf)[EI_CLASS] == ELFCLASS32 ? 32 : 64,
            get_file_format(elf->e_machine));
        iterate_sections(elf, buff);
        munmap(buff, s.st_size);
        close(fd);
    }
    return 0;
}

int main(const int argc, const char **argv)
{
    const char *const DEFAULT = "a.out";

    if (argc > 1)
        return iterate_on_files(&argv[1], argc - 1);
    return iterate_on_files(&DEFAULT, 1);
}
