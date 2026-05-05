/* Extrait — PRCursor/src/server.c — handle_new_group : réponse NEW_GROUP_OK (4) */

    wire_put_u8(&p, PAROLES_CODEREQ_NEW_GROUP_OK);
    wire_put_u32_be(&p, g->idg);
    wire_put_u16_be(&p, g->mcast_port);
    memcpy(p, &g->mcast_ip, 16);
    p += 16;
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
