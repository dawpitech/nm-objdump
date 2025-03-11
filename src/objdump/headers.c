/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** headers.c
*/

#include <stdio.h>

#include "flags.h"
#include "objdump.h"

static const char *get_readable_architecture(const Elf64_Half machine_type)
{
    switch (machine_type) {
        case EM_386:
            return "i386";
        case EM_X86_64:
            return "i386:x86-64";
        default:
            return "???";
    }
}

static size_t get_flags_value(const Elf64_Half obj_type)
{
    switch (obj_type) {
        case ET_REL:
            return HAS_SYMS | HAS_RELOC;
        case ET_EXEC:
            return D_PAGED | HAS_SYMS | EXEC_P;
        case ET_DYN:
            return D_PAGED | DYNAMIC | HAS_SYMS;
        default:
            return 0;
    }
}

static const char *get_flags_readable(const Elf64_Half obj_type)
{
    switch (obj_type) {
        case ET_REL:
            return "HAS_RELOC, HAS_SYMS";
        case ET_EXEC:
            return "EXEC_P, HAS_SYMS, D_PAGED";
        case ET_DYN:
            return "HAS_SYMS, DYNAMIC, D_PAGED";
        default:
            return "";
    }
}

void print_headers(const Elf64_Ehdr *elf, const char *buff)
{
    printf("architecture: %s, flags 0x%08lx:\n"
        "%s\n"
        "start address 0x%016lx\n\n",
        get_readable_architecture(elf->e_machine),
        get_flags_value(elf->e_type),
        get_flags_readable(elf->e_type),
        elf->e_entry);
}
