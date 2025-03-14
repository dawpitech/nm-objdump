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

void print_sections_64(const Elf64_Ehdr *elf, const char *buff);
void print_sections_32(const Elf32_Ehdr *elf, const char *buff);
void print_headers_64(const Elf64_Ehdr *elf);
void print_headers_32(const Elf32_Ehdr *elf);

#endif //OBJDUMP_H
