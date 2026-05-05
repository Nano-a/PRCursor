/* Extrait — PRCursor/src/server.c — exemples de gardes menant à erreur (→ send_err côté serve_client) */

/* handle_new_group : utilisateur inexistant */
    User *u = find_user(uid);
    if (!u) return -1;

/* handle_invite : groupe absent ou pas admin */
    Group *g = find_group(idg);
    if (!g || g->admin_id != uid) return -1;
        if (!find_user(invitee)) return -1;

/* handle_list_mem : groupe demandé inconnu ou pas membre */
    Group *g = find_group(idg);
    if (!g) return -1;
    if (!group_is_member(g, uid)) return -1;

/* handle_inv_ans : groupe absent ; pas invité pour 0/1 ; pas membre pour quitte */
    Group *g = find_group(idg);
    if (!g) return -1;
    if (!group_is_pending(g, uid)) return -1; /* pour an 0 ou 1 */

/* dispatch : auth session — UID fil doit matcher la session (étape 3) */
        if (wire_uid != sess_uid) return -1;

/* dispatch : corps LIST_MEM doit être exactement 8 octets (uid+idg) */
            if (left) return -1;
