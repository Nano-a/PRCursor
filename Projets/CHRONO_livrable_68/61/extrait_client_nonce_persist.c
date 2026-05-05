/* PRCursor src/client.c — NUM côté client : 0 par défaut, puis fichier après chaque auth
 * Lignes : ~31–52, et après vérif AUTH_OK ~175
 */

static void nonce_path(char *out, size_t cap, uint16_t port, uint32_t uid) {
    snprintf(out, cap, "/tmp/paroles_nonce_%u_%u", (unsigned)port, (unsigned)uid);
}

static uint32_t read_nonce(uint16_t port, uint32_t uid) {
    char path[128];
    nonce_path(path, sizeof path, port, uid);
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    unsigned long v = 0;
    if (fscanf(f, "%lu", &v) != 1) v = 0;
    fclose(f);
    return (uint32_t)v;
}

static void write_nonce(uint16_t port, uint32_t uid, uint32_t n) {
    char path[128];
    nonce_path(path, sizeof path, port, uid);
    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "%u", n);
    fclose(f);
}

/* Dans one_cmd_authed, après vérification de la signature serveur (AUTH_OK) : */
    write_nonce(port, uid, num + 1u);
