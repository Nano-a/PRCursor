#include "../include/paroles_proto.h"
#include "net.h"
#include "wire.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int verbose;

static void vlog(const char *fmt, ...) {
    if (!verbose) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

#define MAX_USERS 1024
#define MAX_GROUPS 512
#define MAX_FEED 4096
#define MAX_BODY PAROLES_MAX_BODY

typedef struct {
    int used;
    uint32_t id;
    char nom[PAROLES_NOM_LEN];
    uint8_t cle[PAROLES_CLE_LEN];
    uint16_t udp_port;
    struct sockaddr_in6 reg_addr;
} User;

typedef struct {
    int is_reply;
    uint32_t author;
    uint32_t numb;
    uint32_t numr;
    uint16_t len;
    unsigned char data[];
} FeedItem;

typedef struct Post {
    uint32_t numb;
    uint32_t author;
    uint16_t len;
    unsigned char *body;
    uint32_t next_reply;
} Post;

typedef struct {
    int used;
    uint32_t idg;
    char *name;
    uint32_t admin_id;
    struct in6_addr mcast_ip;
    uint16_t mcast_port;
    int closed;
    uint32_t *mem;
    size_t nmem, cmem;
    uint32_t *pend;
    size_t npend, cpend;
    Post *posts;
    size_t nposts, cposts;
    uint32_t next_billet;
    FeedItem **feed;
    size_t nfeed, cfeed;
} Group;

static User users[MAX_USERS];
static Group groups[MAX_GROUPS];
static uint32_t next_uid = 1;
static uint32_t next_gid = 1;

static User *find_user(uint32_t id) {
    for (int i = 0; i < MAX_USERS; i++)
        if (users[i].used && users[i].id == id) return &users[i];
    return NULL;
}

static Group *find_group(uint32_t idg) {
    for (int i = 0; i < MAX_GROUPS; i++)
        if (groups[i].used && groups[i].idg == idg && !groups[i].closed) return &groups[i];
    return NULL;
}

static int cle_is_zero(const uint8_t *c) {
    for (int i = 0; i < PAROLES_CLE_LEN; i++)
        if (c[i]) return 0;
    return 1;
}

static void send_err(int fd) {
    unsigned char b[1 + PAROLES_ERR_TAIL];
    memset(b, 0, sizeof b);
    b[0] = PAROLES_CODEREQ_ERR;
    writen(fd, b, sizeof b);
}

static void send_err_msg(int fd, const char *msg) {
    if (!msg || !*msg) {
        send_err(fd);
        return;
    }
    size_t ml = strlen(msg);
    if (ml > 200) ml = 200;
    unsigned char buf[512];
    unsigned char *p = buf;
    wire_put_u8(&p, PAROLES_CODEREQ_ERR);
    wire_put_u16_be(&p, (uint16_t)ml);
    memcpy(p, msg, ml);
    p += ml;
    writen(fd, buf, (size_t)(p - buf));
}

static void send_ack(int fd) {
    unsigned char b[1 + PAROLES_ERR_TAIL];
    memset(b, 0, sizeof b);
    b[0] = PAROLES_CODEREQ_ACK;
    writen(fd, b, sizeof b);
}

static int group_is_member(Group *g, uint32_t uid) {
    for (size_t i = 0; i < g->nmem; i++)
        if (g->mem[i] == uid) return 1;
    return 0;
}

static int group_is_pending(Group *g, uint32_t uid) {
    for (size_t i = 0; i < g->npend; i++)
        if (g->pend[i] == uid) return 1;
    return 0;
}

static void group_add_member(Group *g, uint32_t uid) {
    if (group_is_member(g, uid)) return;
    if (g->nmem >= g->cmem) {
        g->cmem = g->cmem ? g->cmem * 2 : 8;
        g->mem = realloc(g->mem, g->cmem * sizeof *g->mem);
    }
    g->mem[g->nmem++] = uid;
}

static void group_add_pending(Group *g, uint32_t uid) {
    if (group_is_pending(g, uid)) return;
    if (g->npend >= g->cpend) {
        g->cpend = g->cpend ? g->cpend * 2 : 8;
        g->pend = realloc(g->pend, g->cpend * sizeof *g->pend);
    }
    g->pend[g->npend++] = uid;
}

static void group_remove_pending(Group *g, uint32_t uid) {
    size_t j = 0;
    for (size_t i = 0; i < g->npend; i++)
        if (g->pend[i] != uid) g->pend[j++] = g->pend[i];
    g->npend = j;
}

static void group_remove_member(Group *g, uint32_t uid) {
    size_t j = 0;
    for (size_t i = 0; i < g->nmem; i++)
        if (g->mem[i] != uid) g->mem[j++] = g->mem[i];
    g->nmem = j;
}

static void feed_push(Group *g, FeedItem *it) {
    if (g->nfeed >= g->cfeed) {
        g->cfeed = g->cfeed ? g->cfeed * 2 : 16;
        g->feed = realloc(g->feed, g->cfeed * sizeof *g->feed);
    }
    g->feed[g->nfeed++] = it;
}

static void notif_mcast(Group *g, uint16_t code) {
    int s = socket(AF_INET6, SOCK_DGRAM, 0);
    if (s < 0) return;
    struct sockaddr_in6 dst;
    memset(&dst, 0, sizeof dst);
    dst.sin6_family = AF_INET6;
    dst.sin6_port = htons(g->mcast_port);
    dst.sin6_addr = g->mcast_ip;
    unsigned char pkt[6];
    unsigned char *q = pkt;
    wire_put_u16_be(&q, code);
    wire_put_u32_be(&q, g->idg);
    udp6_send(s, &dst, pkt, sizeof pkt);
    close(s);
}

static void notif_udp_user(User *u, uint16_t code, uint32_t idg) {
    int s = socket(AF_INET6, SOCK_DGRAM, 0);
    if (s < 0) return;
    struct sockaddr_in6 dst = u->reg_addr;
    dst.sin6_port = htons(u->udp_port);
    unsigned char pkt[6];
    unsigned char *q = pkt;
    wire_put_u16_be(&q, code);
    wire_put_u32_be(&q, idg);
    udp6_send(s, &dst, pkt, sizeof pkt);
    close(s);
}

static Post *find_post(Group *g, uint32_t numb) {
    for (size_t i = 0; i < g->nposts; i++)
        if (g->posts[i].numb == numb) return &g->posts[i];
    return NULL;
}

static void close_group(Group *g) {
    if (g->closed) return;
    g->closed = 1;
    notif_mcast(g, PAROLES_NOTIF_CLOSE);
}

static int handle_reg(int fd, struct sockaddr_in6 *peer, const unsigned char *body, size_t blen) {
    if (blen != PAROLES_NOM_LEN + PAROLES_CLE_LEN) return -1;
    if (!cle_is_zero(body + PAROLES_NOM_LEN)) return -1;
    int slot = -1;
    for (int i = 0; i < MAX_USERS; i++)
        if (!users[i].used) {
            slot = i;
            break;
        }
    if (slot < 0) return -1;
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
    vlog("reg user %u nom=%.10s udp=%u\n", u->id, u->nom, u->udp_port);
    return writen(fd, out, (size_t)(p - out));
}

static int handle_new_group(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    User *u = find_user(uid);
    if (!u) return -1;
    if (blen < 2) return -1;
    uint16_t len;
    const unsigned char *q = body;
    size_t left = blen;
    if (wire_get_u16_be(&q, &left, &len) < 0) return -1;
    if (left < len || len == 0) return -1;
    int slot = -1;
    for (int i = 0; i < MAX_GROUPS; i++)
        if (!groups[i].used) {
            slot = i;
            break;
        }
    if (slot < 0) return -1;
    Group *g = &groups[slot];
    memset(g, 0, sizeof *g);
    g->used = 1;
    g->idg = next_gid++;
    g->name = malloc(len + 1);
    if (!g->name) return -1;
    memcpy(g->name, q, len);
    g->name[len] = 0;
    g->admin_id = uid;
    char addrbuf[64];
    snprintf(addrbuf, sizeof addrbuf, "ff0e::1:%u", g->idg);
    inet_pton(AF_INET6, addrbuf, &g->mcast_ip);
    g->mcast_port = (uint16_t)(30000u + (g->idg % 30000u));
    group_add_member(g, uid);
    unsigned char out[64];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_NEW_GROUP_OK);
    wire_put_u32_be(&p, g->idg);
    wire_put_u16_be(&p, g->mcast_port);
    memcpy(p, &g->mcast_ip, 16);
    p += 16;
    vlog("group %u '%s' admin=%u\n", g->idg, g->name, uid);
    return writen(fd, out, (size_t)(p - out));
}

static int handle_invite(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    if (blen < 8) return -1;
    const unsigned char *q = body;
    size_t left = blen;
    uint32_t idg, nb;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &nb) < 0) return -1;
    Group *g = find_group(idg);
    if (!g || g->admin_id != uid) return -1;
    if (left < nb * (4u + PAROLES_INV_PAD)) return -1;
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t invitee;
        if (wire_get_u32_be(&q, &left, &invitee) < 0) return -1;
        if (wire_expect_zeros(&q, &left, PAROLES_INV_PAD) < 0) return -1;
        if (!find_user(invitee)) return -1;
        group_add_pending(g, invitee);
        User *uu = find_user(invitee);
        if (uu) notif_udp_user(uu, PAROLES_NOTIF_INV_UDP, idg);
    }
    send_ack(fd);
    return 0;
}

static int handle_list_inv(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)body;
    if (blen) return -1;
    size_t cap = 8192;
    unsigned char *out = malloc(cap);
    if (!out) return -1;
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_INV_OK);
    unsigned char *nbp = p;
    wire_put_u32_be(&p, 0);
    uint32_t nb = 0;
    for (int gi = 0; gi < MAX_GROUPS; gi++) {
        Group *g = &groups[gi];
        if (!g->used || g->closed) continue;
        if (!group_is_pending(g, uid)) continue;
        uint16_t ln = (uint16_t)strlen(g->name);
        size_t need = (size_t)(p - out) + 4 + 2 + ln + PAROLES_NOM_LEN;
        if (need > cap) {
            cap = need * 2;
            size_t off = (size_t)(p - out);
            out = realloc(out, cap);
            p = out + off;
            nbp = out + 1 + 0;
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

static int handle_inv_ans(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    if (blen != 4 + 1) return -1;
    const unsigned char *q = body;
    size_t left = blen;
    uint32_t idg;
    uint8_t an;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u8(&q, &left, &an) < 0) return -1;
    Group *g = find_group(idg);
    if (!g) return -1;
    if (an == 2) {
        if (!group_is_member(g, uid)) return -1;
        if (g->admin_id == uid) {
            close_group(g);
            send_ack(fd);
            return 0;
        }
        group_remove_member(g, uid);
        send_ack(fd);
        notif_mcast(g, PAROLES_NOTIF_LEAVE);
        return 0;
    }
    if (!group_is_pending(g, uid)) return -1;
    if (an == 0) {
        group_remove_pending(g, uid);
        send_ack(fd);
        return 0;
    }
    if (an != 1) return -1;
    group_remove_pending(g, uid);
    group_add_member(g, uid);
    unsigned char out[8192];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_JOIN_OK);
    wire_put_u32_be(&p, g->idg);
    wire_put_u16_be(&p, g->mcast_port);
    memcpy(p, &g->mcast_ip, 16);
    p += 16;
    wire_put_u32_be(&p, (uint32_t)g->nmem);
    for (size_t i = 0; i < g->nmem; i++) {
        User *uu = find_user(g->mem[i]);
        if (!uu) return -1;
        wire_put_u32_be(&p, g->mem[i]);
        memcpy(p, uu->nom, PAROLES_NOM_LEN);
        p += PAROLES_NOM_LEN;
    }
    notif_mcast(g, PAROLES_NOTIF_JOIN);
    vlog("user %u joined group %u\n", uid, idg);
    return writen(fd, out, (size_t)(p - out));
}

static int handle_list_mem(int fd, uint32_t uid, uint32_t idg) {
    (void)uid;
    unsigned char out[8192];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_MEM_OK);
    if (idg == 0) {
        wire_put_u32_be(&p, 0);
        uint32_t nb = 0;
        unsigned char *nbspot = p;
        wire_put_u32_be(&p, 0);
        for (int i = 0; i < MAX_USERS; i++) {
            if (!users[i].used) continue;
            wire_put_u32_be(&p, users[i].id);
            memcpy(p, users[i].nom, PAROLES_NOM_LEN);
            p += PAROLES_NOM_LEN;
            nb++;
        }
        unsigned char *pp = nbspot;
        wire_put_u32_be(&pp, nb);
        return writen(fd, out, (size_t)(p - out));
    }
    Group *g = find_group(idg);
    if (!g) return -1;
    if (!group_is_member(g, uid)) return -1;
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, (uint32_t)g->nmem);
    for (size_t i = 0; i < g->nmem; i++) {
        User *uu = find_user(g->mem[i]);
        if (!uu) return -1;
        wire_put_u32_be(&p, g->mem[i]);
        memcpy(p, uu->nom, PAROLES_NOM_LEN);
        p += PAROLES_NOM_LEN;
    }
    return writen(fd, out, (size_t)(p - out));
}

static int handle_post(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    if (blen < 4 + 2) return -1;
    const unsigned char *q = body;
    size_t left = blen;
    uint32_t idg;
    uint16_t dlen;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u16_be(&q, &left, &dlen) < 0) return -1;
    if (left < dlen || dlen > MAX_BODY) return -1;
    Group *g = find_group(idg);
    if (!g || !group_is_member(g, uid)) return -1;
    if (g->nposts >= g->cposts) {
        g->cposts = g->cposts ? g->cposts * 2 : 8;
        g->posts = realloc(g->posts, g->cposts * sizeof *g->posts);
    }
    Post *po = &g->posts[g->nposts++];
    memset(po, 0, sizeof *po);
    po->numb = g->next_billet++;
    po->author = uid;
    po->len = dlen;
    po->body = malloc(dlen);
    if (!po->body) return -1;
    memcpy(po->body, q, dlen);
    size_t fisz = sizeof(FeedItem) + dlen;
    FeedItem *fi = malloc(fisz);
    if (!fi) return -1;
    fi->is_reply = 0;
    fi->author = uid;
    fi->numb = po->numb;
    fi->numr = 0;
    fi->len = dlen;
    memcpy(fi->data, q, dlen);
    feed_push(g, fi);
    unsigned char out[32];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_POST_OK);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, po->numb);
    notif_mcast(g, PAROLES_NOTIF_NEW_MSG);
    return writen(fd, out, (size_t)(p - out));
}

static int handle_reply(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    if (blen < 4 + 4 + 2) return -1;
    const unsigned char *q = body;
    size_t left = blen;
    uint32_t idg, numb;
    uint16_t dlen;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &numb) < 0) return -1;
    if (wire_get_u16_be(&q, &left, &dlen) < 0) return -1;
    if (left < dlen || dlen > MAX_BODY) return -1;
    Group *g = find_group(idg);
    if (!g || !group_is_member(g, uid)) return -1;
    Post *po = find_post(g, numb);
    if (!po) return -1;
    po->next_reply++;
    uint32_t numr = po->next_reply;
    size_t fisz = sizeof(FeedItem) + dlen;
    FeedItem *fi = malloc(fisz);
    if (!fi) return -1;
    fi->is_reply = 1;
    fi->author = uid;
    fi->numb = numb;
    fi->numr = numr;
    fi->len = dlen;
    memcpy(fi->data, q, dlen);
    feed_push(g, fi);
    unsigned char out[32];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_REPLY_OK);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, numb);
    wire_put_u32_be(&p, numr);
    notif_mcast(g, PAROLES_NOTIF_NEW_MSG);
    User *auth = find_user(po->author);
    if (auth && auth->id != uid) notif_udp_user(auth, PAROLES_NOTIF_FETCH, idg);
    return writen(fd, out, (size_t)(p - out));
}

static ssize_t feed_index_after(Group *g, uint32_t numb, uint32_t numr) {
    for (size_t i = 0; i < g->nfeed; i++) {
        FeedItem *fi = g->feed[i];
        if (fi->numb == numb && fi->numr == numr) return (ssize_t)i;
    }
    return -1;
}
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 0;
}
