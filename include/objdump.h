/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** objdump.h
*/

#ifndef OBJDUMP_H
    #define OBJDUMP_H

    #include <elf.h>
    #include <stdbool.h>

    #define MAX_FILES_NB 64

typedef struct {
    struct {
        bool full_content;
        bool headers;
    } flags;
    const char *files[MAX_FILES_NB];
    size_t files_nb;
} objdump_t;

void print_sections(const Elf64_Ehdr *elf, const char *buff);
void print_headers(const Elf64_Ehdr *elf, const char *buff);

#endif //OBJDUMP_H
