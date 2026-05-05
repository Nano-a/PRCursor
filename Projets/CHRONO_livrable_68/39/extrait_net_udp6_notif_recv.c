/*
 * CHRONO N°39 — extrait depuis PRCursor/src/net.c (helpers réception notifs UDP/mcast).
 * Voir ../../../PRCursor pour le fichier complet + en-tête net.h
 */

#include "net.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int udp6_bind_any(uint16_t port) {
    int s = socket(AF_INET6, SOCK_DGRAM, 0);
    if (s < 0) return -1;
    int on = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) != 0) goto bad;
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof a);
    a.sin6_family = AF_INET6;
    a.sin6_port = htons(port);
    a.sin6_addr = in6addr_any;
    if (bind(s, (struct sockaddr *)&a, sizeof a) < 0) goto bad;
    return s;
bad:
    close(s);
    return -1;
}

int udp6_mcast_recv_socket(const char *mcast_ipv6, uint16_t port) {
    struct in6_addr g;
    if (inet_pton(AF_INET6, mcast_ipv6, &g) != 1) {
        errno = EINVAL;
        return -1;
    }
    int s = udp6_bind_any(port);
    if (s < 0) return -1;
    if (join_mcast(s, &g, 0) != 0) {
        close(s);
        return -1;
    }
    return s;
}
