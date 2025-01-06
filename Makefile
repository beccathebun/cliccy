CC ?= gcc

builder:
	$(CC) -o build -Iinclude build.c
