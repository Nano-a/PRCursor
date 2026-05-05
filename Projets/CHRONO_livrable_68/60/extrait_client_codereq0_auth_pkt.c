/* PRCursor src/client.c — paquet CODEREQ 0 + vérification AUTH_OK dans one_cmd_authed
 * Lignes source : ~134–175 (après TLS et g_io_ssl = ssl)
 */

    uint32_t num = read_nonce(port, uid);
    unsigned char tbs[9];
    unsigned char *tp = tbs;
    wire_put_u8(&tp, PAROLES_CODEREQ_AUTH);
    wire_put_u32_be(&tp, uid);
    wire_put_u32_be(&tp, num);
    unsigned char sig[128];
    size_t slen = sizeof sig;
    if (paroles_ed25519_sign(g_auth_priv, tbs, 9, sig, &slen) < 0) goto fail;
    if (slen != PAROLES_ED25519_SIG_LEN) goto fail;
    unsigned char authpkt[96];
    tp = authpkt;
    wire_put_u8(&tp, PAROLES_CODEREQ_AUTH);
    wire_put_u32_be(&tp, uid);
    wire_put_u32_be(&tp, num);
    wire_put_u16_be(&tp, (uint16_t)slen);
    memcpy(tp, sig, slen);
    tp += slen;
    if (conn_writen(g_io_ssl, fd, authpkt, (size_t)(tp - authpkt)) < 0) goto fail;

    unsigned char aresp[128];
    if (conn_readn(g_io_ssl, fd, aresp, 1 + 4 + 4 + 2 + PAROLES_ED25519_SIG_LEN, PAROLES_TCP_TIMEOUT_MS) < 0)
        goto fail;
    const unsigned char *q = aresp;
    size_t left = sizeof aresp;
    uint8_t ac;
    if (wire_get_u8(&q, &left, &ac) < 0 || ac != PAROLES_CODEREQ_AUTH_OK) goto fail;
    uint32_t rid, rnum;
    uint16_t lsig;
    if (wire_get_u32_be(&q, &left, &rid) < 0) goto fail;
    if (wire_get_u32_be(&q, &left, &rnum) < 0) goto fail;
    if (wire_get_u16_be(&q, &left, &lsig) < 0) goto fail;
    if (lsig != PAROLES_ED25519_SIG_LEN || left < PAROLES_ED25519_SIG_LEN) goto fail;
    if (rid != uid || rnum != num) goto fail;
    unsigned char stbs[9];
    tp = stbs;
    wire_put_u8(&tp, PAROLES_CODEREQ_AUTH_OK);
    wire_put_u32_be(&tp, rid);
    wire_put_u32_be(&tp, rnum);
    if (g_srv_pub_verify && paroles_ed25519_verify(g_srv_pub_verify, stbs, 9, q, PAROLES_ED25519_SIG_LEN) < 0)
        goto fail;
    write_nonce(port, uid, num + 1u);
