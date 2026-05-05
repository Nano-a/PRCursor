/* Extrait — PRCursor/src/server.c — handle_list_inv : CODEREQ 7 */

static int handle_list_inv(int fd, uint32_t uid, const unsigned char *body, size_t blen) {
    (void)body;
    if (blen) return -1;
    size_t cap = 8192;
    unsigned char *out = malloc(cap);
    if (!out) return -1;
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_INV_OK);
    unsigned char *nbp = p;
    wire_put_u32_be(&p, 0);
    uint32_t nb = 0;
    for (int gi = 0; gi < MAX_GROUPS; gi++) {
        Group *g = &groups[gi];
        if (!g->used || g->closed) continue;
        if (!group_is_pending(g, uid)) continue;
        uint16_t ln = (uint16_t)strlen(g->name);
        size_t need = (size_t)(p - out) + 4 + 2 + ln + PAROLES_NOM_LEN;
        if (need > cap) {
            cap = need * 2;
            size_t off = (size_t)(p - out);
            out = realloc(out, cap);
            p = out + off;
            nbp = out + 1 + 0;
        }
        wire_put_u32_be(&p, g->idg);
        wire_put_u16_be(&p, ln);
        memcpy(p, g->name, ln);
        p += ln;
        User *adm = find_user(g->admin_id);
        if (adm)
            memcpy(p, adm->nom, PAROLES_NOM_LEN);
        else
            memset(p, 0, PAROLES_NOM_LEN);
        p += PAROLES_NOM_LEN;
        nb++;
    }
    unsigned char *p2 = nbp;
    wire_put_u32_be(&p2, nb);
    int r = conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
    free(out);
    return r;
}
