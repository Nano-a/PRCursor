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
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 0;
}
