#ifndef AUTH_ED25519_H
#define AUTH_ED25519_H

#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>

EVP_PKEY *paroles_load_ed25519_private_pem(const char *path);
EVP_PKEY *paroles_load_ed25519_public_pem(const char *path);
/* cle[0..31] = clé publique brute ED25519 (reste ignoré). */
EVP_PKEY *paroles_ed25519_pubkey_from_cle(const uint8_t *cle113);

int paroles_ed25519_sign(EVP_PKEY *sk, const unsigned char *msg, size_t msglen,
                         unsigned char *sig, size_t *siglen);
int paroles_ed25519_verify(EVP_PKEY *pk, const unsigned char *msg, size_t msglen,
                           const unsigned char *sig, size_t siglen);

#endif
