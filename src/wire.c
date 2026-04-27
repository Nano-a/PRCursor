#include "wire.h"

#include <arpa/inet.h>
#include <string.h>

void wire_put16(uint8_t *p, uint16_t v) {
    uint16_t be = htons(v);
    memcpy(p, &be, 2);
}

uint16_t wire_get16(const uint8_t *p) {
    uint16_t be;
    memcpy(&be, p, 2);
    return ntohs(be);
}

void wire_pad_nom(uint8_t out[PAROLES_NOM_LEN], const char *s) {
    memset(out, 0, PAROLES_NOM_LEN);
    if (!s)
        return;
    size_t n = strnlen(s, PAROLES_NOM_LEN);
    memcpy(out, s, n);
}
