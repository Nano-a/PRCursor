/* Extrait — PRCursor/src/server.c — feed_index_after + handle_feed complet */

static ssize_t feed_index_after(Group *g, uint32_t numb, uint32_t numr) {
    for (size_t i = 0; i < g->nfeed; i++) {
        FeedItem *fi = g->feed[i];
        if (fi->numb == numb && fi->numr == numr) return (ssize_t)i;
    }
    return -1;
}

static int handle_feed(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    if (blen != 4 + 4 + 4) return -1;
    const unsigned char *q = body;
    size_t left = blen;
    uint32_t idg, numb, numr;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &numb) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &numr) < 0) return -1;
    Group *g = find_group(idg);
    if (!g || !group_is_member(g, uid)) return -1;
    ssize_t start = feed_index_after(g, numb, numr);
    if (start < 0) return -1;
    size_t from = (size_t)start + 1;
    size_t cap = 65536;
    unsigned char *out = malloc(cap);
    if (!out) return -1;
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_FEED_OK);
    wire_put_u32_be(&p, idg);
    unsigned char *msp = p;
    wire_put_u32_be(&p, 0);
    uint32_t cnt = 0;
    for (size_t i = from; i < g->nfeed; i++) {
        FeedItem *fi = g->feed[i];
        size_t need = (size_t)(p - out) + 4 + 4 + 4 + 2 + fi->len;
        if (need > cap) {
            cap = need * 2;
            size_t off = (size_t)(p - out);
            out = realloc(out, cap);
            p = out + off;
            msp = out + 1 + 4;
        }
        wire_put_u32_be(&p, fi->author);
        wire_put_u32_be(&p, fi->numb);
        wire_put_u32_be(&p, fi->numr);
        wire_put_u16_be(&p, fi->len);
        memcpy(p, fi->data, fi->len);
        p += fi->len;
        cnt++;
        Post *pp = find_post(g, fi->numb);
        if (pp && pp->author != uid) {
            User *au = find_user(pp->author);
            if (au) notif_udp_user(au, PAROLES_NOTIF_FETCH, idg);
        }
    }
    unsigned char *pp = msp;
    wire_put_u32_be(&pp, cnt);
    int r = conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
    free(out);
    return r;
}
