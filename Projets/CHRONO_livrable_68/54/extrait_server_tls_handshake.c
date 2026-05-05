/* Extrait PRCursor src/server.c — TLS : SSL_accept + main --tls
 * Fusionner dans server.c (dépend : tls_io.h, g_io_ssl global, serve_business_switch, etc.)
 * Lignes source : ~816–931
 */

static void serve_client(int cfd, struct sockaddr_in6 *peer, SSL_CTX *tls_ctx) {
    SSL *ssl = NULL;
    if (tls_ctx) {
        ssl = SSL_new(tls_ctx);
        if (!ssl) {
            close(cfd);
            return;
        }
        SSL_set_fd(ssl, cfd);
        if (SSL_accept(ssl) <= 0) {
            SSL_free(ssl);
            close(cfd);
            return;
        }
    }
    g_io_ssl = ssl;

    unsigned char hdr[1];
    unsigned char buf[256 * 1024];
    if (conn_readn(g_io_ssl, cfd, hdr, 1, PAROLES_TCP_TIMEOUT_MS) < 0) goto end;
    uint8_t code = hdr[0];
    uint32_t sess_uid = 0;

    if (code == PAROLES_CODEREQ_REG) {
        if (conn_readn(g_io_ssl, cfd, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, PAROLES_TCP_TIMEOUT_MS) < 0)
            goto bad;
        if (dispatch(cfd, peer, code, buf, PAROLES_NOM_LEN + PAROLES_CLE_LEN, 0) < 0) goto err;
        goto end;
    }

    if (g_srv_sign_key) {
        if (code != PAROLES_CODEREQ_AUTH) goto err;
        uint32_t aid, anum;
        if (do_client_auth(cfd, &aid, &anum) < 0) goto err;
        if (send_auth_ok(cfd, aid, anum) < 0) goto err;
        sess_uid = aid;
        if (conn_readn(g_io_ssl, cfd, hdr, 1, PAROLES_TCP_TIMEOUT_MS) < 0) goto end;
        code = hdr[0];
        if (code == PAROLES_CODEREQ_REG) goto err;
    }

    if (serve_business_switch(cfd, peer, code, sess_uid) < 0) goto err;
    goto end;
bad:
    goto end;
err:
    if (verbose) send_err_msg(cfd, "erreur requete");
    else
        send_err(cfd);
    goto end;
end:
    g_io_ssl = NULL;
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(cfd);
}

int main(int argc, char **argv) {
    verbose = (argc >= 2 && strcmp(argv[1], "-v") == 0);
    int pi = 1;
    if (verbose) pi++;
    const char *tls_cert = NULL, *tls_key = NULL;
    SSL_CTX *tls_ctx = NULL;
    if (argc - pi >= 3 && strcmp(argv[pi], "--tls") == 0) {
        tls_cert = argv[pi + 1];
        tls_key = argv[pi + 2];
        pi += 3;
        tls_ctx = paroles_tls_server_ctx(tls_cert, tls_key);
        if (!tls_ctx) {
            fprintf(stderr, "paroles_server: TLS init impossible (cert/key)\n");
            return 1;
        }
    }
    if (argc - pi >= 2 && strcmp(argv[pi], "--signing-key") == 0) {
        g_srv_sign_key = paroles_load_ed25519_private_pem(argv[pi + 1]);
        if (!g_srv_sign_key) {
            fprintf(stderr, "paroles_server: clé ED25519 serveur (--signing-key) invalide\n");
            paroles_tls_ctx_free(tls_ctx);
            return 1;
        }
        pi += 2;
    }
    if (argc - pi < 2) {
        fprintf(stderr,
                "usage: paroles_server [-v] [--tls cert.pem key.pem] [--signing-key priv.pem] "
                "bind_ipv6 port\n");
        EVP_PKEY_free(g_srv_sign_key);
        paroles_tls_ctx_free(tls_ctx);
        return 1;
    }
    const char *host = argv[pi];
    uint16_t port = (uint16_t)atoi(argv[pi + 1]);
    if (strcmp(host, "-v") == 0) {
        fprintf(stderr, "usage error\n");
        EVP_PKEY_free(g_srv_sign_key);
        paroles_tls_ctx_free(tls_ctx);
        return 1;
    }
    int srv = tcp6_listen(host, port);
    if (srv < 0) {
        perror("listen");
        EVP_PKEY_free(g_srv_sign_key);
        paroles_tls_ctx_free(tls_ctx);
        return 1;
    }
    fprintf(stderr, "paroles_server ecoute [%s]:%u%s%s\n", host, (unsigned)port,
            tls_ctx ? " (TLS)" : "", g_srv_sign_key ? " +auth" : "");
    for (;;) {
        struct sockaddr_in6 peer;
        socklen_t pl = sizeof peer;
        int c = tcp6_accept(srv, &peer, &pl);
        if (c < 0) continue;
        serve_client(c, &peer, tls_ctx);
    }
}
