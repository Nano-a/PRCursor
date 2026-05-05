/* Extrait — PRCursor/src/server.c — handle_new_group : admin + multicast */

    g->admin_id = uid;
    char addrbuf[64];
    snprintf(addrbuf, sizeof addrbuf, "ff0e::1:%u", g->idg);
    inet_pton(AF_INET6, addrbuf, &g->mcast_ip);
    g->mcast_port = (uint16_t)(30000u + (g->idg % 30000u));
    group_add_member(g, uid);
