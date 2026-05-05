/*
 * CHRONO N°39 — extrait depuis PRCursor/src/client.c
 * Réception notifications : même format 6 octets BE que tests/notif_recv.py
 *
 * Dépend du projet complet : net.h (udp6_bind_any, udp6_mcast_recv_socket, udp6_recv).
 */

static void write_notif_line(const unsigned char *buf, int n) {
    if (n < 6)
        return;
    uint16_t code = (uint16_t)(((uint16_t)buf[0] << 8) | buf[1]);
    uint32_t idg =
        ((uint32_t)buf[2] << 24) | ((uint32_t)buf[3] << 16) | ((uint32_t)buf[4] << 8) | (uint32_t)buf[5];
    printf("%u %u\n", (unsigned)code, (unsigned)idg);
}

static int cmd_listen_udp(uint16_t udp_port, int duration_sec) {
    if (duration_sec <= 0) duration_sec = 10;
    int fd = udp6_bind_any(udp_port);
    if (fd < 0) {
        perror("listen_udp");
        return -1;
    }
    time_t deadline = time(NULL) + duration_sec;
    while (time(NULL) < deadline) {
        unsigned char buf[512];
        struct sockaddr_in6 from;
        int left_ms = 1000;
        time_t left = deadline - time(NULL);
        if ((time_t)left_ms > left * 1000) left_ms = (int)(left * 1000);
        int n = udp6_recv(fd, buf, sizeof buf, &from, left_ms > 0 ? left_ms : 1);
        if (n > 0) write_notif_line(buf, n);
    }
    close(fd);
    return 0;
}

static int cmd_listen_mcast(const char *mcast_ipv6, uint16_t mcast_port, int duration_sec) {
    if (duration_sec <= 0) duration_sec = 10;
    int fd = udp6_mcast_recv_socket(mcast_ipv6, mcast_port);
    if (fd < 0) {
        perror("listen_mcast");
        return -1;
    }
    time_t deadline = time(NULL) + duration_sec;
    while (time(NULL) < deadline) {
        unsigned char buf[512];
        struct sockaddr_in6 from;
        int left_ms = 1000;
        time_t left = deadline - time(NULL);
        if ((time_t)left_ms > left * 1000) left_ms = (int)(left * 1000);
        int n = udp6_recv(fd, buf, sizeof buf, &from, left_ms > 0 ? left_ms : 1);
        if (n > 0) write_notif_line(buf, n);
    }
    close(fd);
    return 0;
}

/* Dans main : branches listen_udp / listen_mcast après feed — voir PRCursor/src/client.c lignes ~628–645 */
