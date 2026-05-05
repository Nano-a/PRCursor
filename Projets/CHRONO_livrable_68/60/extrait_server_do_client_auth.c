/* PRCursor src/server.c — do_client_auth + send_auth_ok (CODEREQ 0 / 25)
 * Lignes source : ~600–666
 */

static int do_client_auth(int cfd, uint32_t *aid, uint32_t *anum) {
    /* Un seul read après CODEREQ=0 : évite blocages SSL_read fragmentés. */
    unsigned char h[4 + 4 + 2 + PAROLES_ED25519_SIG_LEN];
    if (conn_readn(g_io_ssl, cfd, h, sizeof h, PAROLES_TCP_TIMEOUT_MS) < 0) return -1;
    const unsigned char *q = h;
    size_t le = sizeof h;
    uint32_t id, num;
    uint16_t lsig;
    if (wire_get_u32_be(&q, &le, &id) < 0) return -1;
    if (wire_get_u32_be(&q, &le, &num) < 0) return -1;
    if (wire_get_u16_be(&q, &le, &lsig) < 0) return -1;
    if (lsig != PAROLES_ED25519_SIG_LEN || le < PAROLES_ED25519_SIG_LEN) return -1;
    const unsigned char *sig = q;
    User *u = find_user(id);
    if (!u) {
        vlog("auth: utilisateur %u inconnu\n", id);
        return -1;
    }
    if (num != u->auth_nonce) {
        vlog("auth: NUM %u != attendu %u (id=%u)\n", num, u->auth_nonce, id);
        return -1;
    }
    unsigned char tbs[9];
    unsigned char *p = tbs;
    wire_put_u8(&p, PAROLES_CODEREQ_AUTH);
    wire_put_u32_be(&p, id);
    wire_put_u32_be(&p, num);
    EVP_PKEY *pk = paroles_ed25519_pubkey_from_cle(u->cle);
    if (!pk) return -1;
    int ok = paroles_ed25519_verify(pk, tbs, 9, sig, PAROLES_ED25519_SIG_LEN);
    EVP_PKEY_free(pk);
    if (ok < 0) {
        vlog("auth: signature ED25519 invalide (id=%u num=%u)\n", id, num);
        return -1;
    }
    u->auth_nonce++;
    *aid = id;
    *anum = num;
    return 0;
}

static int send_auth_ok(int cfd, uint32_t id, uint32_t num) {
    unsigned char tbs[9];
    unsigned char *p = tbs;
    wire_put_u8(&p, PAROLES_CODEREQ_AUTH_OK);
    wire_put_u32_be(&p, id);
    wire_put_u32_be(&p, num);
    unsigned char sig[128];
    size_t slen = sizeof sig;
    if (paroles_ed25519_sign(g_srv_sign_key, tbs, 9, sig, &slen) < 0) {
        vlog("send_auth_ok: paroles_ed25519_sign a échoué\n");
        return -1;
    }
    if (slen != PAROLES_ED25519_SIG_LEN) {
        vlog("send_auth_ok: longueur signature %zu\n", slen);
        return -1;
    }
    unsigned char out[128];
    p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_AUTH_OK);
    wire_put_u32_be(&p, id);
    wire_put_u32_be(&p, num);
    wire_put_u16_be(&p, (uint16_t)slen);
    memcpy(p, sig, slen);
    p += slen;
    return conn_writen(g_io_ssl, cfd, out, (size_t)(p - out));
}
