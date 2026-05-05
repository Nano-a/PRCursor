/* Extrait — PRCursor/src/client.c — LIST_INV (6) / LIST_INV_OK (7) */

static int cmd_listinv(const char *host, uint16_t port, uint32_t uid) {
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_INV);
    wire_put_u32_be(&p, uid);
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 5) return -1;
    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1;
    uint32_t nb;
    if (wire_get_u32_be(&q, &left, &nb) < 0) return -1;
    printf("invitations %u\n", nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t ig;
        uint16_t ln;
        if (wire_get_u32_be(&q, &left, &ig) < 0) return -1;
        if (wire_get_u16_be(&q, &left, &ln) < 0) return -1;
        if (left < ln + PAROLES_NOM_LEN) return -1;
        char gn[256];
        memcpy(gn, q, ln);
        gn[ln] = 0;
        q += ln;
        char adm[PAROLES_NOM_LEN + 1];
        memcpy(adm, q, PAROLES_NOM_LEN);
        adm[PAROLES_NOM_LEN] = 0;
        q += PAROLES_NOM_LEN;
        left -= (size_t)ln + PAROLES_NOM_LEN;
        printf("  idg=%u groupe=%s admin=%s\n", ig, gn, adm);
    }
    return 0;
}
