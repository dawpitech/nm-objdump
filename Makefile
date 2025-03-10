##
## EPITECH PROJECT, 2025
## Makefile
## File description:
## Makefile
##

CC	:=	gcc

CFLAGS	=	-std=gnu17
CFLAGS	+=	-W
CFLAGS	+=	-Wall
CFLAGS	+=	-Wextra
CFLAGS	+=	-Wundef
CFLAGS	+=	-Wshadow
CFLAGS	+=	-Wunreachable-code
CFLAGS	+=	-pedantic
CFLAGS	+=	-I./include/
CFLAGS	+=	-MMD -MP

BDIR	=	.build

TOTO_SRC	=	$(shell find "src/toto" -name "*.c")
NM_SRC		=	$(shell find "src/nm" -name "*.c")
OBJDMP_SRC	=	$(shell find "src/objdump" -name "*.c")

TOTO_OBJ	=	$(TOTO_SRC:%.c=$(BDIR)/toto/%.o)
NM_OBJ		=	$(NM_SRC:%.c=$(BDIR)/nm/%.o)
OBJDMP_OBJ	=	$(OBJDMP_SRC:%.c=$(BDIR)/objdump/%.o)

TOTO_NAME	=	toto
NM_NAME		=	my_nm
OBJDMP_NAME	=	my_objdump

.DEFAULT_GOAL := all

.PHONY: all
all: $(TOTO_NAME) nm objdump

$(BDIR)/toto/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

$(TOTO_NAME): CFLAGS	+=	-Wno-unused-result
$(TOTO_NAME): $(TOTO_OBJ)
	$(CC) $^ $(CFLAGS) -o $@

$(BDIR)/nm/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

nm: $(NM_OBJ)
	$(CC) $^ $(CFLAGS) -o $(NM_NAME)

$(BDIR)/objdump/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

objdump: $(OBJDMP_OBJ)
	$(CC) $^ $(CFLAGS) -o $(OBJDMP_NAME)

.PHONY: clean
clean:
	@ rm -f $(TOTO_OBJ)
	@ rm -f $(NM_OBJ)
	@ rm -f $(OBJDMP_OBJ)
	@ rm -rf .build

.PHONY: fclean
fclean: clean
	@ rm -f $(TOTO_NAME)
	@ rm -f $(NM_NAME)
	@ rm -f $(OBJDMP_NAME)

.NOTPARALLEL: re
.PHONY: re
re: fclean all

-include $(TOTO_OBJ:.o=.d)
-include $(NM_OBJ:.o=.d)
