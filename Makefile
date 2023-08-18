CC=gcc
CFLAGS=-Wall -Werror -pedantic -O3
OUT=subnet-cli
SRCS=subnet.c subnet-cli.c

build: subnet-cli.c subnet.c
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS)

example: main.c subnet.c
	$(CC) $(CFLAGS) -o example main.c subnet.c