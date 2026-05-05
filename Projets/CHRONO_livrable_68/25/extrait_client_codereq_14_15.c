/* Extrait — PRCursor/src/client.c — REPLY (14) / REPLY_OK (15) */

static int cmd_reply(const char *host, uint16_t port, uint32_t uid, uint32_t idg, uint32_t numb,
                     const char *txt) {
    unsigned char msg[PAROLES_MAX_BODY + 64], resp[64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_REPLY);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, numb);
    uint16_t tl = (uint16_t)strlen(txt);
    wire_put_u16_be(&p, tl);
    memcpy(p, txt, tl);
    p += tl;
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 4 + 4) return -1;
    if (resp[0] != PAROLES_CODEREQ_REPLY_OK) return -1;
    printf("OK reply\n");
    return 0;
}
