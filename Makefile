CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -Iinclude
LDFLAGS =
LDLIBS =

SRC = src
BIN = paroles_server paroles_client paroles_smoke test_feed_order

all: $(BIN)

paroles_server: $(SRC)/server.o $(SRC)/wire.o $(SRC)/net.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

paroles_client: $(SRC)/client.o $(SRC)/wire.o $(SRC)/net.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

paroles_smoke: $(SRC)/smoke.o $(SRC)/wire.o $(SRC)/net.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test_feed_order: $(SRC)/test_feed_order.o $(SRC)/wire.o $(SRC)/net.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(SRC)/server.o: $(SRC)/server.c include/paroles_proto.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/client.o: $(SRC)/client.c include/paroles_proto.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/smoke.o: $(SRC)/smoke.c include/paroles_proto.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/test_feed_order.o: $(SRC)/test_feed_order.c include/paroles_proto.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/wire.o: $(SRC)/wire.c $(SRC)/wire.h include/paroles_proto.h
$(SRC)/net.o: $(SRC)/net.c $(SRC)/net.h

clean:
	rm -f $(SRC)/*.o $(BIN)

test: all
	@chmod +x tests/smoke.sh tests/feed_order.sh
	@tests/smoke.sh
	@tests/feed_order.sh

.PHONY: all clean test
