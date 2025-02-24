/*
** EPITECH PROJECT, 2025
** nm-objdump
** File description:
** main.c
*/

#include <unistd.h>

void toto(void)
{
    write(0, "Hello there!\n", 13);
}

int main(void)
{
    toto();
    return 0;
}
