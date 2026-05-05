/* Extrait — PRCursor/src/tls_io.c — conn_readn (TLS ou clair), timeouts WANT_READ/WRITE */

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
