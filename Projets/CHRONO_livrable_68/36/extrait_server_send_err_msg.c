/* Extrait — PRCursor/src/server.c — erreur 31 avec longueur + message (option debug) */

static void send_err_msg(int fd, const char *msg) {
    if (!msg || !*msg) {
        send_err(fd);
        return;
    }
    size_t ml = strlen(msg);
    if (ml > 200) ml = 200;
    unsigned char buf[512];
    unsigned char *p = buf;
    wire_put_u8(&p, PAROLES_CODEREQ_ERR);
    wire_put_u16_be(&p, (uint16_t)ml);
    memcpy(p, msg, ml);
    p += ml;
    conn_writen(g_io_ssl, fd, buf, (size_t)(p - buf));
}
