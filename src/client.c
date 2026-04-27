#include "../include/paroles_proto.h"
#include "net.h"
#include "wire.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int verbose;

static void pad_nom(unsigned char *dst, const char *src) {
    memset(dst, 0, PAROLES_NOM_LEN);
    strncpy((char *)dst, src, PAROLES_NOM_LEN);
}

static int one_cmd(const char *host, uint16_t port, const unsigned char *msg, size_t len,
                   unsigned char *resp, size_t rmax) {
    struct sockaddr_in6 lh;
    memset(&lh, 0, sizeof lh);
    int fd = tcp6_connect(host, port, &lh);
    if (fd < 0) {
        perror("connect");
        return -1;
    }
    if (writen(fd, msg, len) < 0) {
        close(fd);
        return -1;
    }
    ssize_t n = read(fd, resp, rmax);
    close(fd);
    if (n <= 0) return -1;
    if (verbose) fprintf(stderr, "reponse %zd octets, code=%u\n", n, resp[0]);
    return (int)n;
}

static int cmd_reg(const char *host, uint16_t port, const char *name) {
    unsigned char msg[256], resp[4096];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_REG);
    pad_nom(p, name);
    p += PAROLES_NOM_LEN;
    wire_put_zeros(&p, PAROLES_CLE_LEN);
    int n = one_cmd(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 2 + PAROLES_CLE_LEN) return -1;
    const unsigned char *q = resp;
    size_t left = (size_t)n;
    uint8_t c;
    if (wire_get_u8(&q, &left, &c) < 0 || c != PAROLES_CODEREQ_REG_OK) return -1;
    uint32_t id;
    uint16_t udp;
    if (wire_get_u32_be(&q, &left, &id) < 0) return -1;
    if (wire_get_u16_be(&q, &left, &udp) < 0) return -1;
    printf("OK id=%u udp=%u\n", id, (unsigned)udp);
    return 0;
}

static int cmd_newgroup(const char *host, uint16_t port, uint32_t uid, const char *gname) {
    unsigned char msg[512], resp[256];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_NEW_GROUP);
    wire_put_u32_be(&p, uid);
    uint16_t gl = (uint16_t)strlen(gname);
    wire_put_u16_be(&p, gl);
    memcpy(p, gname, gl);
    p += gl;
    int n = one_cmd(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 2 + 16) return -1;
    const unsigned char *q = resp;
    size_t left = (size_t)n;
    uint8_t c;
    if (wire_get_u8(&q, &left, &c) < 0 || c != PAROLES_CODEREQ_NEW_GROUP_OK) return -1;
    uint32_t idg;
    uint16_t mport;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u16_be(&q, &left, &mport) < 0) return -1;
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, q, ip, sizeof ip);
    printf("OK idg=%u mcast=%s port=%u\n", idg, ip, (unsigned)mport);
    return 0;
}

static int cmd_invite(const char *host, uint16_t port, uint32_t admin, uint32_t idg, int ninv,
                      uint32_t *ids) {
    unsigned char msg[2048], resp[64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_INVITE);
    wire_put_u32_be(&p, admin);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, (uint32_t)ninv);
    for (int i = 0; i < ninv; i++) {
        wire_put_u32_be(&p, ids[i]);
        wire_put_zeros(&p, PAROLES_INV_PAD);
    }
    int n = one_cmd(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1) return -1;
    return resp[0] == PAROLES_CODEREQ_ACK ? 0 : -1;
}

static int cmd_listinv(const char *host, uint16_t port, uint32_t uid) {
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_INV);
    wire_put_u32_be(&p, uid);
    int n = one_cmd(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 5) return -1;
    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1;
    uint32_t nb;
    if (wire_get_u32_be(&q, &left, &nb) < 0) return -1;
    printf("invitations %u\n", nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t ig;
        uint16_t ln;
        if (wire_get_u32_be(&q, &left, &ig) < 0) return -1;
        if (wire_get_u16_be(&q, &left, &ln) < 0) return -1;
        if (left < ln + PAROLES_NOM_LEN) return -1;
        char gn[256];
        memcpy(gn, q, ln);
        gn[ln] = 0;
        q += ln;
        char adm[PAROLES_NOM_LEN + 1];
        memcpy(adm, q, PAROLES_NOM_LEN);
        adm[PAROLES_NOM_LEN] = 0;
        q += PAROLES_NOM_LEN;
        left -= (size_t)ln + PAROLES_NOM_LEN;
        printf("  idg=%u groupe=%s admin=%s\n", ig, gn, adm);
    }
    return 0;
}

static int cmd_ans(const char *host, uint16_t port, uint32_t uid, uint32_t idg, int an) {
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_INV_ANS);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u8(&p, (uint8_t)an);
    int n = one_cmd(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1) return -1;
    if (resp[0] == PAROLES_CODEREQ_ACK) {
        printf("OK ack\n");
        return 0;
    }
    if (resp[0] == PAROLES_CODEREQ_JOIN_OK) {
        printf("OK join\n");
        return 0;
    }
    return -1;
}

static int cmd_listmem(const char *host, uint16_t port, uint32_t uid, uint32_t idg) {
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_MEM);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    int n = one_cmd(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 4) return -1;
    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1;
    uint32_t ig, nb;
    if (wire_get_u32_be(&q, &left, &ig) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &nb) < 0) return -1;
    printf("membres idg=%u nb=%u\n", ig, nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t ui;
        if (wire_get_u32_be(&q, &left, &ui) < 0) return -1;
        if (left < PAROLES_NOM_LEN) return -1;
        char nm[PAROLES_NOM_LEN + 1];
        memcpy(nm, q, PAROLES_NOM_LEN);
        nm[PAROLES_NOM_LEN] = 0;
        q += PAROLES_NOM_LEN;
        left -= PAROLES_NOM_LEN;
        printf("  %u %s\n", ui, nm);
    }
    return 0;
}
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 0;
}
