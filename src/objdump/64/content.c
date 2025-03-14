/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** content.c
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "objdump.h"

static bool should_print_section_64(const Elf64_Shdr *shdr,
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

static void print_se_content_hex_64(const unsigned char *content,
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

static void print_se_content_ascii_64(const unsigned char *content,
    const size_t size)
{
    printf("  ");
    for (size_t idx = 0; idx < size; idx++)
        printf("%c", isprint(content[idx]) ? content[idx] : '.');
}

static void print_section_64(const Elf64_Shdr *shdr,
    const char *shstrtab, const char *buff)
{
    const unsigned char *content_addr =
        (unsigned char *) buff + shdr->sh_offset;

    if (!should_print_section_64(shdr, shstrtab))
        return;
    printf("Contents of section %s:\n", &shstrtab[shdr->sh_name]);
    for (size_t i = 0; i < shdr->sh_size; i += 16) {
        printf(" %04lx", shdr->sh_addr + i);
        if (i + 16 > shdr->sh_size) {
            print_se_content_hex_64(content_addr + i, shdr->sh_size - i);
            print_se_content_ascii_64(content_addr + i, shdr->sh_size - i);
        } else {
            print_se_content_hex_64(content_addr + i, 16);
            print_se_content_ascii_64(content_addr + i, 16);
        }
        printf("\n");
    }
}

void print_sections_64(const Elf64_Ehdr *elf, const char *buff)
{
    const Elf64_Shdr *shdr = (Elf64_Shdr *) (buff + elf->e_shoff);
    const char *shstrtab = buff + shdr[elf->e_shstrndx].sh_offset;

    for (int i = 0; i < elf->e_shnum; i++)
        print_section_64(&shdr[i], shstrtab, buff);
}
