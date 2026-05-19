/*
 * Snapshot dossier CHRONO N°17 — **octet pour octet identique au client du dossier 16**.
 * À copier en remplacement complet de src/client.c.
 * Référencé ici pour vous éviter deux chemins sur Discord ; le commit ligne 17 peut
 * ne toucher que le serveur dans Git.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "paroles_proto.h"
#include "net.h"
#include "wire.h"

#ifndef PAROLES_ACCEPT_REAL_CLE_113

static void pad_nom(unsigned char *dst, const char *src) {
    memset(dst, 0, PAROLES_NOM_LEN);
    strncpy((char *)dst, src, PAROLES_NOM_LEN);
}

static int fill_ed25519_cle_from_pem(const char *path, unsigned char *cle) {
    (void)path;
    (void)cle;
    return -1;
}

static int cmd_reg(const char *host, uint16_t port, const char *name, const char *pub_pem)
{
    unsigned char msg[256], resp[4096];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_REG);
    pad_nom(p, name);
    p += PAROLES_NOM_LEN;
    if (pub_pem) {
        unsigned char cle[PAROLES_CLE_LEN];
        if (fill_ed25519_cle_from_pem(pub_pem, cle) < 0)
            return -1;
        memcpy(p, cle, PAROLES_CLE_LEN);
        p += PAROLES_CLE_LEN;
    } else {
        wire_put_zeros(&p, PAROLES_CLE_LEN);
    }

    struct sockaddr_in6 lh;
    memset(&lh, 0, sizeof lh);
    int fd = tcp6_connect(host, port, &lh);
    if (fd < 0) {
        perror("connect");
        return -1;
    }
    size_t nout = (size_t)(p - msg);
    if (writen(fd, msg, nout) < 0) {
        close(fd);
        return -1;
    }
    size_t need = 1u + 4u + 2u + (size_t)PAROLES_CLE_LEN;
    if (readn(fd, resp, need) < 0) {
        close(fd);
        return -1;
    }

    const unsigned char *q = resp;
    size_t left = need;
    uint8_t c;
    if (wire_get_u8(&q, &left, &c) < 0 || c != PAROLES_CODEREQ_REG_OK) {
        close(fd);
        return -1;
    }
    uint32_t id;
    uint16_t udp;
    if (wire_get_u32_be(&q, &left, &id) < 0 || wire_get_u16_be(&q, &left, &udp) < 0 ||
        wire_expect_zeros(&q, &left, (size_t)PAROLES_CLE_LEN) < 0) {
        close(fd);
        return -1;
    }
    printf("OK id=%u udp=%u\n", id, (unsigned)udp);
    close(fd);
    return 0;
}

/*
 * extrait_client_codereq_3_4.c — CODEREQ 3 / lecture 4 ;
 * tls : one_cmd -> ici tcp6_connect / writen / readn puis close (un échange TCP).
 */
static int cmd_newgroup(const char *host, uint16_t port, uint32_t uid, const char *gname)
{
    unsigned char msg[512], resp[256];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_NEW_GROUP);
    wire_put_u32_be(&p, uid);
    uint16_t gl = (uint16_t)strlen(gname);
    wire_put_u16_be(&p, gl);
    memcpy(p, gname, gl);
    p += gl;

    struct sockaddr_in6 lh;
    memset(&lh, 0, sizeof lh);
    int fd = tcp6_connect(host, port, &lh);
    if (fd < 0) {
        perror("connect");
        return -1;
    }

    size_t nout = (size_t)(p - msg);
    if (writen(fd, msg, nout) < 0) {
        close(fd);
        return -1;
    }

    size_t need = (size_t)(1 + 4 + 2 + PAROLES_IP6_BIN);
    if (readn(fd, resp, need) < 0) {
        close(fd);
        return -1;
    }

    const unsigned char *q = resp;
    size_t left = need;
    uint8_t c;
    if (wire_get_u8(&q, &left, &c) < 0 || c != PAROLES_CODEREQ_NEW_GROUP_OK) {
        close(fd);
        return -1;
    }
    uint32_t idg;
    uint16_t mport;
    if (wire_get_u32_be(&q, &left, &idg) < 0 || wire_get_u16_be(&q, &left, &mport) < 0) {
        close(fd);
        return -1;
    }
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, q, ip, sizeof ip);
    printf("OK idg=%u mcast=%s port=%u\n", idg, ip, (unsigned)mport);
    close(fd);
    return 0;
}

#endif /* !PAROLES_ACCEPT_REAL_CLE_113 */

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <host_ipv6> <port> reg <pseudo>|newgroup <uid> <nom_groupe>\n",
                argv[0]);
        return 1;
    }
    const char *host = argv[1];
    uint16_t port = (uint16_t)atoi(argv[2]);

#ifndef PAROLES_ACCEPT_REAL_CLE_113
    if (argc >= 5 && strcmp(argv[3], "reg") == 0)
        return cmd_reg(host, port, argv[4], NULL) == 0 ? 0 : 1;

    if (argc >= 6 && strcmp(argv[3], "newgroup") == 0)
        return cmd_newgroup(host, port, (uint32_t)strtoul(argv[4], NULL, 10), argv[5]) == 0 ? 0 : 1;
#endif

    int fd = tcp6_connect(host, port, NULL);
    if (fd < 0) {
        perror("tcp6_connect");
        return 1;
    }
    close(fd);
    return 0;
}
