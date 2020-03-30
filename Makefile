.DEFAULT_GOAL: all

CC = gcc

INCLUDE := include/
SRCS := $(wildcard 	src/*.c)
COALESCE ?= -DUNSAFE_COALESCE

all: ${SRCS} ${INCLUDE}
	${CC} -Wall -Werror ${SRCS} -I${INCLUDE} ${COALESCE}

debug: ${SRCS} ${INCLUDE}
	${CC} -Wall -Werror -g ${SRCS} -I${INCLUDE} -DDEBUG ${COALESCE}



.PHONY: clean

clean:
	rm -f ./a.out
