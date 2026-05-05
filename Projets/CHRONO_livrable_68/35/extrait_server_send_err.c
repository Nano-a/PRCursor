/* Extrait — PRCursor/src/server.c — erreur 31 + ZEROS */

static void send_err(int fd) {
    unsigned char b[1 + PAROLES_ERR_TAIL];
    memset(b, 0, sizeof b);
    b[0] = PAROLES_CODEREQ_ERR;
    conn_writen(g_io_ssl, fd, b, sizeof b);
}
