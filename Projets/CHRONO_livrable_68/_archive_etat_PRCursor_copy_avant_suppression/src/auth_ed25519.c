/* CHRONO N°58 — implémentation retirée (extrait_auth_ed25519.c) — livrable/58/
 * CHRONO N°62 — sign/verify EVP (extrait_auth_ed25519_sign_verify.c) couverts par le stub N°58 */
#include "../include/auth_ed25519.h"

EVP_PKEY *paroles_load_ed25519_private_pem(const char *path) {
    (void)path;
    return NULL;
}

EVP_PKEY *paroles_load_ed25519_public_pem(const char *path) {
    (void)path;
    return NULL;
}

EVP_PKEY *paroles_ed25519_pubkey_from_cle(const uint8_t *cle113) {
    (void)cle113;
    return NULL;
}

int paroles_ed25519_sign(EVP_PKEY *sk, const unsigned char *msg, size_t msglen, unsigned char *sig,
                         size_t *siglen) {
    (void)sk;
    (void)msg;
    (void)msglen;
    (void)sig;
    (void)siglen;
    return -1;
}

int paroles_ed25519_verify(EVP_PKEY *pk, const unsigned char *msg, size_t msglen, const unsigned char *sig,
                           size_t siglen) {
    (void)pk;
    (void)msg;
    (void)msglen;
    (void)sig;
    (void)siglen;
    return -1;
}
