/* Extrait — PRCursor/src/server.c — corps message REG_OK après création du User u */

    unsigned char out[256];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_REG_OK);
    wire_put_u32_be(&p, u->id);
    wire_put_u16_be(&p, u->udp_port);
#ifdef PAROLES_ACCEPT_REAL_CLE_113
    memcpy(p, u->cle, PAROLES_CLE_LEN);
    p += PAROLES_CLE_LEN;
#else
    wire_put_zeros(&p, PAROLES_CLE_LEN);
#endif
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
