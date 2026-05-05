/* Extrait — PRCursor/src/server.c — handle_post : NUMB = next_billet++, POST_OK */

    Post *po = &g->posts[g->nposts++];
    memset(po, 0, sizeof *po);
    po->numb = g->next_billet++;
    po->author = uid;
    po->len = dlen;
    po->body = malloc(dlen);
    if (!po->body) return -1;
    memcpy(po->body, q, dlen);
    /* ... feed_push, notif_mcast ... */
    unsigned char out[32];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_POST_OK);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, po->numb);
    notif_mcast(g, PAROLES_NOTIF_NEW_MSG);
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
