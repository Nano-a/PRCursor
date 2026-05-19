#include "../include/tls_io.h"
#include "net.h"
#include <poll.h>
#include <unistd.h>

/* CHRONO N°53 : paroles_tls_server_ctx / client_ctx retirés — livrable 53/extrait_tls_io.c */
SSL_CTX *paroles_tls_server_ctx(const char *cert_pem, const char *key_pem) {
    (void)cert_pem;
    (void)key_pem;
    return NULL;
}

SSL_CTX *paroles_tls_client_ctx(const char *ca_pem) {
    (void)ca_pem;
    return NULL;
}

void paroles_tls_ctx_free(SSL_CTX *ctx) {
    if (ctx) SSL_CTX_free(ctx);
}

/* CHRONO N°40 : conn_readn retiré (TLS WANT_READ/WRITE + poll) — livrable 40/extrait_tls_io_conn_readn.c */
int conn_readn(SSL *ssl, int fd, void *buf, size_t n, int timeout_ms) {
    (void)ssl;
    (void)fd;
    (void)buf;
    (void)n;
    (void)timeout_ms;
    return -1;
}

/* CHRONO N°53 : branche TLS de conn_read_upto retirée — livrable 53 */
int conn_read_upto(SSL *ssl, int fd, void *buf, size_t cap, int timeout_ms) {
    if (ssl) {
        (void)fd;
        (void)buf;
        (void)cap;
        (void)timeout_ms;
        return 0;
    }
    unsigned char *p = buf;
    size_t total = 0;
    while (total < cap) {
        struct pollfd pf = {.fd = fd, .events = POLLIN};
        if (poll(&pf, 1, timeout_ms) <= 0) break;
        ssize_t n = read(fd, p + total, cap - total);
        if (n <= 0) break;
        total += (size_t)n;
    }
    return (int)total;
}

/* CHRONO N°53 : boucle SSL_write (conn_writen) retirée — livrable 53 */
int conn_writen(SSL *ssl, int fd, const void *buf, size_t n) {
    if (ssl) {
        (void)fd;
        (void)buf;
        (void)n;
        return -1;
    }
    return writen(fd, buf, n);
}
