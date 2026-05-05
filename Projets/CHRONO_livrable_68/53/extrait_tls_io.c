#include "../include/tls_io.h"
#include "net.h"
#include <poll.h>
#include <unistd.h>

SSL_CTX *paroles_tls_server_ctx(const char *cert_pem, const char *key_pem) {
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) return NULL;
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    if (SSL_CTX_use_certificate_file(ctx, cert_pem, SSL_FILETYPE_PEM) <= 0) goto bad;
    if (SSL_CTX_use_PrivateKey_file(ctx, key_pem, SSL_FILETYPE_PEM) <= 0) goto bad;
    if (!SSL_CTX_check_private_key(ctx)) goto bad;
    return ctx;
bad:
    SSL_CTX_free(ctx);
    return NULL;
}

SSL_CTX *paroles_tls_client_ctx(const char *ca_pem) {
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) return NULL;
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    if (SSL_CTX_load_verify_locations(ctx, ca_pem, NULL) != 1) {
        SSL_CTX_free(ctx);
        return NULL;
    }
    return ctx;
}

void paroles_tls_ctx_free(SSL_CTX *ctx) {
    if (ctx) SSL_CTX_free(ctx);
}

int conn_readn(SSL *ssl, int fd, void *buf, size_t n, int timeout_ms) {
    if (!ssl) return readn(fd, buf, n, timeout_ms);
    unsigned char *p = buf;
    size_t left = n;
    while (left > 0) {
        int r = SSL_read(ssl, p, (int)left);
        if (r > 0) {
            p += r;
            left -= (size_t)r;
            continue;
        }
        int e = SSL_get_error(ssl, r);
        if (e == SSL_ERROR_WANT_READ) {
            struct pollfd pf = {.fd = fd, .events = POLLIN};
            if (poll(&pf, 1, timeout_ms) <= 0) return -1;
            continue;
        }
        if (e == SSL_ERROR_WANT_WRITE) {
            struct pollfd pf = {.fd = fd, .events = POLLOUT};
            if (poll(&pf, 1, timeout_ms) <= 0) return -1;
            continue;
        }
        return -1;
    }
    return 0;
}

int conn_read_upto(SSL *ssl, int fd, void *buf, size_t cap, int timeout_ms) {
    unsigned char *p = buf;
    size_t total = 0;
    while (total < cap) {
        struct pollfd pf = {.fd = fd, .events = POLLIN};
        if (poll(&pf, 1, timeout_ms) <= 0) break;
        ssize_t n;
        if (ssl) {
            int r = SSL_read(ssl, p + total, (int)(cap - total));
            if (r > 0) {
                total += (size_t)r;
                continue;
            }
            int e = SSL_get_error(ssl, r);
            if (e == SSL_ERROR_ZERO_RETURN) break;
            if (e == SSL_ERROR_WANT_READ) continue;
            if (e == SSL_ERROR_WANT_WRITE) {
                pf.events = POLLOUT;
                if (poll(&pf, 1, timeout_ms) <= 0) break;
                continue;
            }
            break;
        } else {
            n = read(fd, p + total, cap - total);
            if (n <= 0) break;
            total += (size_t)n;
        }
    }
    return (int)total;
}

int conn_writen(SSL *ssl, int fd, const void *buf, size_t n) {
    if (!ssl) return writen(fd, buf, n);
    const unsigned char *p = buf;
    size_t left = n;
    while (left > 0) {
        int w = SSL_write(ssl, p, (int)left);
        if (w > 0) {
            p += w;
            left -= (size_t)w;
            continue;
        }
        int e = SSL_get_error(ssl, w);
        if (e == SSL_ERROR_WANT_WRITE) {
            struct pollfd pf = {.fd = fd, .events = POLLOUT};
            if (poll(&pf, 1, PAROLES_TCP_TIMEOUT_MS) <= 0) return -1;
            continue;
        }
        if (e == SSL_ERROR_WANT_READ) {
            struct pollfd pf = {.fd = fd, .events = POLLIN};
            if (poll(&pf, 1, PAROLES_TCP_TIMEOUT_MS) <= 0) return -1;
            continue;
        }
        return -1;
    }
    return 0;
}
