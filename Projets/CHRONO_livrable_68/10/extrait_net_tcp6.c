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
