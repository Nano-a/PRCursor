/* PRCursor src/server.c — handle_reg : CLE 113 réelle si PAROLES_ACCEPT_REAL_CLE_113
 * Fusionner dans server.c (voir aussi #ifndef branche cle_is_zero pour étape 1).
 * Lignes source : ~226–264
 */

static int handle_reg(int fd, struct sockaddr_in6 *peer, const unsigned char *body, size_t blen) {
    if (blen != PAROLES_NOM_LEN + PAROLES_CLE_LEN) return -1;
#ifndef PAROLES_ACCEPT_REAL_CLE_113
    if (!cle_is_zero(body + PAROLES_NOM_LEN)) return -1;
#endif
    int slot = -1;
    for (int i = 0; i < MAX_USERS; i++)
        if (!users[i].used) {
            slot = i;
            break;
        }
    if (slot < 0) return -1;
    User *u = &users[slot];
    memset(u, 0, sizeof *u);
    u->used = 1;
    u->id = next_uid++;
    memcpy(u->nom, body, PAROLES_NOM_LEN);
#ifdef PAROLES_ACCEPT_REAL_CLE_113
    memcpy(u->cle, body + PAROLES_NOM_LEN, PAROLES_CLE_LEN);
#else
    memset(u->cle, 0, sizeof u->cle);
#endif
    u->udp_port = (uint16_t)(20000u + (u->id % 45000u));
    u->reg_addr = *peer;
    u->reg_addr.sin6_port = htons(u->udp_port);
    unsigned char out[256];
    unsigned char *p = out;
    wire_put_u8(&p, PAROLES_CODEREQ_REG_OK);
    wire_put_u32_be(&p, u->id);
    wire_put_u16_be(&p, u->udp_port);
#ifdef PAROLES_ACCEPT_REAL_CLE_113
    memcpy(p, u->cle, PAROLES_CLE_LEN);
    p += PAROLES_CLE_LEN;
#else
    wire_put_zeros(&p, PAROLES_CLE_LEN);
#endif
    vlog("reg user %u nom=%.10s udp=%u\n", u->id, u->nom, u->udp_port);
    return conn_writen(g_io_ssl, fd, out, (size_t)(p - out));
}
