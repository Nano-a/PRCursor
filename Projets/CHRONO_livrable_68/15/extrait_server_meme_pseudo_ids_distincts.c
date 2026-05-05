/* Extrait — PRCursor/src/server.c — handle_reg : nouveau slot, ID unique global, nom copié tel quel */

    User *u = &users[slot];
    memset(u, 0, sizeof *u);
    u->used = 1;
    u->id = next_uid++;
    memcpy(u->nom, body, PAROLES_NOM_LEN);
