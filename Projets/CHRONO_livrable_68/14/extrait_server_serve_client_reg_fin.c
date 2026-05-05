/* Extrait — PRCursor/src/server.c — serve_client : une requête REG puis fin de connexion */

    if (code == PAROLES_CODEREQ_REG) {
        if (conn_readn(g_io_ssl, cfd, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, PAROLES_TCP_TIMEOUT_MS) < 0)
            goto bad;
        if (dispatch(cfd, peer, code, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, 0) < 0) goto err;
        goto end;
    }

/* ... plus loin, label end: fermeture */

end:
    g_io_ssl = NULL;
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(cfd);
