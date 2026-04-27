#include "wire.h"
#include <string.h>

void wire_put_u8(unsigned char **p, uint8_t v) {
    *(*p)++ = v;
}

void wire_put_u16_be(unsigned char **p, uint16_t v) {
    *(*p)++ = (unsigned char)((v >> 8) & 0xff);
    *(*p)++ = (unsigned char)(v & 0xff);
}

void wire_put_u32_be(unsigned char **p, uint32_t v) {
    *(*p)++ = (unsigned char)((v >> 24) & 0xff);
    *(*p)++ = (unsigned char)((v >> 16) & 0xff);
    *(*p)++ = (unsigned char)((v >> 8) & 0xff);
    *(*p)++ = (unsigned char)(v & 0xff);
}

int wire_get_u8(const unsigned char **p, size_t *left, uint8_t *out) {
    if (*left < 1) return -1;
    *out = *(*p)++;
    (*left)--;
    return 0;
}

int wire_get_u16_be(const unsigned char **p, size_t *left, uint16_t *out) {
    if (*left < 2) return -1;
    *out = (uint16_t)((*p)[0] << 8 | (*p)[1]);
    *p += 2;
    *left -= 2;
    return 0;
}

int wire_get_u32_be(const unsigned char **p, size_t *left, uint32_t *out) {
    if (*left < 4) return -1;
    *out = ((uint32_t)(*p)[0] << 24) | ((uint32_t)(*p)[1] << 16) |
           ((uint32_t)(*p)[2] << 8) | (uint32_t)(*p)[3];
    *p += 4;
    *left -= 4;
    return 0;
}

void wire_put_zeros(unsigned char **p, size_t n) {
    memset(*p, 0, n);
    *p += n;
}

int wire_expect_zeros(const unsigned char **p, size_t *left, size_t n) {
    if (*left < n) return -1;
    for (size_t i = 0; i < n; i++)
        if (*(*p)++) return -1;
    *left -= n;
    return 0;
}
