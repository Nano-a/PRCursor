#include "../include/paroles_proto.h"
#include "../include/auth_ed25519.h"
#include "../include/tls_io.h"
#include "net.h"
#include "wire.h"
#include <arpa/inet.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* CHRONO N°42 : mode verbeux (-v) retiré — livrable 42
 * CHRONO N°54 : client TLS (--tls, SSL_connect, SNI) retiré — extrait_client_tls_handshake.c */
static SSL *g_io_ssl;
static EVP_PKEY *g_auth_priv;
static EVP_PKEY *g_srv_pub_verify;

static void pad_nom(unsigned char *dst, const char *src) {
    memset(dst, 0, PAROLES_NOM_LEN);
    strncpy((char *)dst, src, PAROLES_NOM_LEN);
}

/* CHRONO N°61 : nonce_path / read_nonce / write_nonce retirés — extrait_client_nonce_persist.c */

static int one_cmd_plain(const char *host, uint16_t port, const unsigned char *msg, size_t len,
                         unsigned char *resp, size_t rmax) {
    struct sockaddr_in6 lh;
    memset(&lh, 0, sizeof lh);
    int fd = tcp6_connect(host, port, &lh);
    if (fd < 0) {
        perror("connect");
        return -1;
    }
    g_io_ssl = NULL;
    if (conn_writen(g_io_ssl, fd, msg, len) < 0) {
        close(fd);
        return -1;
    }
    int n = conn_read_upto(g_io_ssl, fd, resp, rmax, 30000);
    close(fd);
    if (n <= 0) return -1;
    return n;
}

static int one_cmd_authed(const char *host, uint16_t port, uint32_t uid, const unsigned char *msg,
                          size_t len, unsigned char *resp, size_t rmax) {
    struct sockaddr_in6 lh;
    memset(&lh, 0, sizeof lh);
    int fd = tcp6_connect(host, port, &lh);
    if (fd < 0) {
        perror("connect");
        return -1;
    }
    g_io_ssl = NULL;
    /* CHRONO N°60 : CODEREQ 0 + AUTH_OK + vérif serveur retirés — extrait_client_codereq0_auth_pkt.c
     * CHRONO N°63 : suite one_cmd_authed (métier sur même TCP après auth) retirée — extrait_client_one_cmd_authed_tail.c */
    (void)uid;
    (void)msg;
    (void)len;
    (void)resp;
    (void)rmax;
    goto fail;
fail:
    g_io_ssl = NULL;
    close(fd);
    return -1;
}

static int one_cmd(const char *host, uint16_t port, uint32_t auth_uid, const unsigned char *msg, size_t len,
                   unsigned char *resp, size_t rmax) {
    if (!g_auth_priv) return one_cmd_plain(host, port, msg, len, resp, rmax);
    return one_cmd_authed(host, port, auth_uid, msg, len, resp, rmax);
}

/* CHRONO N°58 : CLE 113 depuis PEM retirée — extrait_client_reg_cle113.c */
static int fill_ed25519_cle_from_pem(const char *path, unsigned char *cle) {
    (void)path;
    (void)cle;
    return -1;
}

static int cmd_reg(const char *host, uint16_t port, const char *name, const char *pub_pem) {
    (void)host;
    (void)port;
    (void)name;
    (void)pub_pem;
    /* CHRONO 11–13 : cmd_reg retiré — livrable 11 (CLE nuls) + 13 (CODEREQ 1–2) */
    return -1;
}

static int cmd_newgroup(const char *host, uint16_t port, uint32_t uid, const char *gname) {
    (void)host;
    (void)port;
    (void)uid;
    (void)gname;
    /* CHRONO 16 : cmd_newgroup retiré — livrable 16 */
    return -1;
}

static int cmd_invite(const char *host, uint16_t port, uint32_t admin, uint32_t idg, int ninv,
                      uint32_t *ids) {
    (void)host;
    (void)port;
    (void)admin;
    (void)idg;
    (void)ninv;
    (void)ids;
    /* CHRONO 19 : cmd_invite retiré — livrable 19 */
    return -1;
}

static int cmd_listinv(const char *host, uint16_t port, uint32_t uid) {
    (void)host;
    (void)port;
    (void)uid;
    /* CHRONO 20 : cmd_listinv retiré — livrable 20 */
    return -1;
}

static int cmd_ans(const char *host, uint16_t port, uint32_t uid, uint32_t idg, int an) {
    (void)host;
    (void)port;
    (void)uid;
    (void)idg;
    (void)an;
    /* CHRONO 21 : cmd_ans retiré — livrable 21 */
    return -1;
}

static int cmd_listmem(const char *host, uint16_t port, uint32_t uid, uint32_t idg) {
    (void)host;
    (void)port;
    (void)uid;
    (void)idg;
    /* CHRONO 23 : cmd_listmem retiré — livrable 23 */
    return -1;
}

static int cmd_post(const char *host, uint16_t port, uint32_t uid, uint32_t idg, const char *txt) {
    (void)host;
    (void)port;
    (void)uid;
    (void)idg;
    (void)txt;
    /* CHRONO 24 : cmd_post retiré — livrable 24 */
    return -1;
}

static int cmd_reply(const char *host, uint16_t port, uint32_t uid, uint32_t idg, uint32_t numb,
                     const char *txt) {
    (void)host;
    (void)port;
    (void)uid;
    (void)idg;
    (void)numb;
    (void)txt;
    /* CHRONO 25 : cmd_reply retiré — livrable 25 */
    return -1;
}

static int cmd_feed(const char *host, uint16_t port, uint32_t uid, uint32_t idg, uint32_t numb,
                    uint32_t numr) {
    (void)host;
    (void)port;
    (void)uid;
    (void)idg;
    (void)numb;
    (void)numr;
    /* CHRONO 26 : cmd_feed retiré — livrable 26 */
    return -1;
}

static void usage(void) {
    fprintf(stderr,
            "usage: paroles_client [--key priv_ed25519.pem] [--server-pub pub_ed25519.pem] "
            "host port cmd [args]\n"
            "  reg <nom> [pub_ed25519.pem]\n"
            "  newgroup <uid> <nom>\n"
            "  invite <admin> <idg> <uid> ...\n"
            "  listinv <uid>\n"
            "  ans <uid> <idg> <0|1|2>\n"
            "  listmem <uid> <idg>\n"
            "  post <uid> <idg> <texte>\n"
            "  reply <uid> <idg> <numb> <texte>\n"
            "  feed <uid> <idg> <numb> <numr>\n");
}

static void client_tls_atexit(void) {
    EVP_PKEY_free(g_auth_priv);
    EVP_PKEY_free(g_srv_pub_verify);
    g_auth_priv = NULL;
    g_srv_pub_verify = NULL;
}

int main(int argc, char **argv) {
    int i = 1;
    if (argc - i >= 2 && strcmp(argv[i], "--key") == 0) {
        g_auth_priv = paroles_load_ed25519_private_pem(argv[i + 1]);
        if (!g_auth_priv) {
            fprintf(stderr, "paroles_client: --key ED25519 invalide\n");
            return 1;
        }
        i += 2;
    }
    if (argc - i >= 2 && strcmp(argv[i], "--server-pub") == 0) {
        g_srv_pub_verify = paroles_load_ed25519_public_pem(argv[i + 1]);
        if (!g_srv_pub_verify) {
            fprintf(stderr, "paroles_client: --server-pub ED25519 invalide\n");
            EVP_PKEY_free(g_auth_priv);
            return 1;
        }
        i += 2;
    }
    if (argc - i < 3) {
        usage();
        EVP_PKEY_free(g_auth_priv);
        EVP_PKEY_free(g_srv_pub_verify);
        return 1;
    }
    const char *host = argv[i++];
    uint16_t port = (uint16_t)atoi(argv[i++]);
    const char *cmd = argv[i++];
    if (g_auth_priv || g_srv_pub_verify) {
        static int tls_atexit_done;
        if (!tls_atexit_done) {
            atexit(client_tls_atexit);
            tls_atexit_done = 1;
        }
    }
    if (!strcmp(cmd, "reg")) {
        if (argc - i < 1) {
            usage();
            return 1;
        }
        const char *ppem = (argc - i >= 2) ? argv[i + 1] : NULL;
        return cmd_reg(host, port, argv[i], ppem) < 0 ? 1 : 0;
    }
    if (!strcmp(cmd, "newgroup"))
        return cmd_newgroup(host, port, (uint32_t)atoi(argv[i]), argv[i + 1]) < 0 ? 1 : 0;
    if (!strcmp(cmd, "invite")) {
        uint32_t ad = (uint32_t)atoi(argv[i++]);
        uint32_t idg = (uint32_t)atoi(argv[i++]);
        int n = argc - i;
        uint32_t *ids = malloc(sizeof(uint32_t) * (size_t)n);
        for (int k = 0; k < n; k++) ids[k] = (uint32_t)atoi(argv[i + k]);
        int r = cmd_invite(host, port, ad, idg, n, ids);
        free(ids);
        return r < 0 ? 1 : 0;
    }
    if (!strcmp(cmd, "listinv"))
        return cmd_listinv(host, port, (uint32_t)atoi(argv[i])) < 0 ? 1 : 0;
    if (!strcmp(cmd, "ans"))
        return cmd_ans(host, port, (uint32_t)atoi(argv[i]), (uint32_t)atoi(argv[i + 1]),
                        atoi(argv[i + 2])) < 0
                   ? 1
                   : 0;
    if (!strcmp(cmd, "listmem"))
        return cmd_listmem(host, port, (uint32_t)atoi(argv[i]), (uint32_t)atoi(argv[i + 1])) < 0
                   ? 1
                   : 0;
    if (!strcmp(cmd, "post"))
        return cmd_post(host, port, (uint32_t)atoi(argv[i]), (uint32_t)atoi(argv[i + 1]),
                        argv[i + 2]) < 0
                   ? 1
                   : 0;
    if (!strcmp(cmd, "reply"))
        return cmd_reply(host, port, (uint32_t)atoi(argv[i]), (uint32_t)atoi(argv[i + 1]),
                         (uint32_t)atoi(argv[i + 2]), argv[i + 3]) < 0
                   ? 1
                   : 0;
    if (!strcmp(cmd, "feed"))
        return cmd_feed(host, port, (uint32_t)atoi(argv[i]), (uint32_t)atoi(argv[i + 1]),
                        (uint32_t)atoi(argv[i + 2]), (uint32_t)atoi(argv[i + 3])) < 0
                   ? 1
                   : 0;
    usage();
    return 1;
}
