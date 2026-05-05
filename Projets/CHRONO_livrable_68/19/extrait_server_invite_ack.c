/* Extrait — PRCursor/src/server.c — ACK (24) + handle_invite */

static void send_ack(int fd) {
    unsigned char b[1 + PAROLES_ERR_TAIL];
    memset(b, 0, sizeof b);
    b[0] = PAROLES_CODEREQ_ACK;
    conn_writen(g_io_ssl, fd, b, sizeof b);
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
