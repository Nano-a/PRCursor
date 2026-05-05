/* Extrait — PRCursor/src/client.c — FEED (16) / FEED_OK (17) */

static int cmd_feed(const char *host, uint16_t port, uint32_t uid, uint32_t idg, uint32_t numb,
                    uint32_t numr) {
    unsigned char msg[64], resp[65536];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_FEED);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, numb);
    wire_put_u32_be(&p, numr);
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 4) return -1;
    if (resp[0] != PAROLES_CODEREQ_FEED_OK) return -1;
    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1;
    uint32_t ig, nb;
    if (wire_get_u32_be(&q, &left, &ig) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &nb) < 0) return -1;
    printf("feed idg=%u nb=%u\n", ig, nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t author, nb2, nr;
        uint16_t dl;
        if (wire_get_u32_be(&q, &left, &author) < 0) return -1;
        if (wire_get_u32_be(&q, &left, &nb2) < 0) return -1;
        if (wire_get_u32_be(&q, &left, &nr) < 0) return -1;
        if (wire_get_u16_be(&q, &left, &dl) < 0) return -1;
        if (left < dl) return -1;
        char buf[PAROLES_MAX_BODY + 1];
        if (dl > sizeof buf - 1) return -1;
        memcpy(buf, q, dl);
        buf[dl] = 0;
        q += dl;
        left -= dl;
        printf("  [%u billet=%u rep=%u] %s\n", author, nb2, nr, buf);
    }
    return 0;
}
