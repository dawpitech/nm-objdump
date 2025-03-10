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

TOTO_OBJ	=	$(TOTO_SRC:%.c=$(BDIR)/toto/%.o)
NM_OBJ		=	$(NM_SRC:%.c=$(BDIR)/nm/%.o)

TOTO_NAME	=	toto
NM_NAME		=	my_nm

.DEFAULT_GOAL := all

.PHONY: all
all: $(TOTO_NAME) $(NM_NAME)

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
	$(CC) $^ $(CFLGAS) -o $(NM_NAME)

.PHONY: clean
clean:
	@ rm -f $(TOTO_OBJ)
	@ rm -f $(NM_OBJ)
	@ rm -rf .build

.PHONY: fclean
fclean: clean
	@ rm -f $(TOTO_NAME)
	@ rm -f $(NM_NAME)

.NOTPARALLEL: re
.PHONY: re
re: fclean all

-include $(TOTO_OBJ:.o=.d)
-include $(NM_OBJ:.o=.d)
