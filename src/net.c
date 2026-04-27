#include "net.h"

#include <errno.h>
#include <poll.h>
#include <unistd.h>

int net_read_all(int fd, void *buf, size_t n, int timeout_ms) {
    unsigned char *p = buf;
    size_t left = n;
    while (left > 0) {
        if (timeout_ms >= 0) {
            struct pollfd pfd = {.fd = fd, .events = POLLIN};
            int pr = poll(&pfd, 1, timeout_ms);
            if (pr == 0)
                return -2;
            if (pr < 0) {
                if (errno == EINTR)
                    continue;
                return -1;
            }
        }
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (r == 0)
            return -1;
        p += (size_t)r;
        left -= (size_t)r;
    }
    return 0;
}

int net_write_all(int fd, const void *buf, size_t n) {
    const unsigned char *p = buf;
    size_t left = n;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        p += (size_t)w;
        left -= (size_t)w;
    }
    return 0;
}
