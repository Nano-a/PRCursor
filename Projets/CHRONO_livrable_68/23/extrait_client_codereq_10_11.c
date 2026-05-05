/* Extrait — PRCursor/src/client.c — LIST_MEM (10) / LIST_MEM_OK (11) */

static int cmd_listmem(const char *host, uint16_t port, uint32_t uid, uint32_t idg) {
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_MEM);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 4) return -1;
    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1;
    uint32_t ig, nb;
    if (wire_get_u32_be(&q, &left, &ig) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &nb) < 0) return -1;
    printf("membres idg=%u nb=%u\n", ig, nb);
    for (uint32_t i = 0; i < nb; i++) {
        uint32_t ui;
        if (wire_get_u32_be(&q, &left, &ui) < 0) return -1;
        if (left < PAROLES_NOM_LEN) return -1;
        char nm[PAROLES_NOM_LEN + 1];
        memcpy(nm, q, PAROLES_NOM_LEN);
        nm[PAROLES_NOM_LEN] = 0;
        q += PAROLES_NOM_LEN;
        left -= PAROLES_NOM_LEN;
        printf("  %u %s\n", ui, nm);
    }
    return 0;
}
