/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** nm.c
*/

#include <ctype.h>
#include <elf.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "tek.h"

static char get_symbol_ex(const Elf64_Sym symbol, const Elf64_Shdr *shdr,
    const bool is_global)
{
    const Elf64_Word sh_type = shdr[symbol.st_shndx].sh_type;
    const Elf64_Word sh_flags = shdr[symbol.st_shndx].sh_flags;

    if (sh_type == SHT_NOBITS)
        return is_global ? 'B' : 'b';
    if (sh_type == SHT_FINI_ARRAY && sh_flags == (SHF_ALLOC | SHF_WRITE))
        return is_global ? 'D' : 'd';
    if (sh_type == SHT_INIT_ARRAY && sh_flags == (SHF_ALLOC | SHF_WRITE))
        return is_global ? 'D' : 'd';
    if (sh_type == SHT_PROGBITS && sh_flags == (SHF_ALLOC | SHF_WRITE))
        return is_global ? 'D' : 'd';
    if (sh_type == SHT_DYNAMIC)
        return is_global ? 'D' : 'd';
    if (sh_type == SHT_PROGBITS && sh_flags == SHF_ALLOC)
        return is_global ? 'R' : 'r';
    if (sh_type == SHT_PROGBITS)
        return is_global ? 'T' : 't';
    if (sh_flags != SHF_WRITE)
        return is_global ? 'R' : 'r';
    return '?';
}

static char get_symbol(const Elf64_Sym symbol, const Elf64_Shdr *shdr)
{
    const unsigned char symbol_bind = ELF64_ST_BIND(symbol.st_info);
    const bool is_global = symbol_bind == STB_GLOBAL;

    if (symbol_bind == STB_WEAK)
        return symbol.st_value == SHN_UNDEF ? 'w' : 'W';
    if (symbol.st_shndx == SHN_UNDEF)
        return 'U';
    return get_symbol_ex(symbol, shdr, is_global);
}

static void print_symbol_info(const Elf64_Sym symbol, const Elf64_Shdr *shdr,
    const char *sym_strtab, const bool is_relocatable)
{
    if (*(sym_strtab + symbol.st_name) == '\0' || symbol.st_info == STT_FILE)
        return;
    if (symbol.st_value == 0 && !is_relocatable) {
        printf("%16c %c %s\n", ' ', get_symbol(symbol, shdr),
            sym_strtab + symbol.st_name);
        return;
    }
    printf("%016lx %c %s\n", symbol.st_value, get_symbol(symbol, shdr),
        sym_strtab + symbol.st_name);
}

static void print_err(const char *file, const char *msg)
{
    fprintf(stderr, "my_nm: '%s': %s\n", file, msg);
}

static Elf64_Ehdr *load_elf(const char *filepath, char **buff, struct stat *s,
    int *fd)
{
    *fd = open(filepath, O_RDONLY);
    if (*fd == -1) {
        print_err(filepath, "No such file");
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

static void iterate_symbols(const Elf64_Ehdr *elf, const char *buff,
    const char *filepath)
{
    const Elf64_Shdr *shdr = (Elf64_Shdr *) (buff + elf->e_shoff);
    const Elf64_Shdr *sym_table = NULL;
    const Elf64_Shdr *symtab = NULL;
    const char *sym_strtab = NULL;
    const Elf64_Sym *sym = NULL;

    for (int i = 0; i < elf->e_shnum; i++)
        if (shdr[i].sh_type == SHT_SYMTAB)
            sym_table = &shdr[i];
    if (sym_table == NULL) {
        print_err(filepath, "Couldn't find symbol table in given file.\n");
        return;
    }
    symtab = &shdr[sym_table->sh_link];
    sym_strtab = buff + symtab->sh_offset;
    sym = (Elf64_Sym *) (buff + sym_table->sh_offset);
    for (long unsigned i = 0; i < sym_table->sh_size / sizeof(Elf64_Sym); i++)
        print_symbol_info(sym[i], shdr, sym_strtab, elf->e_type == ET_REL);
}

static int iterate_on_files(const char *const *files, const int size)
{
    int fd = -1;
    char *buff = NULL;
    struct stat s;
    const Elf64_Ehdr *elf = NULL;

    for (int i = 0; i < size; i++) {
        if (size > 1)
            printf("\n%s:\n", files[i]);
        elf = load_elf(files[i], &buff, &s, &fd);
        if (elf == NULL)
            return EXIT_FAILURE_TEK;
        iterate_symbols(elf, buff, files[i]);
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
