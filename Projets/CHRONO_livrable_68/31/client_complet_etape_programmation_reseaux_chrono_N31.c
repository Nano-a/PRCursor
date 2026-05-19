/*
 * Snapshot CHRONO N°31 — dossier **30** ;
 * mise en évidence **`cmd_ans`** (**`ans <uid> <idg> <0|1|2>`**), voir `extrait_scenario_ans.sh`.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "paroles_proto.h"
#include "net.h"
#include "wire.h"

#ifndef PAROLES_ACCEPT_REAL_CLE_113

#define MAX_CLI_INVITE 128

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

/*
 * extrait_client_codereq_5.c ; one_cmd(...) -> tcp6_connect/writen/readn puis close.
 */
static int cmd_invite(const char *host, uint16_t port, uint32_t admin, uint32_t idg, int ninv,
                      uint32_t *ids)
{
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

    size_t ak = (size_t)(1 + PAROLES_ERR_TAIL);
    if (readn(fd, resp, ak) < 0) {
        close(fd);
        return -1;
    }
    if (resp[0] != PAROLES_CODEREQ_ACK) {
        close(fd);
        return -1;
    }

    printf("OK invite ACK\n");
    close(fd);
    return 0;
}

/* extrait_client_codereq_6_7 — LIST_INV puis LIST_INV_OK */
static int cmd_listinv(const char *host, uint16_t port, uint32_t uid)
{
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_INV);
    wire_put_u32_be(&p, uid);

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

    ssize_t n = 0;
    ssize_t r;
    while (n < (ssize_t)sizeof resp &&
           (r = recv(fd, resp + n, sizeof resp - (size_t)n, 0)) > 0)
        n += r;
    if (n < (ssize_t)(1 + 4)) {
        close(fd);
        return -1;
    }

    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1u;
    uint32_t nb;
    if (wire_get_u32_be(&q, &left, &nb) < 0)
        return -1;

    printf("invitations %u\n", nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t ig;
        uint16_t ln;
        if (wire_get_u32_be(&q, &left, &ig) < 0)
            return -1;
        if (wire_get_u16_be(&q, &left, &ln) < 0)
            return -1;
        if (left < (size_t)ln + PAROLES_NOM_LEN)
            return -1;
        char gn[512];
        if (ln >= sizeof gn)
            return -1;
        memcpy(gn, q, ln);
        gn[ln] = '\0';
        q += ln;
        char adm[PAROLES_NOM_LEN + 1];
        memcpy(adm, q, PAROLES_NOM_LEN);
        adm[PAROLES_NOM_LEN] = '\0';
        q += PAROLES_NOM_LEN;
        left -= (size_t)ln + PAROLES_NOM_LEN;
        printf("  idg=%u groupe=%s admin=%s\n", ig, gn, adm);
    }
    close(fd);
    return 0;
}

/*
 * extrait_client_codereq_8 — INV_ANS ; ACK 24 ou JOIN_OK 9.
 */
static int cmd_ans(const char *host, uint16_t port, uint32_t uid, uint32_t idg, int an)
{
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_INV_ANS);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u8(&p, (uint8_t)an);

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

    ssize_t n = 0;
    ssize_t r;
    while (n < (ssize_t)sizeof resp &&
           (r = recv(fd, resp + n, sizeof resp - (size_t)n, 0)) > 0)
        n += r;
    if (n < 1) {
        close(fd);
        return -1;
    }

    if (resp[0] == PAROLES_CODEREQ_ACK) {
        printf("OK ack\n");
        close(fd);
        return 0;
    }
    if (resp[0] == PAROLES_CODEREQ_JOIN_OK) {
        printf("OK join\n");
        close(fd);
        return 0;
    }
    close(fd);
    return -1;
}

/* extrait_client_codereq_10_11 — LIST_MEM / LIST_MEM_OK */
static int cmd_listmem(const char *host, uint16_t port, uint32_t uid, uint32_t idg)
{
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_MEM);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);

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

    ssize_t n = 0;
    ssize_t r;
    while (n < (ssize_t)sizeof resp &&
           (r = recv(fd, resp + n, sizeof resp - (size_t)n, 0)) > 0)
        n += r;
    if (n < (ssize_t)(1 + 4 + 4)) {
        close(fd);
        return -1;
    }

    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1u;
    uint32_t ig, nb;
    if (wire_get_u32_be(&q, &left, &ig) < 0)
        return -1;
    if (wire_get_u32_be(&q, &left, &nb) < 0)
        return -1;

    printf("membres idg=%u nb=%u\n", ig, nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t ui;
        if (wire_get_u32_be(&q, &left, &ui) < 0)
            return -1;
        if (left < PAROLES_NOM_LEN)
            return -1;
        char nm[PAROLES_NOM_LEN + 1];
        memcpy(nm, q, PAROLES_NOM_LEN);
        nm[PAROLES_NOM_LEN] = 0;
        q += PAROLES_NOM_LEN;
        left -= PAROLES_NOM_LEN;
        printf("  %u %s\n", ui, nm);
    }
    close(fd);
    return 0;
}

/* extrait_client_codereq_12_13 — POST / POST_OK */
static int cmd_post(const char *host, uint16_t port, uint32_t uid, uint32_t idg, const char *txt)
{
    unsigned char msg[PAROLES_MAX_BODY + 64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_POST);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    uint16_t tl = (uint16_t)strlen(txt);
    wire_put_u16_be(&p, tl);
    memcpy(p, txt, tl);
    p += tl;

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

    unsigned char rr[64];
    size_t need = (size_t)(1 + 4 + 4);
    if (readn(fd, rr, need) < 0) {
        close(fd);
        return -1;
    }
    if (rr[0] != PAROLES_CODEREQ_POST_OK) {
        close(fd);
        return -1;
    }
    const unsigned char *q = rr + 1;
    size_t left = need - 1u;
    uint32_t ig, numb;
    if (wire_get_u32_be(&q, &left, &ig) < 0 || wire_get_u32_be(&q, &left, &numb) < 0) {
        close(fd);
        return -1;
    }
    printf("OK billet numb=%u\n", numb);
    close(fd);
    return 0;
}

/* extrait_client_codereq_14_15 — REPLY / REPLY_OK */
static int cmd_reply(const char *host, uint16_t port, uint32_t uid, uint32_t idg, uint32_t numb,
                     const char *txt)
{
    unsigned char msg[PAROLES_MAX_BODY + 64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_REPLY);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, numb);
    uint16_t tl = (uint16_t)strlen(txt);
    wire_put_u16_be(&p, tl);
    memcpy(p, txt, tl);
    p += tl;

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

    unsigned char rr[64];
    size_t need = (size_t)(1 + 4 + 4 + 4);
    if (readn(fd, rr, need) < 0) {
        close(fd);
        return -1;
    }
    if (rr[0] != PAROLES_CODEREQ_REPLY_OK) {
        close(fd);
        return -1;
    }
    const unsigned char *qx = rr + 1;
    size_t lf = need - 1u;
    uint32_t ig, nb, nr;
    if (wire_get_u32_be(&qx, &lf, &ig) < 0 || wire_get_u32_be(&qx, &lf, &nb) < 0 ||
        wire_get_u32_be(&qx, &lf, &nr) < 0) {
        close(fd);
        return -1;
    }
    printf("OK reply numb=%u numr=%u\n", nb, nr);
    close(fd);
    return 0;
}

/* extrait_client_codereq_16_17 — FEED / FEED_OK */
static int cmd_feed(const char *host, uint16_t port, uint32_t uid, uint32_t idg, uint32_t numb,
                    uint32_t numr)
{
    unsigned char msg[64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_FEED);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, numb);
    wire_put_u32_be(&p, numr);

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

    unsigned char resp[65536];
    ssize_t n = 0;
    ssize_t rr;
    while (n < (ssize_t)sizeof resp && (rr = recv(fd, resp + n, sizeof resp - (size_t)n, 0)) > 0)
        n += rr;
    close(fd);

    if (n < (ssize_t)(1 + 4 + 4))
        return -1;
    if (resp[0] != PAROLES_CODEREQ_FEED_OK)
        return -1;

    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1u;
    uint32_t ig, nb;
    if (wire_get_u32_be(&q, &left, &ig) < 0 || wire_get_u32_be(&q, &left, &nb) < 0)
        return -1;

    printf("feed idg=%u nb=%u\n", ig, nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t author, nb2, nr;
        uint16_t dl;
        if (wire_get_u32_be(&q, &left, &author) < 0 || wire_get_u32_be(&q, &left, &nb2) < 0 ||
            wire_get_u32_be(&q, &left, &nr) < 0 || wire_get_u16_be(&q, &left, &dl) < 0)
            return -1;
        if (left < dl)
            return -1;
        char buf[PAROLES_MAX_BODY + 1];
        memcpy(buf, q, dl);
        buf[dl] = 0;
        q += dl;
        left -= dl;
        printf("  [%u billet=%u rep=%u] %s\n", author, nb2, nr, buf);
    }
    return 0;
}

#endif /* !PAROLES_ACCEPT_REAL_CLE_113 */

int main(int argc, char **argv)
{
#ifndef PAROLES_ACCEPT_REAL_CLE_113
    if (argc < 3) {
        fprintf(stderr,
                "usage: %s <host> <port> reg …|…|feed <uid> <idg> <numb> <numr>|post …|reply …\n",
                argv[0]);
        return 1;
    }
    const char *host = argv[1];
    uint16_t port = (uint16_t)atoi(argv[2]);

    if (argc >= 5 && strcmp(argv[3], "reg") == 0)
        return cmd_reg(host, port, argv[4], NULL) == 0 ? 0 : 1;

    if (argc >= 6 && strcmp(argv[3], "newgroup") == 0)
        return cmd_newgroup(host, port, (uint32_t)strtoul(argv[4], NULL, 10), argv[5]) == 0 ? 0 : 1;

    if (argc >= 7 && strcmp(argv[3], "invite") == 0) {
        uint32_t admin = (uint32_t)strtoul(argv[4], NULL, 10);
        uint32_t idg = (uint32_t)strtoul(argv[5], NULL, 10);
        long nlv = strtol(argv[6], NULL, 10);
        if (nlv < 0 || nlv > MAX_CLI_INVITE)
            return 1;
        int ninv = (int)nlv;
        if (argc != 7 + ninv) {
            fprintf(stderr, "%s invite: attends %d uid(s), argv mal formé.\n", argv[0], ninv);
            return 1;
        }
        uint32_t ids[MAX_CLI_INVITE];
        for (int i = 0; i < ninv; i++)
            ids[i] = (uint32_t)strtoul(argv[7 + i], NULL, 10);
        return cmd_invite(host, port, admin, idg, ninv, ids) == 0 ? 0 : 1;
    }

    if (argc == 5 && strcmp(argv[3], "listinv") == 0)
        return cmd_listinv(host, port, (uint32_t)strtoul(argv[4], NULL, 10)) == 0 ? 0 : 1;

    if (argc == 6 && strcmp(argv[3], "listmem") == 0) {
        uint32_t uu = (uint32_t)strtoul(argv[4], NULL, 10);
        uint32_t idgg = (uint32_t)strtoul(argv[5], NULL, 10);
        return cmd_listmem(host, port, uu, idgg) == 0 ? 0 : 1;
    }

    if (argc >= 7 && strcmp(argv[3], "post") == 0) {
        uint32_t uu = (uint32_t)strtoul(argv[4], NULL, 10);
        uint32_t idgg = (uint32_t)strtoul(argv[5], NULL, 10);
        size_t tot = 0;
        for (int ai = 6; ai < argc; ai++)
            tot += strlen(argv[ai]) + ((ai > 6) ? 1u : 0u);
        char *joined = malloc(tot + 1);
        if (!joined)
            return 1;
        char *wp = joined;
        for (int ai = 6; ai < argc; ai++) {
            if (ai > 6)
                *wp++ = ' ';
            size_t L = strlen(argv[ai]);
            memcpy(wp, argv[ai], L);
            wp += L;
        }
        *wp = '\0';
        int ok = cmd_post(host, port, uu, idgg, joined) == 0 ? 0 : 1;
        free(joined);
        return ok;
    }

    if (argc == 8 && strcmp(argv[3], "feed") == 0) {
        uint32_t uu = (uint32_t)strtoul(argv[4], NULL, 10);
        uint32_t idgg = (uint32_t)strtoul(argv[5], NULL, 10);
        uint32_t nba = (uint32_t)strtoul(argv[6], NULL, 10);
        uint32_t nra = (uint32_t)strtoul(argv[7], NULL, 10);
        return cmd_feed(host, port, uu, idgg, nba, nra) == 0 ? 0 : 1;
    }

    if (argc >= 8 && strcmp(argv[3], "reply") == 0) {
        uint32_t uu = (uint32_t)strtoul(argv[4], NULL, 10);
        uint32_t idgg = (uint32_t)strtoul(argv[5], NULL, 10);
        uint32_t numb = (uint32_t)strtoul(argv[6], NULL, 10);
        size_t tot = 0;
        for (int ai = 7; ai < argc; ai++)
            tot += strlen(argv[ai]) + ((ai > 7) ? 1u : 0u);
        char *joined = malloc(tot + 1);
        if (!joined)
            return 1;
        char *wp = joined;
        for (int ai = 7; ai < argc; ai++) {
            if (ai > 7)
                *wp++ = ' ';
            size_t L = strlen(argv[ai]);
            memcpy(wp, argv[ai], L);
            wp += L;
        }
        *wp = '\0';
        int ok = cmd_reply(host, port, uu, idgg, numb, joined) == 0 ? 0 : 1;
        free(joined);
        return ok;
    }

    if (argc == 7 && strcmp(argv[3], "ans") == 0) {
        uint32_t ua = (uint32_t)strtoul(argv[4], NULL, 10);
        uint32_t idgg = (uint32_t)strtoul(argv[5], NULL, 10);
        int an = atoi(argv[6]);
        return cmd_ans(host, port, ua, idgg, an) == 0 ? 0 : 1;
    }

    int fd = tcp6_connect(host, port, NULL);
    if (fd < 0) {
        perror("tcp6_connect");
        return 1;
    }
    close(fd);
    return 0;
#else
    fprintf(stderr, "snapshot CHRONO 19 étape 1 : compiler sans PAROLES_ACCEPT_REAL_CLE_113.\n");
    (void)argc;
    (void)argv;
    return 1;
#endif /* !PAROLES_ACCEPT_REAL_CLE_113 */
}
