#ifndef NET_H
#define NET_H

#include <netinet/in.h>
#include <poll.h>
#include <stddef.h>

/* N°41 — timeout TCP lecture (client idle) */
#define PAROLES_TCP_TIMEOUT_MS 30000

int tcp6_listen(const char *bind_addr, uint16_t port);
int tcp6_accept(int srv, struct sockaddr_in6 *peer, socklen_t *peer_len);
int tcp6_connect(const char *host, uint16_t port, struct sockaddr_in6 *local_hint);

int writen(int fd, const void *buf, size_t n);
int readn(int fd, void *buf, size_t n, int timeout_ms);
int udp6_send(int fd, const struct sockaddr_in6 *to, const void *buf, size_t n);
int udp6_recv(int fd, void *buf, size_t max, struct sockaddr_in6 *from, int timeout_ms);

int join_mcast(int fd, const struct in6_addr *maddr, unsigned ifindex);

#endif
