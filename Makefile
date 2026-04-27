CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
all: paroles_server paroles_client
paroles_server: src/server.c
	$(CC) $(CFLAGS) -o $@ src/server.c
paroles_client: src/client.c
	$(CC) $(CFLAGS) -o $@ src/client.c
clean:
	rm -f paroles_server paroles_client
.PHONY: all clean
