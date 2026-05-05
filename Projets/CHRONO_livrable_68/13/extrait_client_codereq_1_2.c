/* Extrait — PRCursor/src/client.c — CODEREQ 1 → REG_OK 2 */

static int cmd_reg(const char *host, uint16_t port, const char *name, const char *pub_pem) {
    unsigned char msg[256], resp[4096];
    unsigned char *p = msg;
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
    int n = one_cmd_plain(host, port, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 2 + PAROLES_CLE_LEN) return -1;
    const unsigned char *q = resp;
    size_t left = (size_t)n;
    uint8_t c;
    if (wire_get_u8(&q, &left, &c) < 0 || c != PAROLES_CODEREQ_REG_OK) return -1;
    uint32_t id;
    uint16_t udp;
    if (wire_get_u32_be(&q, &left, &id) < 0) return -1;
    if (wire_get_u16_be(&q, &left, &udp) < 0) return -1;
    printf("OK id=%u udp=%u\n", id, (unsigned)udp);
    return 0;
}
