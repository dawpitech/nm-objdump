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

#include "objdump.h"

static void print_err(const char *file, const char *msg)
{
    fprintf(stderr, "my_objdump: %s: %s\n", file, msg);
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

static void print_dump(const Elf64_Ehdr *elf, const char *dump,
    const char *filename, const objdump_t *config)
{
    printf("\n%s:     file format elf%d-%s\n", filename,
        ((char *)elf)[EI_CLASS] == ELFCLASS32 ? 32 : 64,
    get_file_format(elf->e_machine));
    if (((char *)elf)[EI_CLASS] == ELFCLASS32)
        return;
    if (config->flags.headers)
        print_headers(elf, dump);
    else
        printf("\n");
    if (config->flags.full_content)
        print_sections(elf, dump);
}

static int dump_file(const char *const file, const objdump_t *config)
{
    int fd = -1;
    char *buff = NULL;
    struct stat s;
    const Elf64_Ehdr *elf = NULL;

    elf = load_elf(file, &buff, &s, &fd);
    if (elf == NULL)
        return EXIT_FAILURE_TEK;
    print_dump(elf, buff, file, config);
    munmap(buff, s.st_size);
    close(fd);
    return 0;
}

static int iterate_on_files(const char *const *files, const size_t size,
    const objdump_t *config)
{
    for (size_t i = 0; i < size; i++)
        if (dump_file(files[i], config) != 0)
            return EXIT_FAILURE_TEK;
    return 0;
}

static int parse_inputs(char *const *params, const int size,
    objdump_t *config)
{
    for (int i = 0; i < size; i++) {
        if (config->files_nb == MAX_FILES_NB)
            return -1;
        if (*params[i] != '-') {
            config->files[config->files_nb] = params[i];
            config->files_nb += 1;
            continue;
        }
        if (strpbrk(params[i], "s") != NULL)
            config->flags.full_content = true;
        if (strpbrk(params[i], "f") != NULL)
            config->flags.headers = true;
    }
    return 0;
}

int main(const int argc, char *const *argv)
{
    objdump_t config = {0};
    const char *const DEFAULT = "a.out";

    if (parse_inputs(&argv[1], argc - 1, &config) != 0)
        return EXIT_FAILURE_TEK;
    if (!config.flags.full_content && !config.flags.headers)
        return EXIT_FAILURE_TEK;
    if (config.files_nb > 0)
        return iterate_on_files(config.files, config.files_nb, &config);
    return dump_file(DEFAULT, &config);
}
