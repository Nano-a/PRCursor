#ifndef PAROLES_WIRE_H
#define PAROLES_WIRE_H

#include <stdint.h>

#include "../include/paroles_proto.h"

void wire_put16(uint8_t *p, uint16_t v);
uint16_t wire_get16(const uint8_t *p);

void wire_pad_nom(uint8_t out[PAROLES_NOM_LEN], const char *s);

#endif
