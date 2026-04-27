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

test: all
	@chmod +x tests/smoke.sh tests/regression_codereq.sh tests/verify_codereq_implemented.sh tests/feed_order_pdf.sh
	@tests/verify_codereq_implemented.sh
	@PORT=4242 tests/smoke.sh
	@PORT=4245 tests/regression_codereq.sh
	@PORT=4246 tests/feed_order_pdf.sh

.PHONY: all clean test
