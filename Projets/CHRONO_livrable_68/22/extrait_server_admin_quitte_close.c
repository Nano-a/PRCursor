/* Extrait — PRCursor/src/server.c — admin quitte (AN=2) → close_group */

    if (an == 2) {
        if (!group_is_member(g, uid)) return -1;
        if (g->admin_id == uid) {
            close_group(g);
            send_ack(fd);
            return 0;
        }
        group_remove_member(g, uid);
        send_ack(fd);
        notif_mcast(g, PAROLES_NOTIF_LEAVE);
        return 0;
    }
