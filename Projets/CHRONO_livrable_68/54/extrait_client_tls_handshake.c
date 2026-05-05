/* PRCursor src/client.c — extraits TLS à fusionner (ne compile pas seul).
 * Réf. lignes : ~26–29, ~64–82 (one_cmd_plain), même bloc ~114–131 (one_cmd_authed), ~484–491 (main).
 */

static const char *tls_sni_for_host(const char *host) {
    if (!strcmp(host, "::1") || !strcmp(host, "127.0.0.1")) return "localhost";
    return host;
}

/* --- après tcp6_connect réussi dans one_cmd_plain / one_cmd_authed --- */
    SSL *ssl = NULL;
    if (g_tls_cli) {
        ssl = SSL_new(g_tls_cli);
        if (!ssl) {
            close(fd);
            return -1;
        }
        SSL_set_fd(ssl, fd);
        if (SSL_set_tlsext_host_name(ssl, tls_sni_for_host(host)) != 1) {
            SSL_free(ssl);
            close(fd);
            return -1;
        }
        if (SSL_connect(ssl) <= 0) {
            SSL_free(ssl);
            close(fd);
            return -1;
        }
    }
    g_io_ssl = ssl;

/* --- dans main, après -v, avant --key --- */
    if (argc - i >= 2 && strcmp(argv[i], "--tls") == 0) {
        g_tls_cli = paroles_tls_client_ctx(argv[i + 1]);
        if (!g_tls_cli) {
            fprintf(stderr, "paroles_client: TLS (CA) init impossible\n");
            return 1;
        }
        i += 2;
    }
