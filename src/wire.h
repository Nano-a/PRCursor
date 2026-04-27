#ifndef WIRE_H
#define WIRE_H

#include <stddef.h>
#include <stdint.h>

void wire_put_u8(unsigned char **p, uint8_t v);
void wire_put_u16_be(unsigned char **p, uint16_t v);
void wire_put_u32_be(unsigned char **p, uint32_t v);
int wire_get_u8(const unsigned char **p, size_t *left, uint8_t *out);
int wire_get_u16_be(const unsigned char **p, size_t *left, uint16_t *out);
int wire_get_u32_be(const unsigned char **p, size_t *left, uint32_t *out);

void wire_put_zeros(unsigned char **p, size_t n);
int wire_expect_zeros(const unsigned char **p, size_t *left, size_t n);

#endif
