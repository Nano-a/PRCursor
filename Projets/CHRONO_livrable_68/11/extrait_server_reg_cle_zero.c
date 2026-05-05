/* Extrait — PRCursor/src/server.c — étape 1 : refus CLE non nulle, stockage et REG_OK avec CLE zéros */

#ifndef PAROLES_ACCEPT_REAL_CLE_113
static int cle_is_zero(const uint8_t *c) {
    for (int i = 0; i < PAROLES_CLE_LEN; i++)
        if (c[i]) return 0;
    return 1;
}
#endif

/* Dans handle_reg, après vérification blen : */
#ifndef PAROLES_ACCEPT_REAL_CLE_113
    if (!cle_is_zero(body + PAROLES_NOM_LEN)) return -1;
#endif

/* Après création du User u : clé utilisateur côté serveur (étape 1) */
#ifndef PAROLES_ACCEPT_REAL_CLE_113
    memset(u->cle, 0, sizeof u->cle);
#endif

/* Corps de la réponse REG_OK : ID, PORTUDP, puis CLE 113 octets */
    wire_put_u8(&p, PAROLES_CODEREQ_REG_OK);
    wire_put_u32_be(&p, u->id);
    wire_put_u16_be(&p, u->udp_port);
#ifndef PAROLES_ACCEPT_REAL_CLE_113
    wire_put_zeros(&p, PAROLES_CLE_LEN);
#endif
