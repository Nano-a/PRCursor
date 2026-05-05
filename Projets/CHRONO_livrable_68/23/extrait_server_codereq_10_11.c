/* Extrait — PRCursor/src/server.c — handle_list_mem */

static int handle_list_mem(int fd, uint32_t uid, uint32_t idg) {
    (void)uid;
    unsigned char out[8192];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_LIST_MEM_OK);
    if (idg == 0) {
        wire_put_u32_be(&p, 0);
        uint32_t nb = 0;
        unsigned char *nbspot = p;
        wire_put_u32_be(&p, 0);
        for (int i = 0; i < MAX_USERS; i++) {
            if (!users[i].used) continue;
            wire_put_u32_be(&p, users[i].id);
            memcpy(p, users[i].nom, PAROLES_NOM_LEN);
            p += PAROLES_NOM_LEN;
            nb++;
        }
        unsigned char *pp = nbspot;
        wire_put_u32_be(&pp, nb);
        return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
    }
    Group *g = find_group(idg);
    if (!g) return -1;
    if (!group_is_member(g, uid)) return -1;
    wire_put_u32_be(&p, idg);
    wire_put_u32_be(&p, (uint32_t)g->nmem);
    for (size_t i = 0; i < g->nmem; i++) {
        User *uu = find_user(g->mem[i]);
        if (!uu) return -1;
        wire_put_u32_be(&p, g->mem[i]);
        memcpy(p, uu->nom, PAROLES_NOM_LEN);
        p += PAROLES_NOM_LEN;
    }
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
}
