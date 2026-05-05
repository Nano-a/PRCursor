/* Extrait — PRCursor/src/server.c — serve_client : lecture en-tête avec timeout */

    unsigned char hdr[1];
    unsigned char buf[256 * 1024];
    if (conn_readn(g_io_ssl, cfd, hdr, 1, PAROLES_TCP_TIMEOUT_MS) < 0) goto end;
    uint8_t code = hdr[0];
    /* ... */
    if (code == PAROLES_CODEREQ_REG) {
        if (conn_readn(g_io_ssl, cfd, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, PAROLES_TCP_TIMEOUT_MS) < 0)
            goto bad;
        /* ... */
    }
