/* Extrait — PRCursor/src/server.c — émission multicast (6 octets : code BE + idg BE) */

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
