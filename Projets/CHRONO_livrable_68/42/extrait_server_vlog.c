/* Extrait — PRCursor/src/server.c — mode verbeux serveur */

static int verbose;
static SSL *g_io_ssl;

static void vlog(const char *fmt, ...) {
    if (!verbose) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

/* Exemples d’usage dans les handlers : */
    vlog("reg user %u nom=%.10s udp=%u\n", u->id, u->nom, u->udp_port);
    vlog("group %u '%s' admin=%u\n", g->idg, g->name, uid);
    vlog("user %u joined group %u\n", uid, idg);

/* Dans main : */
    verbose = (argc >= 2 && strcmp(argv[1], "-v") == 0);
    int pi = 1;
    if (verbose) pi++;
