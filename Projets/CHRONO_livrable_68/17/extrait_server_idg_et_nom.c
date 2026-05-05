/* Extrait — PRCursor/src/server.c — IDG strictement croissant, nom non unique */

static uint32_t next_gid = 1;

/* Dans handle_new_group, après memset(g) et g->used = 1 : */
    g->idg = next_gid++;
    g->name = malloc(len + 1);
    if (!g->name) return -1;
    memcpy(g->name, q, len);
    g->name[len] = 0;
