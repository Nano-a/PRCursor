/* Extrait — PRCursor/src/server.c — handle_reg : port UDP + copie adresse client (peer) */

    u->udp_port = (uint16_t)(20000u + (u->id % 45000u));
    u->reg_addr = *peer;
    u->reg_addr.sin6_port = htons(u->udp_port);
