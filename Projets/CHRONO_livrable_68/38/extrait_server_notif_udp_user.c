/* Extrait — PRCursor — UDP perso (même layout 6 octets que mcast) */

#define PAROLES_NOTIF_INV_UDP 22
#define PAROLES_NOTIF_FETCH 23

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

/* Appels typiques : handle_invite → notif_udp_user(uu, PAROLES_NOTIF_INV_UDP, idg);
 * handle_reply / handle_feed → notif_udp_user(auth, PAROLES_NOTIF_FETCH, idg); */
