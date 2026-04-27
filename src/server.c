/*
 * Serveur Paroles — étape 1 : TCP/UDP IPv6, sans TLS ni signatures.
 * Référence : sujet PR6 « Paroles ».
 */

#include "../include/paroles_proto.h"
#include "net.h"
#include "wire.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_USERS 1024
#define MAX_GROUPS 512
#define MAX_PENDING 4096
#define MAX_FEED 8192
#define MAX_GROUP_MEMBERS 512
#define MAX_POST_BODY 16384
#define READ_TIMEOUT_MS 30000

typedef struct {
    int used;
    uint16_t id;
    char nom[PAROLES_NOM_LEN];
    uint8_t cle[PAROLES_CLE_LEN];
    uint16_t udp_port;
    struct sockaddr_in6 peer;
} User;

typedef struct {
    uint16_t author;
    uint16_t numb;
    uint16_t numr;
    size_t len;
    uint8_t *data;
} FeedItem;

typedef struct {
    int used;
    uint16_t id;
    uint16_t admin_id;
    char *gname;
    struct in6_addr mcast;
    uint16_t mcast_port;
    uint16_t memb[MAX_GROUP_MEMBERS];
    size_t nmemb;
    FeedItem *feed;
    size_t nfeed;
    size_t capfeed;
    int next_post;
} Group;

typedef struct {
    uint16_t uid;
    uint16_t gid;
} Pending;

typedef struct {
    User users[MAX_USERS];
    uint16_t next_user_id;
    Group groups[MAX_GROUPS];
    uint16_t next_group_id;
    Pending pending[MAX_PENDING];
    size_t npending;
    int udp_sock;
} Server;

static User *user_find(Server *s, uint16_t id) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (s->users[i].used && s->users[i].id == id)
            return &s->users[i];
    }
    return NULL;
}

static Group *group_find(Server *s, uint16_t gid) {
    for (int i = 0; i < MAX_GROUPS; i++) {
        if (s->groups[i].used && s->groups[i].id == gid)
            return &s->groups[i];
    }
    return NULL;
}

static int is_member(const Group *g, uint16_t uid) {
    for (size_t i = 0; i < g->nmemb; i++) {
        if (g->memb[i] == uid)
            return 1;
    }
    return 0;
}

static void group_free_feed(Group *g) {
    for (size_t i = 0; i < g->nfeed; i++)
        free(g->feed[i].data);
    free(g->feed);
    g->feed = NULL;
    g->nfeed = g->capfeed = 0;
}

static void pending_purge_gid(Server *s, uint16_t gid) {
    size_t w = 0;
    for (size_t i = 0; i < s->npending; i++) {
        if (s->pending[i].gid == gid)
            continue;
        s->pending[w++] = s->pending[i];
    }
    s->npending = w;
}

static void group_destroy(Server *s, Group *g) {
    pending_purge_gid(s, g->id);
    free(g->gname);
    g->gname = NULL;
    group_free_feed(g);
    memset(g->memb, 0, sizeof(g->memb));
    g->nmemb = 0;
    g->used = 0;
}

static void mcast_addr_for_group(uint16_t gid, struct in6_addr *out) {
    memset(out, 0, sizeof(*out));
    out->s6_addr[0] = 0xff;
    out->s6_addr[1] = 0x15;
    out->s6_addr[14] = (uint8_t)((gid >> 8) & 0xff);
    out->s6_addr[15] = (uint8_t)(gid & 0xff);
}

static int udp_send_notif(Server *s, const struct sockaddr_in6 *dst, uint16_t code,
                          uint16_t idg) {
    uint8_t p[4];
    wire_put16(p, code);
    wire_put16(p + 2, idg);
    return sendto(s->udp_sock, p, 4, 0, (const struct sockaddr *)dst,
                  sizeof(*dst));
}

static void notify_user(Server *s, User *u, uint16_t code, uint16_t idg) {
    struct sockaddr_in6 a = u->peer;
    a.sin6_port = htons(u->udp_port);
    (void)udp_send_notif(s, &a, code, idg);
}

static void notify_group_mcast(Server *s, Group *g, uint16_t code) {
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    a.sin6_addr = g->mcast;
    a.sin6_port = htons(g->mcast_port);
    (void)udp_send_notif(s, &a, code, g->id);
}

static int send_err(int fd) {
    uint8_t b[4];
    wire_put16(b, PAROLES_ERR);
    wire_put16(b + 2, 0);
    return net_write_all(fd, b, sizeof(b));
}

static uint16_t alloc_udp_port(Server *s, uint16_t uid) {
    (void)s;
    return (uint16_t)(40000u + (uid % 20000u));
}

static User *user_add(Server *s, const char nom[PAROLES_NOM_LEN],
                      const uint8_t cle[PAROLES_CLE_LEN], const struct sockaddr_in6 *peer) {
    int slot = -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (!s->users[i].used) {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        return NULL;
    User *u = &s->users[slot];
    memset(u, 0, sizeof(*u));
    u->used = 1;
    u->id = s->next_user_id++;
    memcpy(u->nom, nom, PAROLES_NOM_LEN);
    memcpy(u->cle, cle, PAROLES_CLE_LEN);
    u->udp_port = alloc_udp_port(s, u->id);
    u->peer = *peer;
    return u;
}

static Group *group_add(Server *s, uint16_t admin_id, const char *name, size_t name_len) {
    int slot = -1;
    for (int i = 0; i < MAX_GROUPS; i++) {
        if (!s->groups[i].used) {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        return NULL;
    Group *g = &s->groups[slot];
    memset(g, 0, sizeof(*g));
    g->used = 1;
    g->id = ++s->next_group_id; /* strictement positif */
    g->admin_id = admin_id;
    g->gname = malloc(name_len + 1);
    if (!g->gname)
        return NULL;
    memcpy(g->gname, name, name_len);
    g->gname[name_len] = '\0';
    mcast_addr_for_group(g->id, &g->mcast);
    g->mcast_port = (uint16_t)(30000u + (g->id % 20000u));
    g->memb[0] = admin_id;
    g->nmemb = 1;
    g->next_post = 0;
    return g;
}

static int pending_add(Server *s, uint16_t uid, uint16_t gid) {
    if (s->npending >= MAX_PENDING)
        return -1;
    s->pending[s->npending].uid = uid;
    s->pending[s->npending].gid = gid;
    s->npending++;
    return 0;
}

static void pending_remove_for(Server *s, uint16_t uid, uint16_t gid) {
    size_t w = 0;
    for (size_t i = 0; i < s->npending; i++) {
        if (s->pending[i].uid == uid && s->pending[i].gid == gid)
            continue;
        s->pending[w++] = s->pending[i];
    }
    s->npending = w;
}

static int pending_has(const Server *s, uint16_t uid, uint16_t gid) {
    for (size_t i = 0; i < s->npending; i++) {
        if (s->pending[i].uid == uid && s->pending[i].gid == gid)
            return 1;
    }
    return 0;
}

static uint16_t post_author(const Group *g, uint16_t numb) {
    for (size_t i = 0; i < g->nfeed; i++) {
        if (g->feed[i].numb == numb && g->feed[i].numr == 0)
            return g->feed[i].author;
    }
    return 0;
}

static int feed_push(Group *g, uint16_t author, uint16_t numb, uint16_t numr,
                     const uint8_t *data, size_t len) {
    if (g->nfeed >= MAX_FEED)
        return -1;
    if (g->nfeed == g->capfeed) {
        size_t ncap = g->capfeed ? g->capfeed * 2 : 16;
        FeedItem *nf = realloc(g->feed, ncap * sizeof(FeedItem));
        if (!nf)
            return -1;
        g->feed = nf;
        g->capfeed = ncap;
    }
    FeedItem *it = &g->feed[g->nfeed];
    it->author = author;
    it->numb = numb;
    it->numr = numr;
    it->len = len;
    it->data = malloc(len);
    if (!it->data)
        return -1;
    memcpy(it->data, data, len);
    g->nfeed++;
    return 0;
}

static ssize_t feed_find_cursor(const Group *g, uint16_t numb, uint16_t numr) {
    if (numr == 0) {
        for (size_t i = 0; i < g->nfeed; i++) {
            if (g->feed[i].numb == numb && g->feed[i].numr == 0)
                return (ssize_t)i;
        }
        return -1;
    }
    for (size_t i = 0; i < g->nfeed; i++) {
        if (g->feed[i].numb == numb && g->feed[i].numr == numr)
            return (ssize_t)i;
    }
    return -1;
}

static void notif23_for_items(Server *s, Group *g, size_t start, size_t end) {
    for (size_t i = start; i < end; i++) {
        const FeedItem *it = &g->feed[i];
        uint16_t author_post = (it->numr == 0) ? it->author : post_author(g, it->numb);
        if (author_post == 0)
            continue;
        User *u = user_find(s, author_post);
        if (u)
            notify_user(s, u, PAROLES_NOTIF_UDP_READ, g->id);
    }
}

static int handle_register(Server *s, int fd) {
    /* Les 4 premiers octets (CODEREQ + ZEROS) sont déjà lus dans serve_client. */
    uint8_t buf[PAROLES_NOM_LEN + PAROLES_CLE_LEN];
    if (net_read_all(fd, buf, sizeof(buf), READ_TIMEOUT_MS) != 0)
        return -1;
    struct sockaddr_in6 peer;
    socklen_t plen = sizeof(peer);
    if (getpeername(fd, (struct sockaddr *)&peer, &plen) != 0)
        return -1;

    User *u = user_add(s, (const char *)buf, buf + PAROLES_NOM_LEN, &peer);
    if (!u)
        return send_err(fd);

    uint8_t out[2 + 2 + 2 + PAROLES_CLE_LEN];
    wire_put16(out, PAROLES_RESP_REGISTER);
    wire_put16(out + 2, u->id);
    wire_put16(out + 4, u->udp_port);
    memset(out + 6, 0, PAROLES_CLE_LEN); /* clef serveur factice (étape 1) */
    return net_write_all(fd, out, sizeof(out));
}

static int handle_create(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    User *u = user_find(s, uid);
    if (!u)
        return send_err(fd);

    uint8_t lb[2];
    if (net_read_all(fd, lb, 2, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t len = wire_get16(lb);
    if (len == 0 || len > 2048)
        return send_err(fd);
    char *name = malloc(len);
    if (!name)
        return -1;
    if (net_read_all(fd, name, len, READ_TIMEOUT_MS) != 0) {
        free(name);
        return -1;
    }

    Group *g = group_add(s, uid, name, len);
    free(name);
    if (!g)
        return send_err(fd);

    uint8_t out[2 + 2 + 2 + 16];
    wire_put16(out, PAROLES_RESP_CREATE_GROUP);
    wire_put16(out + 2, g->id);
    wire_put16(out + 4, g->mcast_port);
    memcpy(out + 6, &g->mcast, 16);
    return net_write_all(fd, out, sizeof(out));
}

static int handle_invite(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    User *u = user_find(s, uid);
    if (!u)
        return send_err(fd);
    uint8_t rest[4];
    if (net_read_all(fd, rest, 4, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t gid = wire_get16(rest);
    uint16_t nb = wire_get16(rest + 2);
    Group *g = group_find(s, gid);
    if (!g || g->admin_id != uid)
        return send_err(fd);

    for (uint16_t i = 0; i < nb; i++) {
        uint8_t idz[4];
        if (net_read_all(fd, idz, 4, READ_TIMEOUT_MS) != 0)
            return -1;
        uint16_t invitee = wire_get16(idz);
        if (!user_find(s, invitee))
            return send_err(fd);
        if (is_member(g, invitee))
            continue;
        if (pending_has(s, invitee, gid))
            continue;
        if (pending_add(s, invitee, gid) != 0)
            return send_err(fd);
        User *inv = user_find(s, invitee);
        if (inv)
            notify_user(s, inv, PAROLES_NOTIF_UDP_INVITE, gid);
    }

    uint8_t ack[4];
    wire_put16(ack, PAROLES_ACK);
    wire_put16(ack + 2, 0);
    return net_write_all(fd, ack, sizeof(ack));
}

static int handle_list_invites(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    if (!user_find(s, uid))
        return send_err(fd);

    size_t cnt = 0;
    for (size_t i = 0; i < s->npending; i++) {
        if (s->pending[i].uid != uid)
            continue;
        Group *g = group_find(s, s->pending[i].gid);
        User *adm = g ? user_find(s, g->admin_id) : NULL;
        if (g && adm)
            cnt++;
    }

    size_t body = 4;
    for (size_t i = 0; i < s->npending; i++) {
        if (s->pending[i].uid != uid)
            continue;
        Group *g = group_find(s, s->pending[i].gid);
        User *adm = g ? user_find(s, g->admin_id) : NULL;
        if (!g || !adm)
            continue;
        size_t glen = strlen(g->gname);
        body += 2 + 2 + glen + PAROLES_NOM_LEN;
    }

    uint8_t *out = malloc(body);
    if (!out)
        return -1;
    wire_put16(out, PAROLES_RESP_LIST_INVITES);
    wire_put16(out + 2, (uint16_t)cnt);
    size_t off = 4;
    for (size_t i = 0; i < s->npending; i++) {
        if (s->pending[i].uid != uid)
            continue;
        Group *g = group_find(s, s->pending[i].gid);
        User *adm = g ? user_find(s, g->admin_id) : NULL;
        if (!g || !adm)
            continue;
        size_t glen = strlen(g->gname);
        wire_put16(out + off, g->id);
        off += 2;
        wire_put16(out + off, (uint16_t)glen);
        off += 2;
        memcpy(out + off, g->gname, glen);
        off += glen;
        wire_pad_nom(out + off, (const char *)adm->nom);
        off += PAROLES_NOM_LEN;
    }

    int r = net_write_all(fd, out, body);
    free(out);
    return r;
}

static int pack_joined(Server *s, Group *g, uint8_t **out, size_t *osz) {
    size_t nb = g->nmemb;
    size_t sz = 2 + 2 + 2 + 16 + 2 + nb * (4 + PAROLES_NOM_LEN);
    uint8_t *p = malloc(sz);
    if (!p)
        return -1;
    size_t o = 0;
    wire_put16(p + o, PAROLES_RESP_JOINED);
    o += 2;
    wire_put16(p + o, g->id);
    o += 2;
    wire_put16(p + o, g->mcast_port);
    o += 2;
    memcpy(p + o, &g->mcast, 16);
    o += 16;
    wire_put16(p + o, (uint16_t)nb);
    o += 2;
    /* admin en premier */
    uint16_t order[MAX_GROUP_MEMBERS];
    size_t no = 0;
    order[no++] = g->admin_id;
    for (size_t i = 0; i < g->nmemb; i++) {
        if (g->memb[i] == g->admin_id)
            continue;
        order[no++] = g->memb[i];
    }
    for (size_t i = 0; i < no; i++) {
        User *u = user_find(s, order[i]);
        if (!u) {
            free(p);
            return -1;
        }
        wire_put16(p + o, u->id);
        o += 2;
        wire_put16(p + o, 0);
        o += 2;
        wire_pad_nom(p + o, (const char *)u->nom);
        o += PAROLES_NOM_LEN;
    }
    *out = p;
    *osz = o;
    return 0;
}

static int handle_inv_answer(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    User *u = user_find(s, uid);
    if (!u)
        return send_err(fd);
    uint8_t tail[6];
    if (net_read_all(fd, tail, 6, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t gid = wire_get16(tail);
    uint16_t an = wire_get16(tail + 2);
    /* tail+4 zeros */

    Group *g = group_find(s, gid);

    if (an == PAROLES_INV_LEAVE) {
        if (!g)
            return send_err(fd);
        if (!is_member(g, uid))
            return send_err(fd);
        if (g->admin_id == uid) {
            notify_group_mcast(s, g, PAROLES_NOTIF_MCAST_CLOSE);
            group_destroy(s, g);
        } else {
            size_t w = 0;
            for (size_t i = 0; i < g->nmemb; i++) {
                if (g->memb[i] == uid)
                    continue;
                g->memb[w++] = g->memb[i];
            }
            g->nmemb = w;
            notify_group_mcast(s, g, PAROLES_NOTIF_MCAST_LEAVE);
        }
        uint8_t ack[4];
        wire_put16(ack, PAROLES_ACK);
        wire_put16(ack + 2, 0);
        return net_write_all(fd, ack, sizeof(ack));
    }

    if (an == PAROLES_INV_REFUSE) {
        if (!g || !pending_has(s, uid, gid))
            return send_err(fd);
        pending_remove_for(s, uid, gid);
        uint8_t ack[4];
        wire_put16(ack, PAROLES_ACK);
        wire_put16(ack + 2, 0);
        return net_write_all(fd, ack, sizeof(ack));
    }

    if (an == PAROLES_INV_ACCEPT) {
        if (!g)
            return send_err(fd);
        if (!pending_has(s, uid, gid))
            return send_err(fd);
        pending_remove_for(s, uid, gid);
        if (!is_member(g, uid)) {
            if (g->nmemb >= MAX_GROUP_MEMBERS)
                return send_err(fd);
            g->memb[g->nmemb++] = uid;
        }
        notify_group_mcast(s, g, PAROLES_NOTIF_MCAST_JOIN);
        uint8_t *resp = NULL;
        size_t rsz = 0;
        if (pack_joined(s, g, &resp, &rsz) != 0)
            return send_err(fd);
        int wr = net_write_all(fd, resp, rsz);
        free(resp);
        return wr;
    }

    return send_err(fd);
}

static int handle_list_members(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    if (!user_find(s, uid))
        return send_err(fd);
    uint8_t gz[4];
    if (net_read_all(fd, gz, 4, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t idg = wire_get16(gz);

    if (idg == 0) {
        size_t nb = 0;
        for (int i = 0; i < MAX_USERS; i++) {
            if (s->users[i].used)
                nb++;
        }
        size_t sz = 2 + 2 + 2 + 2 + nb * (4 + PAROLES_NOM_LEN);
        uint8_t *out = malloc(sz);
        if (!out)
            return -1;
        wire_put16(out, PAROLES_RESP_LIST_MEMBERS);
        wire_put16(out + 2, 0);
        wire_put16(out + 4, (uint16_t)nb);
        size_t o = 6;
        for (int i = 0; i < MAX_USERS; i++) {
            if (!s->users[i].used)
                continue;
            wire_put16(out + o, s->users[i].id);
            o += 2;
            wire_put16(out + o, 0);
            o += 2;
            wire_pad_nom(out + o, (const char *)s->users[i].nom);
            o += PAROLES_NOM_LEN;
        }
        int w = net_write_all(fd, out, o);
        free(out);
        return w;
    }

    Group *g = group_find(s, idg);
    if (!g || !is_member(g, uid))
        return send_err(fd);

    size_t nb = g->nmemb;
    size_t sz = 2 + 2 + 2 + 2 + nb * (4 + PAROLES_NOM_LEN);
    uint8_t *out = malloc(sz);
    if (!out)
        return -1;
    wire_put16(out, PAROLES_RESP_LIST_MEMBERS);
    wire_put16(out + 2, g->id);
    wire_put16(out + 4, (uint16_t)nb);
    size_t o = 6;
    uint16_t order[MAX_GROUP_MEMBERS];
    size_t no = 0;
    order[no++] = g->admin_id;
    for (size_t i = 0; i < g->nmemb; i++) {
        if (g->memb[i] == g->admin_id)
            continue;
        order[no++] = g->memb[i];
    }
    for (size_t i = 0; i < no; i++) {
        User *uu = user_find(s, order[i]);
        if (!uu)
            continue;
        wire_put16(out + o, uu->id);
        o += 2;
        wire_put16(out + o, 0);
        o += 2;
        wire_pad_nom(out + o, (const char *)uu->nom);
        o += PAROLES_NOM_LEN;
    }
    int w = net_write_all(fd, out, o);
    free(out);
    return w;
}

static int handle_post(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    User *u = user_find(s, uid);
    if (!u)
        return send_err(fd);
    uint8_t mid[6];
    if (net_read_all(fd, mid, 6, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t gid = wire_get16(mid);
    uint16_t l = wire_get16(mid + 4);
    if (l == 0 || l > MAX_POST_BODY)
        return send_err(fd);
    Group *g = group_find(s, gid);
    if (!g || !is_member(g, uid))
        return send_err(fd);
    uint8_t *data = malloc(l);
    if (!data)
        return -1;
    if (net_read_all(fd, data, l, READ_TIMEOUT_MS) != 0) {
        free(data);
        return -1;
    }
    uint16_t numb = (uint16_t)g->next_post++;
    if (feed_push(g, uid, numb, 0, data, l) != 0) {
        free(data);
        return send_err(fd);
    }
    free(data);
    notify_group_mcast(s, g, PAROLES_NOTIF_MCAST_NEW);

    uint8_t out[8];
    wire_put16(out, PAROLES_RESP_POST);
    wire_put16(out + 2, g->id);
    wire_put16(out + 4, numb);
    wire_put16(out + 6, 0);
    return net_write_all(fd, out, sizeof(out));
}

static int handle_reply(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    User *u = user_find(s, uid);
    if (!u)
        return send_err(fd);
    uint8_t mid[10];
    if (net_read_all(fd, mid, 10, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t gid = wire_get16(mid);
    uint16_t numb = wire_get16(mid + 4);
    uint16_t l = wire_get16(mid + 8);
    if (l == 0 || l > MAX_POST_BODY)
        return send_err(fd);
    Group *g = group_find(s, gid);
    if (!g || !is_member(g, uid))
        return send_err(fd);
    if (post_author(g, numb) == 0)
        return send_err(fd);
    uint8_t *data = malloc(l);
    if (!data)
        return -1;
    if (net_read_all(fd, data, l, READ_TIMEOUT_MS) != 0) {
        free(data);
        return -1;
    }
    uint16_t next_r = 0;
    for (size_t i = 0; i < g->nfeed; i++) {
        if (g->feed[i].numb == numb && g->feed[i].numr != 0) {
            if (g->feed[i].numr >= next_r)
                next_r = g->feed[i].numr;
        }
    }
    next_r++;
    if (feed_push(g, uid, numb, next_r, data, l) != 0) {
        free(data);
        return send_err(fd);
    }
    free(data);
    notify_group_mcast(s, g, PAROLES_NOTIF_MCAST_NEW);
    uint16_t pa = post_author(g, numb);
    User *author = user_find(s, pa);
    if (author)
        notify_user(s, author, PAROLES_NOTIF_UDP_READ, gid);

    uint8_t out[8];
    wire_put16(out, PAROLES_RESP_REPLY);
    wire_put16(out + 2, g->id);
    wire_put16(out + 4, numb);
    wire_put16(out + 6, next_r);
    return net_write_all(fd, out, sizeof(out));
}

static int handle_list_feed(Server *s, int fd, const uint8_t *hdr) {
    uint16_t uid = wire_get16(hdr + 2);
    if (!user_find(s, uid))
        return send_err(fd);
    uint8_t mid[8];
    if (net_read_all(fd, mid, 8, READ_TIMEOUT_MS) != 0)
        return -1;
    uint16_t gid = wire_get16(mid);
    uint16_t numb = wire_get16(mid + 4);
    uint16_t numr = wire_get16(mid + 6);
    Group *g = group_find(s, gid);
    if (!g || !is_member(g, uid))
        return send_err(fd);

    size_t start;
    if (numb == 0 && numr == 0) {
        /* Convention pratique : tout le fil depuis le début (hors sujet strict). */
        start = 0;
    } else {
        ssize_t cur = feed_find_cursor(g, numb, numr);
        if (cur < 0)
            return send_err(fd);
        start = (size_t)cur + 1;
    }
    size_t m = g->nfeed - start;
    size_t sz = 2 + 2 + 2 + 2;
    for (size_t i = start; i < g->nfeed; i++) {
        sz += 4 + 4 + 2 + g->feed[i].len;
    }
    uint8_t *out = malloc(sz);
    if (!out)
        return -1;
    size_t o = 0;
    wire_put16(out + o, PAROLES_RESP_LIST_FEED);
    o += 2;
    wire_put16(out + o, g->id);
    o += 2;
    wire_put16(out + o, (uint16_t)m);
    o += 2;
    for (size_t i = start; i < g->nfeed; i++) {
        const FeedItem *it = &g->feed[i];
        wire_put16(out + o, it->author);
        o += 2;
        wire_put16(out + o, 0);
        o += 2;
        wire_put16(out + o, it->numb);
        o += 2;
        wire_put16(out + o, it->numr);
        o += 2;
        wire_put16(out + o, (uint16_t)it->len);
        o += 2;
        memcpy(out + o, it->data, it->len);
        o += it->len;
    }
    notif23_for_items(s, g, start, g->nfeed);
    int w = net_write_all(fd, out, o);
    free(out);
    return w;
}

static int dispatch(Server *s, int fd, uint16_t code, const uint8_t *first4) {
    uint8_t hdr[4];
    memcpy(hdr, first4, 4);
    switch (code) {
    case PAROLES_REQ_REGISTER:
        return handle_register(s, fd);
    case PAROLES_REQ_CREATE_GROUP:
        return handle_create(s, fd, hdr);
    case PAROLES_REQ_INVITE:
        return handle_invite(s, fd, hdr);
    case PAROLES_REQ_LIST_INVITES:
        return handle_list_invites(s, fd, hdr);
    case PAROLES_REQ_INVITE_ANSWER:
        return handle_inv_answer(s, fd, hdr);
    case PAROLES_REQ_LIST_MEMBERS:
        return handle_list_members(s, fd, hdr);
    case PAROLES_REQ_POST:
        return handle_post(s, fd, hdr);
    case PAROLES_REQ_REPLY:
        return handle_reply(s, fd, hdr);
    case PAROLES_REQ_LIST_FEED:
        return handle_list_feed(s, fd, hdr);
    default:
        return send_err(fd);
    }
}

static int serve_client(Server *s, int fd) {
    struct timeval tv = {.tv_sec = READ_TIMEOUT_MS / 1000,
                         .tv_usec = (READ_TIMEOUT_MS % 1000) * 1000};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    uint8_t h[4];
    int rr = net_read_all(fd, h, 4, READ_TIMEOUT_MS);
    if (rr != 0)
        return -1;
    uint16_t code = wire_get16(h);
    return dispatch(s, fd, code, h);
}

static int server_init_udp(Server *s) {
    int u = socket(AF_INET6, SOCK_DGRAM, 0);
    if (u < 0)
        return -1;
    int z = 0;
    setsockopt(u, IPPROTO_IPV6, IPV6_V6ONLY, &z, sizeof(z));
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    a.sin6_addr = in6addr_any;
    a.sin6_port = htons(0);
    if (bind(u, (struct sockaddr *)&a, sizeof(a)) != 0) {
        close(u);
        return -1;
    }
    s->udp_sock = u;
    return 0;
}

int main(int argc, char **argv) {
    const char *bindaddr = "::";
    uint16_t port = 4242;
    if (argc >= 2)
        bindaddr = argv[1];
    if (argc >= 3)
        port = (uint16_t)atoi(argv[2]);

    Server s;
    memset(&s, 0, sizeof(s));
    s.next_user_id = 1;
    s.next_group_id = 0;
    if (server_init_udp(&s) != 0) {
        perror("udp");
        return 1;
    }

    int ls = socket(AF_INET6, SOCK_STREAM, 0);
    if (ls < 0) {
        perror("socket");
        return 1;
    }
    int on = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int z = 0;
    setsockopt(ls, IPPROTO_IPV6, IPV6_V6ONLY, &z, sizeof(z));

    struct sockaddr_in6 in;
    memset(&in, 0, sizeof(in));
    in.sin6_family = AF_INET6;
    in.sin6_port = htons(port);
    if (inet_pton(AF_INET6, bindaddr, &in.sin6_addr) != 1) {
        fprintf(stderr, "Adresse IPv6 invalide: %s\n", bindaddr);
        return 1;
    }
    if (bind(ls, (struct sockaddr *)&in, sizeof(in)) != 0) {
        perror("bind");
        return 1;
    }
    if (listen(ls, 16) != 0) {
        perror("listen");
        return 1;
    }

    fprintf(stderr, "paroles_server écoute [%s]:%u\n", bindaddr, (unsigned)port);

    for (;;) {
        int fd = accept(ls, NULL, NULL);
        if (fd < 0) {
            if (errno == EINTR)
                continue;
            perror("accept");
            break;
        }
        (void)serve_client(&s, fd);
        close(fd);
    }

    close(s.udp_sock);
    close(ls);
    return 0;
}
