#ifndef TLS_IO_H
#define TLS_IO_H

#include <openssl/ssl.h>
#include <stddef.h>

SSL_CTX *paroles_tls_server_ctx(const char *cert_pem, const char *key_pem);
SSL_CTX *paroles_tls_client_ctx(const char *ca_pem);
void paroles_tls_ctx_free(SSL_CTX *ctx);

/* Lecture / écriture : ssl NULL = TCP clair (readn/writen). */
int conn_readn(SSL *ssl, int fd, void *buf, size_t n, int timeout_ms);
int conn_read_upto(SSL *ssl, int fd, void *buf, size_t cap, int timeout_ms);
int conn_writen(SSL *ssl, int fd, const void *buf, size_t n);

#endif
