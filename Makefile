CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -Iinclude -DPAROLES_ACCEPT_REAL_CLE_113
LDFLAGS =
LDLIBS = -lssl -lcrypto
SRC = src
BIN = paroles_server paroles_client
all: $(BIN)
paroles_server: $(SRC)/server.o $(SRC)/wire.o $(SRC)/net.o $(SRC)/tls_io.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
paroles_client: $(SRC)/client.o $(SRC)/wire.o $(SRC)/net.o $(SRC)/tls_io.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
$(SRC)/server.o: $(SRC)/server.c include/paroles_proto.h include/tls_io.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/client.o: $(SRC)/client.c include/paroles_proto.h include/tls_io.h $(SRC)/net.h $(SRC)/wire.h
$(SRC)/tls_io.o: $(SRC)/tls_io.c include/tls_io.h $(SRC)/net.h
$(SRC)/wire.o: $(SRC)/wire.c $(SRC)/wire.h include/paroles_proto.h
$(SRC)/net.o: $(SRC)/net.c $(SRC)/net.h
clean:
	rm -f $(SRC)/*.o $(BIN)

test: all
	@chmod +x tests/smoke.sh tests/regression_codereq.sh tests/verify_codereq_implemented.sh tests/feed_order_pdf.sh tests/notif_codereq.sh tests/tls_smoke.sh tests/stage3_cle_smoke.sh scripts/gencerts.sh scripts/gen_ed25519.sh
	@tests/verify_codereq_implemented.sh
	@PORT=4242 tests/smoke.sh
	@PORT=4245 tests/regression_codereq.sh
	@PORT=4246 tests/feed_order_pdf.sh
	@PORT=4247 tests/notif_codereq.sh
	@PORT=4248 tests/tls_smoke.sh
	@PORT=4249 tests/stage3_cle_smoke.sh

.PHONY: all clean test
