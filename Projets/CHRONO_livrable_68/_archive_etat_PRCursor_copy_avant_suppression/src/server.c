#include "../include/paroles_proto.h"
#include "../include/auth_ed25519.h"
#include "../include/tls_io.h"
#include "net.h"
#include "wire.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <string.h>
#include <unistd.h>

static SSL *g_io_ssl;
static EVP_PKEY *g_srv_sign_key;

#define MAX_USERS 1024
#define MAX_GROUPS 512
#define MAX_FEED 4096
#define MAX_BODY PAROLES_MAX_BODY

typedef struct {
    int used;
    uint32_t id;
    char nom[PAROLES_NOM_LEN];
    uint8_t cle[PAROLES_CLE_LEN];
    /* CHRONO N°61 : auth_nonce retiré — extrait_server_auth_nonce.c (liaison avec do_client_auth N°60) */
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

/* CHRONO 11 : cle_is_zero retiré — voir extrait_server_reg_cle_zero.c */

static void send_err(int fd) {
    (void)fd;
    /* CHRONO 35 : send_err retiré (CODEREQ 31 + ZEROS) — livrable 35 */
}

static void send_err_msg(int fd, const char *msg) {
    if (!msg || !*msg) {
        send_err(fd);
        return;
    }
    (void)fd;
    /* CHRONO 36 : send_err_msg retiré (ERR + u16 len + message) — livrable 36 */
}

static void send_ack(int fd) {
    unsigned char b[1 + PAROLES_ERR_TAIL];
    memset(b, 0, sizeof b);
    b[0] = PAROLES_CODEREQ_ACK;
    conn_writen(g_io_ssl, fd, b, sizeof b);
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

/* CHRONO 37 : notif_mcast retiré — livrable 37 (codes 18–21 dans extrait_paroles_proto_notif_mcast.h) */
static void notif_mcast(Group *g, uint16_t code) {
    (void)g;
    (void)code;
}

/* CHRONO 38 : notif_udp_user retiré — livrable 38 (NOTIF_INV_UDP 22, NOTIF_FETCH 23) */
static void notif_udp_user(User *u, uint16_t code, uint32_t idg) {
    (void)u;
    (void)code;
    (void)idg;
}

static Post *find_post(Group *g, uint32_t numb) {
    for (size_t i = 0; i < g->nposts; i++)
        if (g->posts[i].numb == numb) return &g->posts[i];
    return NULL;
}

static void close_group(Group *g) {
    if (g->closed) return;
    g->closed = 1;
    notif_mcast(g, 21); /* CLOSE = extrait 37 */
}

static int handle_reg(int fd, struct sockaddr_in6 *peer, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)peer;
    (void)body;
    (void)blen;
    /* CHRONO 11–15, 13–14 : handle_reg retiré — livrable 11–15 ; CLE 113 réelle = livrable 58/extrait_server_handle_reg_cle113.c */
    return -1;
}

static int handle_new_group(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 16–18 : handle_new_group retiré (NEW_GROUP_OK, idg/nom, mcast/admin) — livrable 16–18 */
    return -1;
}

static int handle_invite(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 19 : handle_invite retiré (send_ack utilisé ailleurs, conservé) — livrable 19 */
    return -1;
}

static int handle_list_inv(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 20 : handle_list_inv retiré — livrable 20 */
    return -1;
}

static int handle_inv_ans(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 21–22 : handle_inv_ans retiré (INV_ANS / admin quitte close_group) — livrable 21–22 */
    return -1;
}

static int handle_list_mem(int fd, uint32_t uid, uint32_t idg) {
    (void)fd;
    (void)uid;
    (void)idg;
    /* CHRONO 23 : handle_list_mem retiré — livrable 23 */
    return -1;
}

static int handle_post(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 24 : handle_post retiré — livrable 24 */
    return -1;
}

static int handle_reply(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 25 : handle_reply retiré — livrable 25 */
    return -1;
}

static ssize_t feed_index_after(Group *g, uint32_t numb, uint32_t numr) {
    (void)g;
    (void)numb;
    (void)numr;
    /* CHRONO 26 : feed_index_after retiré — livrable 26 */
    return -1;
}

static int handle_feed(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)fd;
    (void)uid;
    (void)body;
    (void)blen;
    /* CHRONO 26 : handle_feed retiré — livrable 26 */
    return -1;
}

/* CHRONO N°60 : do_client_auth + send_auth_ok retirés — extraits server_do_client_auth.c */

static int dispatch(int fd, struct sockaddr_in6 *peer, uint8_t code, const unsigned char *body,
                    size_t blen, uint32_t sess_uid) {
    (void)sess_uid;
    /* CHRONO 34 : garde wire_uid == sess_uid (auth) retirée — extrait_server_garde_rejets.c */
    switch (code) {
    case PAROLES_CODEREQ_REG:
        return handle_reg(fd, peer, body, blen);
    case PAROLES_CODEREQ_NEW_GROUP:
        if (blen < 4) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_new_group(fd, uid, q, left);
        }
    case PAROLES_CODEREQ_INVITE:
        if (blen < 12) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_invite(fd, uid, q, left);
        }
    case PAROLES_CODEREQ_LIST_INV:
        if (blen != 4) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_list_inv(fd, uid, q, left);
        }
    case PAROLES_CODEREQ_INV_ANS:
        if (blen != 4 + 4 + 1) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_inv_ans(fd, uid, q, left);
        }
    case PAROLES_CODEREQ_LIST_MEM:
        if (blen < 8) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid, idg;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
            /* CHRONO 34 : if (left) return -1; LIST_MEM corps 8 octets — retiré */
            return handle_list_mem(fd, uid, idg);
        }
    case PAROLES_CODEREQ_POST:
        if (blen < 4) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_post(fd, uid, q, left);
        }
    case PAROLES_CODEREQ_REPLY:
        if (blen < 4) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_reply(fd, uid, q, left);
        }
    case PAROLES_CODEREQ_FEED:
        if (blen < 4) return -1;
        {
            const unsigned char *q = body;
            size_t left = blen;
            uint32_t uid;
            if (wire_get_u32_be(&q, &left, &uid) < 0) return -1;
            return handle_feed(fd, uid, q, left);
        }
    default:
        return -1;
    }
}

static int serve_business_switch(int cfd, struct sockaddr_in6 *peer, uint8_t code, uint32_t sess_uid) {
    unsigned char buf[256 * 1024];
    switch (code) {
    case PAROLES_CODEREQ_REG:
        return -1;
    case PAROLES_CODEREQ_NEW_GROUP:
        if (conn_readn(g_io_ssl, cfd, buf, 6, 30000) < 0) return -1;
        {
            uint16_t ln = (uint16_t)((buf[4] << 8) | buf[5]);
            if (ln > MAX_BODY) return -1;
            if (conn_readn(g_io_ssl, cfd, buf + 6, ln, 30000) < 0) return -1;
            return dispatch(cfd, peer, code, buf, 6 + ln, sess_uid);
        }
    case PAROLES_CODEREQ_INVITE:
        if (conn_readn(g_io_ssl, cfd, buf, 12, 30000) < 0) return -1;
        {
            uint32_t nb = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[9] << 16) |
                          ((uint32_t)buf[10] << 8) | (uint32_t)buf[11];
            if (nb > 8192) return -1;
            if (nb > 0 && conn_readn(g_io_ssl, cfd, buf + 12, nb * 8u, 30000) < 0)
                return -1;
            return dispatch(cfd, peer, code, buf, 12 + nb * 8u, sess_uid);
        }
    case PAROLES_CODEREQ_LIST_INV:
        if (conn_readn(g_io_ssl, cfd, buf, 4, 30000) < 0) return -1;
        return dispatch(cfd, peer, code, buf, 4, sess_uid);
    case PAROLES_CODEREQ_INV_ANS:
        if (conn_readn(g_io_ssl, cfd, buf, 9, 30000) < 0) return -1;
        return dispatch(cfd, peer, code, buf, 9, sess_uid);
    case PAROLES_CODEREQ_LIST_MEM:
        if (conn_readn(g_io_ssl, cfd, buf, 8, 30000) < 0) return -1;
        return dispatch(cfd, peer, code, buf, 8, sess_uid);
    case PAROLES_CODEREQ_POST:
        if (conn_readn(g_io_ssl, cfd, buf, 10, 30000) < 0) return -1;
        {
            uint16_t ln = (uint16_t)((buf[8] << 8) | buf[9]);
            if (ln > MAX_BODY) return -1;
            if (conn_readn(g_io_ssl, cfd, buf + 10, ln, 30000) < 0) return -1;
            return dispatch(cfd, peer, code, buf, 10 + ln, sess_uid);
        }
    case PAROLES_CODEREQ_REPLY:
        if (conn_readn(g_io_ssl, cfd, buf, 14, 30000) < 0) return -1;
        {
            uint16_t ln = (uint16_t)((buf[12] << 8) | buf[13]);
            if (ln > MAX_BODY) return -1;
            if (conn_readn(g_io_ssl, cfd, buf + 14, ln, 30000) < 0) return -1;
            return dispatch(cfd, peer, code, buf, 14 + ln, sess_uid);
        }
    case PAROLES_CODEREQ_FEED:
        if (conn_readn(g_io_ssl, cfd, buf, 16, 30000) < 0) return -1;
        return dispatch(cfd, peer, code, buf, 16, sess_uid);
    default:
        return -1;
    }
}

static void serve_client(int cfd, struct sockaddr_in6 *peer, SSL_CTX *tls_ctx) {
    (void)tls_ctx;
    /* CHRONO N°54 : SSL_accept / tls_ctx retirés — extrait_server_tls_handshake.c */
    SSL *ssl = NULL;
    g_io_ssl = NULL;

    unsigned char hdr[1];
    unsigned char buf[256 * 1024];
    /* CHRONO N°41 : conn_readn timeout retiré ; CHRONO N°54 : plus de TLS → lecture TCP brute */
    if (read(cfd, hdr, 1) <= 0) goto end;
    uint8_t code = hdr[0];
    uint32_t sess_uid = 0;

    /* CHRONO 14 : branche REG (lecture + dispatch) retirée — extrait_server_serve_client_reg_fin.c */
    /* CHRONO N°59 : flux AUTH + sess_uid (hors REG) retiré — extrait_server_session_auth.c (garde dispatch = N°34) */

    if (serve_business_switch(cfd, peer, code, sess_uid) < 0) goto err;
    goto end;
bad:
    goto end;
err:
    send_err(cfd);
    goto end;
end:
    g_io_ssl = NULL;
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(cfd);
}

int main(int argc, char **argv) {
    /* CHRONO N°42 : mode -v / vlog retiré — livrable 42 */
    /* CHRONO N°54 : --tls / paroles_tls_server_ctx retirés — extrait_server_tls_handshake.c */
    int pi = 1;
    SSL_CTX *tls_ctx = NULL;
    if (argc - pi >= 2 && strcmp(argv[pi], "--signing-key") == 0) {
        g_srv_sign_key = paroles_load_ed25519_private_pem(argv[pi + 1]);
        if (!g_srv_sign_key) {
            fprintf(stderr, "paroles_server: clé ED25519 serveur (--signing-key) invalide\n");
            paroles_tls_ctx_free(tls_ctx);
            return 1;
        }
        pi += 2;
    }
    if (argc - pi < 2) {
        fprintf(stderr,
                "usage: paroles_server [--signing-key priv.pem] "
                "bind_ipv6 port\n");
        EVP_PKEY_free(g_srv_sign_key);
        paroles_tls_ctx_free(tls_ctx);
        return 1;
    }
    const char *host = argv[pi];
    uint16_t port = (uint16_t)atoi(argv[pi + 1]);
    if (strcmp(host, "-v") == 0) {
        fprintf(stderr, "usage error\n");
        EVP_PKEY_free(g_srv_sign_key);
        paroles_tls_ctx_free(tls_ctx);
        return 1;
    }
    int srv = tcp6_listen(host, port);
    if (srv < 0) {
        perror("listen");
        EVP_PKEY_free(g_srv_sign_key);
        paroles_tls_ctx_free(tls_ctx);
        return 1;
    }
    fprintf(stderr, "paroles_server ecoute [%s]:%u%s\n", host, (unsigned)port, g_srv_sign_key ? " +auth" : "");
    for (;;) {
        struct sockaddr_in6 peer;
        socklen_t pl = sizeof peer;
        int c = tcp6_accept(srv, &peer, &pl);
        if (c < 0) continue;
        serve_client(c, &peer, tls_ctx);
    }
}
