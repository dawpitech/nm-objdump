/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** objdump.c
*/

#include <elf.h>
#include <fcntl.h>
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

static void print_section_content(const Elf64_Shdr *shdr,
    const char *shstrtab, const char *buff)
{
    if (shdr->sh_type == SHT_NOBITS
        || strlen(&shstrtab[shdr->sh_name]) == 0
        || strcmp(&shstrtab[shdr->sh_name], ".shstrtab") == 0
        || strcmp(&shstrtab[shdr->sh_name], ".strtab") == 0
        || strcmp(&shstrtab[shdr->sh_name], ".symtab") == 0)
        return;
    printf("Contents of section %s:", &shstrtab[shdr->sh_name]);
    for (int idx = 0; idx < shdr->sh_size; idx++) {
        if (idx % 16 == 0)
            printf("\n");
        if (idx % 4 == 0)
            printf(" ");
        if (idx % 16 == 0)
            printf("%04lx ", shdr->sh_addr + idx);
        printf("%02.2x", (unsigned char) *(buff + shdr->sh_offset + idx));
    }
    printf("\n");
}

static void iterate_sections(const Elf64_Ehdr *elf, const char *buff,
    const char *filepath)
{
    const Elf64_Shdr *shdr = (Elf64_Shdr *) (buff + elf->e_shoff);
    const char *shstrtab = buff + shdr[elf->e_shstrndx].sh_offset;

    for (int i = 0; i < elf->e_shnum; i++)
        print_section_content(&shdr[i], shstrtab, buff);
}

static int iterate_on_files(const char *const *files, const int size)
{
    int fd = -1;
    char *buff = NULL;
    struct stat s;
    const Elf64_Ehdr *elf = NULL;

    for (int i = 0; i < size; i++) {
        printf("\n%s:     file format ???\n\n", files[i]);
        elf = load_elf(files[i], &buff, &s, &fd);
        if (elf == NULL)
            return EXIT_FAILURE_TEK;
        iterate_sections(elf, buff, files[i]);
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
