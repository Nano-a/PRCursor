/* Extrait — PRCursor/src/client.c — CODEREQ 3 / lecture CODEREQ 4 */

static int cmd_newgroup(const char *host, uint16_t port, uint32_t uid, const char *gname) {
    unsigned char msg[512], resp[256];
    unsigned char *p = msg;
    wire_put_u8(&p, PAROLES_CODEREQ_NEW_GROUP);
    wire_put_u32_be(&p, uid);
    uint16_t gl = (uint16_t)strlen(gname);
    wire_put_u16_be(&p, gl);
    memcpy(p, gname, gl);
    p += gl;
    int n = one_cmd(host, port, uid, msg, (size_t)(p - msg), resp, sizeof resp);
    if (n < 1 + 4 + 2 + 16) return -1;
    const unsigned char *q = resp;
    size_t left = (size_t)n;
    uint8_t c;
    if (wire_get_u8(&q, &left, &c) < 0 || c != PAROLES_CODEREQ_NEW_GROUP_OK) return -1;
    uint32_t idg;
    uint16_t mport;
    if (wire_get_u32_be(&q, &left, &idg) < 0) return -1;
    if (wire_get_u16_be(&q, &left, &mport) < 0) return -1;
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, q, ip, sizeof ip);
    printf("OK idg=%u mcast=%s port=%u\n", idg, ip, (unsigned)mport);
    return 0;
}
