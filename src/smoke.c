/* Test automatisé minimal du protocole (étape 1). */

#include "../include/paroles_proto.h"
#include "net.h"
#include "wire.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int die(const char *m) {
    fprintf(stderr, "smoke: %s\n", m);
    return 1;
}

static int tcp6(const char *host, uint16_t port) {
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    int z = 0;
    setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &z, sizeof(z));
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, host, &a.sin6_addr) != 1) {
        close(fd);
        return -1;
    }
    a.sin6_port = htons(port);
    if (connect(fd, (struct sockaddr *)&a, sizeof(a)) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static int reg(const char *host, uint16_t port, const char *pseudo, uint16_t *id) {
    int fd = tcp6(host, port);
    if (fd < 0)
        return -1;
    uint8_t msg[4 + PAROLES_NOM_LEN + PAROLES_CLE_LEN];
    wire_put16(msg, PAROLES_REQ_REGISTER);
    wire_put16(msg + 2, 0);
    wire_pad_nom(msg + 4, pseudo);
    memset(msg + 4 + PAROLES_NOM_LEN, 0, PAROLES_CLE_LEN);
    if (net_write_all(fd, msg, sizeof(msg)) != 0) {
        close(fd);
        return -1;
    }
    uint8_t r[119];
    if (net_read_all(fd, r, sizeof(r), -1) != 0) {
        close(fd);
        return -1;
    }
    close(fd);
    if (wire_get16(r) != PAROLES_RESP_REGISTER)
        return -1;
    *id = wire_get16(r + 2);
    return 0;
}

static int create_grp(const char *host, uint16_t port, uint16_t uid, const char *name,
                      uint16_t *gid) {
    int fd = tcp6(host, port);
    size_t nl = strlen(name);
    size_t tot = 6 + nl;
    uint8_t *msg = malloc(tot);
    wire_put16(msg, PAROLES_REQ_CREATE_GROUP);
    wire_put16(msg + 2, uid);
    wire_put16(msg + 4, (uint16_t)nl);
    memcpy(msg + 6, name, nl);
    net_write_all(fd, msg, tot);
    free(msg);
    uint8_t b[22];
    if (net_read_all(fd, b, sizeof(b), -1) != 0) {
        close(fd);
        return -1;
    }
    close(fd);
    if (wire_get16(b) != PAROLES_RESP_CREATE_GROUP)
        return -1;
    *gid = wire_get16(b + 2);
    return 0;
}

static int invite(const char *host, uint16_t port, uint16_t uid, uint16_t gid,
                  uint16_t invitee) {
    int fd = tcp6(host, port);
    uint8_t m[16];
    wire_put16(m, PAROLES_REQ_INVITE);
    wire_put16(m + 2, uid);
    wire_put16(m + 4, gid);
    wire_put16(m + 6, 1);
    wire_put16(m + 8, invitee);
    wire_put16(m + 10, 0);
    net_write_all(fd, m, 12);
    uint8_t a[4];
    net_read_all(fd, a, 4, -1);
    close(fd);
    return wire_get16(a) == PAROLES_ACK ? 0 : -1;
}

static int accept_inv(const char *host, uint16_t port, uint16_t uid, uint16_t gid) {
    int fd = tcp6(host, port);
    uint8_t m[10];
    wire_put16(m, PAROLES_REQ_INVITE_ANSWER);
    wire_put16(m + 2, uid);
    wire_put16(m + 4, gid);
    wire_put16(m + 6, PAROLES_INV_ACCEPT);
    wire_put16(m + 8, 0);
    net_write_all(fd, m, sizeof(m));
    uint8_t h[4];
    net_read_all(fd, h, 4, -1);
    uint16_t c = wire_get16(h);
    if (c != PAROLES_RESP_JOINED) {
        close(fd);
        return -1;
    }
    uint8_t r[20];
    net_read_all(fd, r, sizeof(r), -1);
    uint16_t nb = wire_get16(r + 18);
    for (uint16_t i = 0; i < nb; i++) {
        uint8_t blk[14];
        net_read_all(fd, blk, sizeof(blk), -1);
    }
    close(fd);
    return 0;
}

static int post(const char *host, uint16_t port, uint16_t uid, uint16_t gid,
                const char *text) {
    int fd = tcp6(host, port);
    size_t tl = strlen(text);
    size_t tot = 10 + tl;
    uint8_t *msg = malloc(tot);
    wire_put16(msg, PAROLES_REQ_POST);
    wire_put16(msg + 2, uid);
    wire_put16(msg + 4, gid);
    wire_put16(msg + 6, 0);
    wire_put16(msg + 8, (uint16_t)tl);
    memcpy(msg + 10, text, tl);
    net_write_all(fd, msg, tot);
    free(msg);
    uint8_t o[8];
    net_read_all(fd, o, sizeof(o), -1);
    close(fd);
    return wire_get16(o) == PAROLES_RESP_POST ? 0 : -1;
}

static int feed_all(const char *host, uint16_t port, uint16_t uid, uint16_t gid,
                    uint16_t *nb_out) {
    int fd = tcp6(host, port);
    uint8_t m[12];
    wire_put16(m, PAROLES_REQ_LIST_FEED);
    wire_put16(m + 2, uid);
    wire_put16(m + 4, gid);
    wire_put16(m + 6, 0);
    wire_put16(m + 8, 0);
    wire_put16(m + 10, 0);
    net_write_all(fd, m, sizeof(m));
    uint8_t rh[6];
    net_read_all(fd, rh, sizeof(rh), -1);
    if (wire_get16(rh) != PAROLES_RESP_LIST_FEED) {
        close(fd);
        return -1;
    }
    *nb_out = wire_get16(rh + 4);
    for (uint16_t i = 0; i < *nb_out; i++) {
        uint8_t hd[10];
        net_read_all(fd, hd, sizeof(hd), -1);
        uint16_t len = wire_get16(hd + 8);
        uint8_t *buf = malloc(len);
        net_read_all(fd, buf, len, -1);
        free(buf);
    }
    close(fd);
    return 0;
}

int main(int argc, char **argv) {
    const char *host = "::1";
    uint16_t port = 4242;
    if (argc >= 2)
        host = argv[1];
    if (argc >= 3)
        port = (uint16_t)atoi(argv[2]);

    uint16_t u1, u2, g;
    if (reg(host, port, "alice", &u1) != 0)
        return die("register alice");
    if (reg(host, port, "bob", &u2) != 0)
        return die("register bob");
    if (u1 == u2)
        return die("ids distincts attendus");
    if (create_grp(host, port, u1, "g1", &g) != 0)
        return die("create");
    if (invite(host, port, u1, g, u2) != 0)
        return die("invite");
    if (accept_inv(host, port, u2, g) != 0)
        return die("accept");
    if (post(host, port, u1, g, "hello") != 0)
        return die("post");
    uint16_t nb = 0;
    if (feed_all(host, port, u2, g, &nb) != 0)
        return die("feed");
    if (nb != 1)
        return die("feed nb==1");
    printf("smoke OK (u1=%u u2=%u g=%u)\n", u1, u2, g);
    return 0;
}
