/* PRCursor src/server.c — N°59 auth sauf inscription (extraits à fusionner)
 * Réf. : dispatch ~668–677 ; serve_client ~833–857
 */

/* --- début de dispatch (avant le switch code) --- */
    if (sess_uid && code != PAROLES_CODEREQ_REG) {
        if (blen < 4) return -1;
        const unsigned char *q = body;
        size_t left = blen;
        uint32_t wire_uid;
        if (wire_get_u32_be(&q, &left, &wire_uid) < 0) return -1;
        if (wire_uid != sess_uid) return -1;
    }

/* --- dans serve_client : après conn_readn hdr[0] et uint32_t sess_uid = 0 --- */
    if (code == PAROLES_CODEREQ_REG) {
        if (conn_readn(g_io_ssl, cfd, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, PAROLES_TCP_TIMEOUT_MS) < 0)
            goto bad;
        if (dispatch(cfd, peer, code, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, 0) < 0) goto err;
        goto end;
    }

    if (g_srv_sign_key) {
        if (code != PAROLES_CODEREQ_AUTH) goto err;
        uint32_t aid, anum;
        if (do_client_auth(cfd, &aid, &anum) < 0) goto err;
        if (send_auth_ok(cfd, aid, anum) < 0) goto err;
        sess_uid = aid;
        if (conn_readn(g_io_ssl, cfd, hdr, 1, PAROLES_TCP_TIMEOUT_MS) < 0) goto end;
        code = hdr[0];
        if (code == PAROLES_CODEREQ_REG) goto err;
    }

    if (serve_business_switch(cfd, peer, code, sess_uid) < 0) goto err;
