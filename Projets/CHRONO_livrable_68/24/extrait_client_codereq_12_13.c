/* Extrait — PRCursor/src/client.c — POST (12) / POST_OK (13) */

static int cmd_post(const char *host, uint16_t port, uint32_t uid, uint32_t idg, const char *txt) {
    unsigned char msg[PAROLES_MAX_BODY + 64], resp[64];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_POST);
    wire_put_u32_be(&p, uid);
    wire_put_u32_be(&p, idg);
    uint16_t tl = (uint16_t)strlen(txt);
    wire_put_u16_be(&p, tl);
    memcpy(p, txt, tl);
    p += tl;
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 4) return -1;
    if (resp[0] != PAROLES_CODEREQ_POST_OK) return -1;
    const unsigned char *q = resp + 1;
    size_t left = (size_t)n - 1;
    uint32_t ig, numb;
    if (wire_get_u32_be(&q, &left, &ig) < 0) return -1;
    if (wire_get_u32_be(&q, &left, &numb) < 0) return -1;
    printf("OK billet numb=%u\n", numb);
    return 0;
}
