/*
 * Snapshot CHRONO N°13–20 — base **dossier 19** + CODEREQ **6–7** (liste invitations).
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

#define MAX_USERS 1024
#define MAX_GROUPS 512
#define MAX_GM 512
#define MAX_INV_PER_MSG 256

typedef struct {
    int used;
    uint32_t id;
    char nom[PAROLES_NOM_LEN];
    uint8_t cle[PAROLES_CLE_LEN];
    uint16_t udp_port;
    struct sockaddr_in6 reg_addr;
} User;

typedef struct {
    int used;
    int closed;
    uint32_t idg;
    char *name;
    uint32_t admin_id;
    struct in6_addr mcast_ip;
    uint16_t mcast_port;
    uint32_t mem[MAX_GM];
    size_t nmem;
    uint32_t pend[MAX_GM];
    size_t npend;
} Group;

static User users[MAX_USERS];
static Group groups[MAX_GROUPS];
static uint32_t next_uid = 1;
static uint32_t next_gid = 1;

#ifndef PAROLES_ACCEPT_REAL_CLE_113
static int cle_is_zero(const uint8_t *c) {
    for (int i = 0; i < PAROLES_CLE_LEN; i++)
        if (c[i])
            return 0;
    return 1;
}
#endif

static User *find_user(uint32_t id) {
    for (int i = 0; i < MAX_USERS; i++)
        if (users[i].used && users[i].id == id)
            return &users[i];
    return NULL;
}

static Group *find_group(uint32_t idg) {
    for (int i = 0; i < MAX_GROUPS; i++)
        if (groups[i].used && !groups[i].closed && groups[i].idg == idg)
            return &groups[i];
    return NULL;
}

static int group_is_member(Group *g, uint32_t uid) {
    for (size_t i = 0; i < g->nmem; i++)
        if (g->mem[i] == uid)
            return 1;
    return 0;
}

static void group_add_member(Group *g, uint32_t uid) {
    if (group_is_member(g, uid))
        return;
    if (g->nmem >= MAX_GM)
        return;
    g->mem[g->nmem++] = uid;
}

static int group_is_pending(Group *g, uint32_t uid) {
    for (size_t i = 0; i < g->npend; i++)
        if (g->pend[i] == uid)
            return 1;
    return 0;
}

static void group_add_pending(Group *g, uint32_t uid) {
    if (group_is_pending(g, uid))
        return;
    if (g->npend >= MAX_GM)
        return;
    g->pend[g->npend++] = uid;
}

static void notif_udp_user(User *u, uint16_t code, uint32_t idg) {
    int s = socket(AF_INET6, SOCK_DGRAM, 0);
    if (s < 0)
        return;
    struct sockaddr_in6 dst = u->reg_addr;
    dst.sin6_port = htons(u->udp_port);
    unsigned char pkt[6];
    unsigned char *q = pkt;
    wire_put_u16_be(&q, code);
    wire_put_u32_be(&q, idg);
    (void)sendto(s, pkt, sizeof pkt, 0, (struct sockaddr *)&dst, sizeof dst);
    close(s);
}

static void send_ack(int fd) {
    unsigned char b[1 + PAROLES_ERR_TAIL];
    memset(b, 0, sizeof b);
    b[0] = PAROLES_CODEREQ_ACK;
    (void)writen(fd, b, sizeof b);
}

#ifndef PAROLES_ACCEPT_REAL_CLE_113

static int handle_reg_codereq_1(int cfd, struct sockaddr_in6 *peer,
                                const unsigned char *body, size_t blen)
{
    if (blen != PAROLES_NOM_LEN + PAROLES_CLE_LEN)
        return -1;
    if (!cle_is_zero(body + PAROLES_NOM_LEN))
        return -1;

    int slot = -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (!users[i].used) {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        return -1;

    User *u = &users[slot];
    memset(u, 0, sizeof *u);
    u->used = 1;
    u->id = next_uid++;
    memcpy(u->nom, body, PAROLES_NOM_LEN);
    memset(u->cle, 0, sizeof u->cle);

    u->udp_port = (uint16_t)(20000u + (u->id % 45000u));
    u->reg_addr = *peer;
    u->reg_addr.sin6_port = htons(u->udp_port);

    unsigned char out[256];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_REG_OK);
    wire_put_u32_be(&p, u->id);
    wire_put_u16_be(&p, u->udp_port);
    wire_put_zeros(&p, PAROLES_CLE_LEN);
    return writen(cfd, out, (size_t)(p - out));
}

static int mcast_pair_conflict(Group *ignore, struct in6_addr *ip, uint16_t port)
{
    for (int i = 0; i < MAX_GROUPS; i++) {
        Group *gg = &groups[i];
        if (!gg->used || gg == ignore)
            continue;
        if (gg->mcast_port == port &&
            memcmp(&gg->mcast_ip, ip, sizeof *ip) == 0)
            return 1;
    }
    return 0;
}

static int handle_new_group_tcp(int cfd, uint32_t uid, const unsigned char *body, size_t blen)
{
    User *u = find_user(uid);
    if (!u)
        return -1;

    uint16_t glen;
    const unsigned char *q = body;
    size_t left = blen;
    if (wire_get_u16_be(&q, &left, &glen) < 0 || glen == 0)
        return -1;
    if (left < glen)
        return -1;

    int slot = -1;
    for (int i = 0; i < MAX_GROUPS; i++)
        if (!groups[i].used) {
            slot = i;
            break;
        }
    if (slot < 0)
        return -1;

    Group *g = &groups[slot];
    memset(g, 0, sizeof *g);
    g->used = 1;
    g->closed = 0;
    g->idg = next_gid++;

    g->name = malloc((size_t)glen + 1u);
    if (!g->name)
        return -1;
    memcpy(g->name, q, glen);
    g->name[glen] = '\0';

    g->admin_id = uid;

    {
        char addrbuf[64];
        if (snprintf(addrbuf, sizeof addrbuf, "ff0e::1:%u", g->idg) >= (int)sizeof addrbuf)
            goto fail_ng;
        if (inet_pton(AF_INET6, addrbuf, &g->mcast_ip) != 1)
            goto fail_ng;
        g->mcast_port = (uint16_t)(30000u + (g->idg % 30000u));
        if (mcast_pair_conflict(g, &g->mcast_ip, g->mcast_port))
            goto fail_ng;
    }

    group_add_member(g, uid);

    unsigned char out[256];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_NEW_GROUP_OK);
    wire_put_u32_be(&p, g->idg);
    wire_put_u16_be(&p, g->mcast_port);
    memcpy(p, &g->mcast_ip, (size_t)PAROLES_IP6_BIN);
    p += PAROLES_IP6_BIN;

    return writen(cfd, out, (size_t)(p - out));

fail_ng:
    free(g->name);
    memset(g, 0, sizeof *g);
    return -1;
}

static int handle_invite(int fd, uint32_t uid, const unsigned char *body, size_t blen)
{
    if (blen < 8)
        return -1;
    const unsigned char *q = body;
    size_t left = blen;
    uint32_t idg, nb;
    if (wire_get_u32_be(&q, &left, &idg) < 0)
        return -1;
    if (wire_get_u32_be(&q, &left, &nb) < 0)
        return -1;

    Group *g = find_group(idg);
    if (!g || g->admin_id != uid)
        return -1;
    if (nb > MAX_INV_PER_MSG || left < nb * (4u + PAROLES_INV_PAD))
        return -1;

    for (uint32_t i = 0; i < nb; i++) {
        uint32_t invitee;
        if (wire_get_u32_be(&q, &left, &invitee) < 0)
            return -1;
        if (wire_expect_zeros(&q, &left, (size_t)PAROLES_INV_PAD) < 0)
            return -1;
        if (!find_user(invitee))
            return -1;
        group_add_pending(g, invitee);
        User *uu = find_user(invitee);
        if (uu)
            notif_udp_user(uu, PAROLES_NOTIF_INV_UDP, idg);
    }

    send_ack(fd);
    return 0;
}

/* CHRONO N°20 — extrait_server_codereq_6_7.c (writen à la place de conn_writen) */
static int handle_list_inv(int fd, uint32_t uid, const unsigned char *body, size_t blen)
{
    (void)body;
    if (blen)
        return -1;
    size_t cap = 8192;
    unsigned char *out = malloc(cap);
    if (!out)
        return -1;
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_INV_OK);
    unsigned char *nbp = p;
    wire_put_u32_be(&p, 0);
    uint32_t nb = 0;

    for (int gi = 0; gi < MAX_GROUPS; gi++) {
        Group *g = &groups[gi];
        if (!g->used || g->closed)
            continue;
        if (!group_is_pending(g, uid))
            continue;
        uint16_t ln = (uint16_t)strlen(g->name);
        size_t need = (size_t)(p - out) + 4 + 2 + ln + PAROLES_NOM_LEN;
        if (need > cap) {
            cap = need * 2;
            size_t off = (size_t)(p - out);
            unsigned char *nout = realloc(out, cap);
            if (!nout) {
                free(out);
                return -1;
            }
            out = nout;
            p = out + off;
            nbp = out + 1;
        }
        wire_put_u32_be(&p, g->idg);
        wire_put_u16_be(&p, ln);
        memcpy(p, g->name, ln);
        p += ln;
        User *adm = find_user(g->admin_id);
        if (adm)
            memcpy(p, adm->nom, PAROLES_NOM_LEN);
        else
            memset(p, 0, PAROLES_NOM_LEN);
        p += PAROLES_NOM_LEN;
        nb++;
    }

    unsigned char *p2 = nbp;
    wire_put_u32_be(&p2, nb);

    int r = writen(fd, out, (size_t)(p - out));
    free(out);
    return r;
}
#endif /* !PAROLES_ACCEPT_REAL_CLE_113 */

static int serve_one_codereq(int cfd, struct sockaddr_in6 *peer)
{
    unsigned char codebuf[1];
    if (readn(cfd, codebuf, 1) < 0)
        return -1;
    uint8_t code = codebuf[0];

    if (code == PAROLES_CODEREQ_REG) {
        unsigned char body[PAROLES_NOM_LEN + PAROLES_CLE_LEN];
        if (readn(cfd, body, sizeof body) < 0)
            return -1;
        return handle_reg_codereq_1(cfd, peer, body, sizeof body);
    }
    if (code == PAROLES_CODEREQ_NEW_GROUP) {
        unsigned char uid_be[4];
        if (readn(cfd, uid_be, 4) < 0)
            return -1;
        const unsigned char *qp = uid_be;
        size_t lz = sizeof uid_be;
        uint32_t uid;
        if (wire_get_u32_be(&qp, &lz, &uid) < 0)
            return -1;

        unsigned char len_be[2];
        if (readn(cfd, len_be, 2) < 0)
            return -1;
        qp = len_be;
        lz = sizeof len_be;
        uint16_t glen;
        if (wire_get_u16_be(&qp, &lz, &glen) < 0 || glen == 0)
            return -1;

        size_t bodysz = (size_t)2u + (size_t)glen;
        unsigned char *bodydyn = malloc(bodysz);
        if (!bodydyn)
            return -1;
        memcpy(bodydyn, len_be, 2);
        if (readn(cfd, bodydyn + 2, glen) < 0) {
            free(bodydyn);
            return -1;
        }
        int rr = handle_new_group_tcp(cfd, uid, bodydyn, bodysz);
        free(bodydyn);
        return rr;
    }
    if (code == PAROLES_CODEREQ_INVITE) {
        unsigned char head[12];
        if (readn(cfd, head, sizeof head) < 0)
            return -1;

        const unsigned char *qp = head;
        size_t lz = sizeof head;
        uint32_t admin, idg2, nb2;
        if (wire_get_u32_be(&qp, &lz, &admin) < 0 ||
            wire_get_u32_be(&qp, &lz, &idg2) < 0 ||
            wire_get_u32_be(&qp, &lz, &nb2) < 0)
            return -1;

        if (nb2 > MAX_INV_PER_MSG)
            return -1;
        size_t tailsz = (size_t)nb2 * (4u + PAROLES_INV_PAD);
        unsigned char *ib = malloc((size_t)8u + tailsz);
        if (!ib)
            return -1;
        memcpy(ib, head + 4, 8);
        if (tailsz && readn(cfd, ib + 8, tailsz) < 0) {
            free(ib);
            return -1;
        }
        int r2 = handle_invite(cfd, admin, ib, (size_t)8u + tailsz);
        free(ib);
        return r2;
    }
    if (code == PAROLES_CODEREQ_LIST_INV) {
        unsigned char u4[4];
        if (readn(cfd, u4, 4) < 0)
            return -1;
        const unsigned char *qp2 = u4;
        size_t lz2 = sizeof u4;
        uint32_t uid_l;
        if (wire_get_u32_be(&qp2, &lz2, &uid_l) < 0)
            return -1;
        return handle_list_inv(cfd, uid_l, NULL, 0);
    }

    (void)peer;
    return -1;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <bind_ipv6> <port>\n", argv[0]);
        return 1;
    }
    uint16_t port = (uint16_t)atoi(argv[2]);
    int s = tcp6_listen(argv[1], port);
    if (s < 0) {
        perror("tcp6_listen");
        return 1;
    }
    for (;;) {
        struct sockaddr_in6 peer;
        socklen_t plen = sizeof peer;
        int cfd = tcp6_accept(s, &peer, &plen);
        if (cfd < 0)
            continue;

        (void)serve_one_codereq(cfd, &peer);
        close(cfd);
    }
}
