/* Extrait — PRCursor/src/net.c — lecture avec timeout et détection fin de connexion */

int readn(int fd, void *buf, size_t n, int timeout_ms) {
    unsigned char *p = buf;
    size_t left = n;
    while (left > 0) {
        struct pollfd pf = {.fd = fd, .events = POLLIN};
        int pr = poll(&pf, 1, timeout_ms);
        if (pr <= 0) return -1;
        if (pf.revents & (POLLERR | POLLHUP | POLLNVAL)) return -1;
        ssize_t r = read(fd, p, left);
        if (r <= 0) return -1;
        p += r;
        left -= (size_t)r;
    }
    return 0;
}
