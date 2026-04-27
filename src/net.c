#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int tcp6_listen(const char *bind_addr, uint16_t port) {
    int s = socket(AF_INET6, SOCK_STREAM, 0);
    if (s < 0) return -1;
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof a);
    a.sin6_family = AF_INET6;
    a.sin6_port = htons(port);
    if (inet_pton(AF_INET6, bind_addr, &a.sin6_addr) != 1) {
        close(s);
        return -1;
    }
    if (bind(s, (struct sockaddr *)&a, sizeof a) < 0) {
        close(s);
        return -1;
    }
    if (listen(s, 8) < 0) {
        close(s);
        return -1;
    }
    return s;
}

int tcp6_accept(int srv, struct sockaddr_in6 *peer, socklen_t *peer_len) {
    return accept(srv, (struct sockaddr *)peer, peer_len);
}

int tcp6_connect(const char *host, uint16_t port, struct sockaddr_in6 *local_hint) {
    (void)local_hint;
    int s = socket(AF_INET6, SOCK_STREAM, 0);
    if (s < 0) return -1;
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof a);
    a.sin6_family = AF_INET6;
    a.sin6_port = htons(port);
    if (inet_pton(AF_INET6, host, &a.sin6_addr) != 1) {
        close(s);
        return -1;
    }
    if (connect(s, (struct sockaddr *)&a, sizeof a) < 0) {
        close(s);
        return -1;
    }
    return s;
}

int writen(int fd, const void *buf, size_t n) {
    const unsigned char *p = buf;
    size_t left = n;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w <= 0) return -1;
        p += w;
        left -= (size_t)w;
    }
    return 0;
}

/* N°40 — erreurs lecture / fin de connexion */
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

int udp6_send(int fd, const struct sockaddr_in6 *to, const void *buf, size_t n) {
    ssize_t s = sendto(fd, buf, n, 0, (const struct sockaddr *)to, sizeof *to);
    return s == (ssize_t)n ? 0 : -1;
}

int udp6_recv(int fd, void *buf, size_t max, struct sockaddr_in6 *from, int timeout_ms) {
    struct pollfd pf = {.fd = fd, .events = POLLIN};
    if (poll(&pf, 1, timeout_ms) <= 0) return -1;
    socklen_t flen = sizeof *from;
    ssize_t r = recvfrom(fd, buf, max, 0, (struct sockaddr *)from, &flen);
    return (int)r;
}

int join_mcast(int fd, const struct in6_addr *maddr, unsigned ifindex) {
    struct ipv6_mreq mr;
    memset(&mr, 0, sizeof mr);
    mr.ipv6mr_multiaddr = *maddr;
    mr.ipv6mr_interface = (int)ifindex;
    return setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mr, sizeof mr);
}
