/* PRCursor src/client.c — même connexion : après auth OK, requête métier sans reconnect
 * Lignes source : ~175–187 (suite de one_cmd_authed)
 */

    write_nonce(port, uid, num + 1u);

    if (conn_writen(g_io_ssl, fd, msg, len) < 0) goto fail;
    int n = conn_read_upto(g_io_ssl, fd, resp, rmax, PAROLES_TCP_TIMEOUT_MS);
    g_io_ssl = NULL;
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(fd);
    if (n <= 0) return -1;
    if (verbose) fprintf(stderr, "reponse %d octets, code=%u\n", n, resp[0]);
    return n;
