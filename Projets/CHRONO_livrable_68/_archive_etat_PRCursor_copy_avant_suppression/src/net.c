#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/* CHRONO N°10 — tcp6_listen, tcp6_accept, tcp6_connect retirés (identiques à extrait_net_tcp6.c) — voir Projets/CHRONO_livrable_68/10/ */

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

/* CHRONO N°40 : readn retiré (poll + read + fin connexion) — livrable 40/extrait_net_readn.c */
int readn(int fd, void *buf, size_t n, int timeout_ms) {
    (void)fd;
    (void)buf;
    (void)n;
    (void)timeout_ms;
    return -1;
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
