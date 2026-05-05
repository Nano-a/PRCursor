/* Extrait — PRCursor/src/client.c — mode verbeux client */

static int verbose;

    if (verbose) fprintf(stderr, "reponse %d octets, code=%u\n", n, resp[0]);

/* Dans main : */
    verbose = 0;
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
        verbose = 1;
        i++;
    }
