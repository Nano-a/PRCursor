/* PRCursor src/server.c — NUM attendu (auth_nonce) et incrément après auth OK
 * Réf. handle_reg : memset(u, 0, sizeof *u) → auth_nonce == 0 à l'inscription.
 * Lignes : ~618–635 dans do_client_auth
 */

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
