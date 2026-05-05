/* Extrait — PRCursor/src/client.c — dans cmd_reg : CLE 113 octets nuls si pas de PEM (étape 1) */

    wire_put_u8(&p, PAROLES_CODEREQ_REG);
    pad_nom(p, name);
    p += PAROLES_NOM_LEN;
    if (pub_pem) {
        unsigned char cle[PAROLES_CLE_LEN];
        if (fill_ed25519_cle_from_pem(pub_pem, cle) < 0) return -1;
        memcpy(p, cle, PAROLES_CLE_LEN);
        p += PAROLES_CLE_LEN;
    } else {
        wire_put_zeros(&p, PAROLES_CLE_LEN);
    }
