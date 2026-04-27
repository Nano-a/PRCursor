CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -Iinclude
LDFLAGS =
LDLIBS =
SRC = src
BIN = paroles_server paroles_client
all: $(BIN)
paroles_server: $(SRC)/server.o $(SRC)/wire.o $(SRC)/net.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
paroles_client: $(SRC)/client.o $(SRC)/wire.o $(SRC)/net.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
$(SRC)/server.o: $(SRC)/server.c include/paroles_proto.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/client.o: $(SRC)/client.c include/paroles_proto.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/wire.o: $(SRC)/wire.c $(SRC)/wire.h include/paroles_proto.h
$(SRC)/net.o: $(SRC)/net.c $(SRC)/net.h
clean:
	rm -f $(SRC)/*.o $(BIN)
.PHONY: all clean
