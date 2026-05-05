/* Extrait — PRCursor/src/client.c — INV_ANS (8), lecture ACK (24) ou JOIN_OK (9) */

static int cmd_ans(const char *host, uint16_t port, uint32_t uid, uint32_t idg, int an) {
    unsigned char msg[32], resp[8192];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_INV_ANS);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u8(&p, (uint8_t)an);
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1) return -1;
    if (resp[0] == PAROLES_CODEREQ_ACK) {
        printf("OK ack\n");
        return 0;
    }
    if (resp[0] == PAROLES_CODEREQ_JOIN_OK) {
        printf("OK join\n");
        return 0;
    }
    return -1;
}
