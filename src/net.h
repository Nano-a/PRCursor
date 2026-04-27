#ifndef PAROLES_NET_H
#define PAROLES_NET_H

#include <stddef.h>

/* Lit exactement n octets ou -1 (erreur/EOF), -2 (timeout). */
int net_read_all(int fd, void *buf, size_t n, int timeout_ms);

/* Écrit tout le buffer. Retourne 0 ou -1. */
int net_write_all(int fd, const void *buf, size_t n);

#endif
