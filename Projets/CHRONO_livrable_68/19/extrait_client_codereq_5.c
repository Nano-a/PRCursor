/* Extrait — PRCursor/src/client.c — CODEREQ 5 INVITE */

static int cmd_invite(const char *host, uint16_t port, uint32_t admin, uint32_t idg, int ninv,
                      uint32_t *ids) {
    unsigned char msg[2048], resp[64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_INVITE);
    wire_put_u32_be(&p, admin);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, (uint32_t)ninv);
    for (int i = 0; i < ninv; i++) {
        wire_put_u32_be(&p, ids[i]);
        wire_put_zeros(&p, PAROLES_INV_PAD);
    }
    int n = one_cmd(host, port, admin, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1) return -1;
    return resp[0] == PAROLES_CODEREQ_ACK ? 0 : -1;
}
