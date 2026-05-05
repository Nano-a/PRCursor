/* Extrait — PRCursor/src/server.c — handle_reply : NUMR à partir de 1 */

    Post *po = find_post(g, numb);
    if (!po) return -1;
    po->next_reply++;
    uint32_t numr = po->next_reply;
    /* ... FeedItem, feed_push ... */
    unsigned char out[32];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_REPLY_OK);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, numb);
    wire_put_u32_be(&p, numr);
    notif_mcast(g, PAROLES_NOTIF_NEW_MSG);
    User *auth = find_user(po->author);
    if (auth && auth->id != uid) notif_udp_user(auth, PAROLES_NOTIF_FETCH, idg);
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
