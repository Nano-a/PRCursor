/*
 * Client Paroles interactif — étape 1 (TCP clair, IPv6).
 */

#include "../include/paroles_proto.h"
#include "net.h"
#include "wire.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static char g_host[INET6_ADDRSTRLEN] = "::1";
static uint16_t g_port = 4242;
static char g_ifname[IF_NAMESIZE] = "lo";
static uint16_t g_uid;
static uint16_t g_my_udp_port;
static int g_reg;
static int g_verbose;
static int g_uc_sock = -1;
static int g_mc_socks[16];
static uint16_t g_mc_gids[16];
static int g_n_mc;

static int tcp_connect(void) {
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;
    int z = 0;
    setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &z, sizeof(z));
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, g_host, &a.sin6_addr) != 1) {
        close(fd);
        return -1;
    }
    a.sin6_port = htons(g_port);
    if (connect(fd, (struct sockaddr *)&a, sizeof(a)) != 0) {
        perror("connect");
        close(fd);
        return -1;
    }
    return fd;
}

static int notif_unicast_open(void) {
    if (g_uc_sock >= 0)
        return 0;
    if (g_my_udp_port == 0)
        return -1;
    g_uc_sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (g_uc_sock < 0)
        return -1;
    int z = 0;
    setsockopt(g_uc_sock, IPPROTO_IPV6, IPV6_V6ONLY, &z, sizeof(z));
    int yes = 1;
    setsockopt(g_uc_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    a.sin6_addr = in6addr_any;
    a.sin6_port = htons(g_my_udp_port);
    if (bind(g_uc_sock, (struct sockaddr *)&a, sizeof(a)) != 0) {
        perror("bind UDP notif");
        close(g_uc_sock);
        g_uc_sock = -1;
        return -1;
    }
    if (g_verbose)
        fprintf(stderr, "[notif] ecoute UDP port %u\n", g_my_udp_port);
    return 0;
}

static int mcast_subscribe(uint16_t gid, const struct in6_addr *addr, uint16_t port) {
    for (int i = 0; i < g_n_mc; i++) {
        if (g_mc_gids[i] == gid)
            return 0;
    }
    if (g_n_mc >= 16)
        return -1;
    int s = socket(AF_INET6, SOCK_DGRAM, 0);
    if (s < 0)
        return -1;
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in6 binda;
    memset(&binda, 0, sizeof(binda));
    binda.sin6_family = AF_INET6;
    binda.sin6_addr = in6addr_any;
    binda.sin6_port = htons(port);
    if (bind(s, (struct sockaddr *)&binda, sizeof(binda)) != 0) {
        perror("bind mcast");
        close(s);
        return -1;
    }
    struct ipv6_mreq mr;
    memset(&mr, 0, sizeof(mr));
    mr.ipv6mr_multiaddr = *addr;
    mr.ipv6mr_interface = if_nametoindex(g_ifname);
    if (mr.ipv6mr_interface == 0)
        mr.ipv6mr_interface = 1;
    if (setsockopt(s, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mr, sizeof(mr)) != 0) {
        perror("join mcast");
        close(s);
        return -1;
    }
    g_mc_gids[g_n_mc] = gid;
    g_mc_socks[g_n_mc++] = s;
    if (g_verbose) {
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, addr, ip, sizeof(ip));
        fprintf(stderr, "[notif] multicast groupe %u %s:%u (if %s)\n", gid, ip, port,
                g_ifname);
    }
    return 0;
}

static void handle_notif_datagram(int fd, const char *tag) {
    uint8_t b[256];
    ssize_t n = recv(fd, b, sizeof(b), 0);
    if (n < 4)
        return;
    uint16_t code = wire_get16(b);
    uint16_t idg = wire_get16(b + 2);
    printf("\n[%s] notification code=%u idg=%u\n> ", tag, code, idg);
    fflush(stdout);
}

static void hexdump(const char *label, const uint8_t *p, size_t n) {
    if (!g_verbose)
        return;
    fprintf(stderr, "%s (%zu): ", label, n);
    for (size_t i = 0; i < n && i < 64; i++)
        fprintf(stderr, "%02x ", p[i]);
    fprintf(stderr, "%s\n", n > 64 ? "..." : "");
}

static int cmd_register(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: register <pseudo>\n");
        return 1;
    }
    int fd = tcp_connect();
    if (fd < 0)
        return 1;
    uint8_t msg[4 + PAROLES_NOM_LEN + PAROLES_CLE_LEN];
    wire_put16(msg, PAROLES_REQ_REGISTER);
    wire_put16(msg + 2, 0);
    wire_pad_nom(msg + 4, argv[1]);
    memset(msg + 4 + PAROLES_NOM_LEN, 0, PAROLES_CLE_LEN);
    if (net_write_all(fd, msg, sizeof(msg)) != 0) {
        close(fd);
        return 1;
    }
    uint8_t r[2 + 2 + 2 + PAROLES_CLE_LEN];
    if (net_read_all(fd, r, sizeof(r), -1) != 0) {
        close(fd);
        return 1;
    }
    close(fd);
    hexdump("reg", r, sizeof(r));
    uint16_t c = wire_get16(r);
    if (c == PAROLES_ERR) {
        printf("erreur serveur (31)\n");
        return 1;
    }
    if (c != PAROLES_RESP_REGISTER) {
        printf("réponse inattendue: %u\n", c);
        return 1;
    }
    g_uid = wire_get16(r + 2);
    g_my_udp_port = wire_get16(r + 4);
    g_reg = 1;
    (void)notif_unicast_open();
    printf("inscrit: id=%u udp_port=%u\n", g_uid, g_my_udp_port);
    return 0;
}

static int cmd_create(int argc, char **argv) {
    if (!g_reg || argc < 2) {
        fprintf(stderr, "usage: create <nom_groupe>\n");
        return 1;
    }
    const char *name = argv[1];
    size_t nl = strlen(name);
    if (nl == 0 || nl > 2000)
        return 1;
    int fd = tcp_connect();
    if (fd < 0)
        return 1;
    size_t tot = 4 + 2 + nl;
    uint8_t *msg = malloc(tot);
    if (!msg) {
        close(fd);
        return 1;
    }
    wire_put16(msg, PAROLES_REQ_CREATE_GROUP);
    wire_put16(msg + 2, g_uid);
    wire_put16(msg + 4, (uint16_t)nl);
    memcpy(msg + 6, name, nl);
    net_write_all(fd, msg, tot);
    free(msg);
    uint8_t head[4];
    if (net_read_all(fd, head, 4, -1) != 0) {
        close(fd);
        return 1;
    }
    uint16_t c = wire_get16(head);
    if (c == PAROLES_ERR) {
        net_read_all(fd, head + 2, 2, -1);
        printf("erreur serveur\n");
        close(fd);
        return 1;
    }
    if (c != PAROLES_RESP_CREATE_GROUP) {
        close(fd);
        return 1;
    }
    uint8_t rest[2 + 16];
    net_read_all(fd, rest, sizeof(rest), -1);
    uint16_t gid = wire_get16(head + 2);
    uint16_t mport = wire_get16(rest);
    struct in6_addr mcaddr;
    memcpy(&mcaddr, rest + 2, 16);
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &mcaddr, ip, sizeof(ip));
    (void)mcast_subscribe(gid, &mcaddr, mport);
    printf("groupe id=%u mcast=%s port=%u\n", gid, ip, mport);
    close(fd);
    return 0;
}

static int cmd_invite(int argc, char **argv) {
    if (!g_reg || argc < 3) {
        fprintf(stderr, "usage: invite <idg> <id_invité> [id_invité ...]\n");
        return 1;
    }
    uint16_t gid = (uint16_t)atoi(argv[1]);
    int nb = argc - 2;
    size_t tot = 4 + 4 + (size_t)nb * 4;
    uint8_t *msg = malloc(tot);
    if (!msg)
        return 1;
    wire_put16(msg, PAROLES_REQ_INVITE);
    wire_put16(msg + 2, g_uid);
    wire_put16(msg + 4, gid);
    wire_put16(msg + 6, (uint16_t)nb);
    for (int i = 0; i < nb; i++) {
        wire_put16(msg + 8 + (size_t)i * 4, (uint16_t)atoi(argv[2 + i]));
        wire_put16(msg + 8 + (size_t)i * 4 + 2, 0);
    }
    int fd = tcp_connect();
    net_write_all(fd, msg, tot);
    free(msg);
    uint8_t a[4];
    net_read_all(fd, a, 4, -1);
    close(fd);
    if (wire_get16(a) == PAROLES_ACK)
        printf("invitations envoyées\n");
    else
        printf("échec\n");
    return wire_get16(a) == PAROLES_ACK ? 0 : 1;
}

static int cmd_pending(void) {
    if (!g_reg)
        return 1;
    int fd = tcp_connect();
    uint8_t h[4];
    wire_put16(h, PAROLES_REQ_LIST_INVITES);
    wire_put16(h + 2, g_uid);
    net_write_all(fd, h, 4);
    uint8_t rh[4];
    net_read_all(fd, rh, 4, -1);
    uint16_t c = wire_get16(rh);
    uint16_t nb = wire_get16(rh + 2);
    if (c == PAROLES_ERR) {
        close(fd);
        return 1;
    }
    printf("%u invitation(s)\n", nb);
    for (uint16_t i = 0; i < nb; i++) {
        uint8_t ghead[4];
        net_read_all(fd, ghead, 4, -1);
        uint16_t idg = wire_get16(ghead);
        uint16_t len = wire_get16(ghead + 2);
        char *gn = malloc(len + 1);
        net_read_all(fd, (uint8_t *)gn, len, -1);
        gn[len] = '\0';
        uint8_t adm[PAROLES_NOM_LEN];
        net_read_all(fd, adm, sizeof(adm), -1);
        char admn[PAROLES_NOM_LEN + 1];
        memcpy(admn, adm, PAROLES_NOM_LEN);
        admn[PAROLES_NOM_LEN] = '\0';
        printf("  groupe %u \"%s\" admin %.10s\n", idg, gn, admn);
        free(gn);
    }
    close(fd);
    return 0;
}

static int cmd_answer(int argc, char **argv) {
    if (!g_reg || argc < 3) {
        fprintf(stderr, "usage: accept|refuse|leave <idg>\n");
        return 1;
    }
    uint16_t an = PAROLES_INV_REFUSE;
    if (strcmp(argv[1], "accept") == 0)
        an = PAROLES_INV_ACCEPT;
    else if (strcmp(argv[1], "refuse") == 0)
        an = PAROLES_INV_REFUSE;
    else if (strcmp(argv[1], "leave") == 0)
        an = PAROLES_INV_LEAVE;
    else
        return 1;
    uint16_t gid = (uint16_t)atoi(argv[2]);
    int fd = tcp_connect();
    uint8_t m[10];
    wire_put16(m, PAROLES_REQ_INVITE_ANSWER);
    wire_put16(m + 2, g_uid);
    wire_put16(m + 4, gid);
    wire_put16(m + 6, an);
    wire_put16(m + 8, 0);
    net_write_all(fd, m, sizeof(m));
    uint8_t rh[4];
    net_read_all(fd, rh, 4, -1);
    uint16_t c = wire_get16(rh);
    if (c == PAROLES_ACK) {
        net_read_all(fd, rh + 2, 2, -1);
        printf("ok (acquittement)\n");
        close(fd);
        return 0;
    }
    if (c == PAROLES_RESP_JOINED) {
        uint16_t gid_join = wire_get16(rh + 2);
        uint8_t rest[20];
        net_read_all(fd, rest, sizeof(rest), -1);
        struct in6_addr mcaddr;
        memcpy(&mcaddr, rest + 2, 16);
        uint16_t mport = wire_get16(rest);
        (void)mcast_subscribe(gid_join, &mcaddr, mport);
        uint16_t nbm = wire_get16(rest + 18);
        printf("rejoint: %u membre(s)\n", nbm);
        for (uint16_t i = 0; i < nbm; i++) {
            uint8_t blk[4 + PAROLES_NOM_LEN];
            net_read_all(fd, blk, sizeof(blk), -1);
            printf("  #%u %.10s\n", wire_get16(blk), blk + 4);
        }
        close(fd);
        return 0;
    }
    net_read_all(fd, rh + 2, 2, -1);
    printf("erreur\n");
    close(fd);
    return 1;
}

static int cmd_members(int argc, char **argv) {
    if (!g_reg || argc < 2)
        return 1;
    uint16_t idg = (uint16_t)atoi(argv[1]);
    int fd = tcp_connect();
    uint8_t m[8];
    wire_put16(m, PAROLES_REQ_LIST_MEMBERS);
    wire_put16(m + 2, g_uid);
    wire_put16(m + 4, idg);
    wire_put16(m + 6, 0);
    net_write_all(fd, m, sizeof(m));
    uint8_t rh[6];
    net_read_all(fd, rh, sizeof(rh), -1);
    if (wire_get16(rh) == PAROLES_ERR) {
        close(fd);
        return 1;
    }
    uint16_t nb = wire_get16(rh + 4);
    printf("%u entrée(s)\n", nb);
    for (uint16_t i = 0; i < nb; i++) {
        uint8_t blk[4 + PAROLES_NOM_LEN];
        net_read_all(fd, blk, sizeof(blk), -1);
        printf("  #%u %.10s\n", wire_get16(blk), blk + 4);
    }
    close(fd);
    return 0;
}

static int cmd_post(int argc, char **argv) {
    if (!g_reg || argc < 3)
        return 1;
    uint16_t gid = (uint16_t)atoi(argv[1]);
    char body[8192];
    body[0] = '\0';
    for (int i = 2; i < argc; i++) {
        if (i > 2)
            strncat(body, " ", sizeof(body) - strlen(body) - 1);
        strncat(body, argv[i], sizeof(body) - strlen(body) - 1);
    }
    size_t bl = strlen(body);
    int fd = tcp_connect();
    size_t tot = 4 + 6 + bl;
    uint8_t *msg = malloc(tot);
    wire_put16(msg, PAROLES_REQ_POST);
    wire_put16(msg + 2, g_uid);
    wire_put16(msg + 4, gid);
    wire_put16(msg + 6, 0);
    wire_put16(msg + 8, (uint16_t)bl);
    memcpy(msg + 10, body, bl);
    net_write_all(fd, msg, tot);
    free(msg);
    uint8_t r[8];
    net_read_all(fd, r, sizeof(r), -1);
    close(fd);
    if (wire_get16(r) == PAROLES_RESP_POST)
        printf("billet #%u\n", wire_get16(r + 4));
    else
        printf("erreur\n");
    return 0;
}

static int cmd_reply(int argc, char **argv) {
    if (!g_reg || argc < 4)
        return 1;
    uint16_t gid = (uint16_t)atoi(argv[1]);
    uint16_t numb = (uint16_t)atoi(argv[2]);
    char body[8192];
    body[0] = '\0';
    for (int i = 3; i < argc; i++) {
        if (i > 3)
            strncat(body, " ", sizeof(body) - strlen(body) - 1);
        strncat(body, argv[i], sizeof(body) - strlen(body) - 1);
    }
    size_t bl = strlen(body);
    int fd = tcp_connect();
    size_t tot = 4 + 10 + bl;
    uint8_t *msg = malloc(tot);
    wire_put16(msg, PAROLES_REQ_REPLY);
    wire_put16(msg + 2, g_uid);
    wire_put16(msg + 4, gid);
    wire_put16(msg + 6, 0);
    wire_put16(msg + 8, numb);
    wire_put16(msg + 10, 0);
    wire_put16(msg + 12, (uint16_t)bl);
    memcpy(msg + 14, body, bl);
    net_write_all(fd, msg, tot);
    free(msg);
    uint8_t r[8];
    net_read_all(fd, r, sizeof(r), -1);
    close(fd);
    if (wire_get16(r) == PAROLES_RESP_REPLY)
        printf("réponse #%u au billet #%u\n", wire_get16(r + 6), wire_get16(r + 4));
    else
        printf("erreur\n");
    return 0;
}

static int cmd_feed(int argc, char **argv) {
    if (!g_reg || argc < 4)
        return 1;
    uint16_t gid = (uint16_t)atoi(argv[1]);
    uint16_t numb = (uint16_t)atoi(argv[2]);
    uint16_t numr = (uint16_t)atoi(argv[3]);
    int fd = tcp_connect();
    uint8_t m[12];
    wire_put16(m, PAROLES_REQ_LIST_FEED);
    wire_put16(m + 2, g_uid);
    wire_put16(m + 4, gid);
    wire_put16(m + 6, 0);
    wire_put16(m + 8, numb);
    wire_put16(m + 10, numr);
    net_write_all(fd, m, sizeof(m));
    uint8_t rh[6];
    net_read_all(fd, rh, sizeof(rh), -1);
    if (wire_get16(rh) == PAROLES_ERR) {
        close(fd);
        return 1;
    }
    uint16_t nb = wire_get16(rh + 4);
    printf("%u message(s)\n", nb);
    for (uint16_t i = 0; i < nb; i++) {
        uint8_t hd[10];
        net_read_all(fd, hd, sizeof(hd), -1);
        uint16_t author = wire_get16(hd);
        uint16_t bn = wire_get16(hd + 4);
        uint16_t rn = wire_get16(hd + 6);
        uint16_t len = wire_get16(hd + 8);
        char *buf = malloc(len + 1);
        net_read_all(fd, (uint8_t *)buf, len, -1);
        buf[len] = '\0';
        printf("  [auteur=%u billet=%u rep=%u] %s\n", author, bn, rn, buf);
        free(buf);
    }
    close(fd);
    return 0;
}

static void print_help(void) {
    printf(
        "Commandes:\n"
        "  register <pseudo>\n"
        "  create <nom>\n"
        "  invite <idg> <id> ...\n"
        "  pending\n"
        "  accept|refuse|leave <idg>\n"
        "  members <idg|0>\n"
        "  post <idg> <texte...>\n"
        "  reply <idg> <numb> <texte...>\n"
        "  feed <idg> <numb> <numr>  (0 0 = tout depuis le début)\n"
        "  help | quit\n"
        "(Apres register : ecoute UDP ; apres create/accept : multicast groupe.)\n");
}

static void process_line(char *line) {
    char *cmd = strtok(line, " \t\n\r");
    if (!cmd)
        return;
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        print_help();
        return;
    }
    char *args[64];
    int n = 0;
    args[n++] = cmd;
    char *t;
    while (n < 63 && (t = strtok(NULL, " \t\n\r")))
        args[n++] = t;
    args[n] = NULL;
    if (strcmp(cmd, "register") == 0)
        cmd_register(n, args);
    else if (strcmp(cmd, "create") == 0)
        cmd_create(n, args);
    else if (strcmp(cmd, "invite") == 0)
        cmd_invite(n, args);
    else if (strcmp(cmd, "pending") == 0)
        cmd_pending();
    else if (strcmp(cmd, "accept") == 0 || strcmp(cmd, "refuse") == 0 ||
             strcmp(cmd, "leave") == 0)
        cmd_answer(n, args);
    else if (strcmp(cmd, "members") == 0)
        cmd_members(n, args);
    else if (strcmp(cmd, "post") == 0)
        cmd_post(n, args);
    else if (strcmp(cmd, "reply") == 0)
        cmd_reply(n, args);
    else if (strcmp(cmd, "feed") == 0)
        cmd_feed(n, args);
    else
        fprintf(stderr, "commande inconnue\n");
}

int main(int argc, char **argv) {
    int argi = 1;
    if (argc >= 2 && strcmp(argv[1], "-v") == 0) {
        g_verbose = 1;
        argi++;
    }
    if (argc - argi < 2) {
        fprintf(stderr, "Usage: %s [-v] <ipv6> <port> [if_multicast]\n", argv[0]);
        fprintf(stderr, "  if_multicast : ex. lo, eth0 (defaut: lo)\n");
        return 1;
    }
    strncpy(g_host, argv[argi], sizeof(g_host) - 1);
    g_port = (uint16_t)atoi(argv[argi + 1]);
    if (argc - argi >= 3)
        strncpy(g_ifname, argv[argi + 2], sizeof(g_ifname) - 1);

    char linebuf[4096];
    size_t linepos = 0;
    print_help();
    printf("> ");
    fflush(stdout);

    for (;;) {
        struct pollfd pfds[32];
        int np = 0;
        pfds[np].fd = STDIN_FILENO;
        pfds[np].events = POLLIN;
        np++;
        if (g_uc_sock >= 0) {
            pfds[np].fd = g_uc_sock;
            pfds[np].events = POLLIN;
            np++;
        }
        for (int i = 0; i < g_n_mc; i++) {
            pfds[np].fd = g_mc_socks[i];
            pfds[np].events = POLLIN;
            np++;
        }
        if (poll(pfds, (nfds_t)np, -1) <= 0)
            continue;
        for (int i = 0; i < np; i++) {
            if (!(pfds[i].revents & POLLIN))
                continue;
            if (pfds[i].fd == STDIN_FILENO) {
                char ch;
                ssize_t r = read(STDIN_FILENO, &ch, 1);
                if (r <= 0)
                    return 0;
                if (ch == '\n') {
                    linebuf[linepos] = '\0';
                    linepos = 0;
                    if (strcmp(linebuf, "quit") == 0 || strcmp(linebuf, "q") == 0)
                        return 0;
                    if (linebuf[0])
                        process_line(linebuf);
                    printf("> ");
                    fflush(stdout);
                } else if (linepos < sizeof(linebuf) - 1)
                    linebuf[linepos++] = ch;
            } else if (g_uc_sock >= 0 && pfds[i].fd == g_uc_sock) {
                handle_notif_datagram(g_uc_sock, "UDP");
            } else {
                for (int j = 0; j < g_n_mc; j++) {
                    if (g_mc_socks[j] == pfds[i].fd)
                        handle_notif_datagram(g_mc_socks[j], "MCAST");
                }
            }
        }
    }
}
