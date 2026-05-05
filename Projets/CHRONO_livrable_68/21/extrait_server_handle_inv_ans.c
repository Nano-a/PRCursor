/* Extrait — PRCursor/src/server.c — handle_inv_ans : AN 2 (quitte), 0 (refus), 1 (accepte → JOIN_OK) */

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
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
}
