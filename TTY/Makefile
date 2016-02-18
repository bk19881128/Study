include ../Makefile.inc
CC = gcc
CFLAGES =  -lpthread
SRC = main.c\
	xtty.c\
	TkCli.c

OBJ = $(addprefix obj/,$(SRC:.c=.o))

TARGET = main

all: ${TARGET}

.PHONY: clean


main: ${OBJ}
	${CC}  ${OBJ} -o $@ ${CFLAGES}
obj/%.o : %.c	
	mkdir -p $(dir $@)
	${CC} -c $< -o $@ ${CFLAGES}
clean:
	${RM} ${TARGET} *.o 
	${RM} -r obj
cc: 
	@echo ${CC}
t: 
	@echo "this tst"
