/* Alignement PR6 / cours : signatures ED25519 via EVP_DigestSignInit(NULL) — message brut. */
#include "../include/auth_ed25519.h"
#include <openssl/pem.h>
#include <stdio.h>

int paroles_ed25519_sign(EVP_PKEY *sk, const unsigned char *msg, size_t msglen,
                         unsigned char *sig, size_t *siglen) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;
    if (EVP_DigestSignInit(ctx, NULL, NULL, NULL, sk) <= 0) {
        EVP_MD_CTX_free(ctx);
        return -1;
    }
    if (EVP_DigestSign(ctx, sig, siglen, msg, msglen) <= 0) {
        EVP_MD_CTX_free(ctx);
        return -1;
    }
    EVP_MD_CTX_free(ctx);
    return 0;
}

int paroles_ed25519_verify(EVP_PKEY *pk, const unsigned char *msg, size_t msglen,
                           const unsigned char *sig, size_t siglen) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;
    if (EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pk) <= 0) {
        EVP_MD_CTX_free(ctx);
        return -1;
    }
    int ok = EVP_DigestVerify(ctx, sig, siglen, msg, msglen);
    EVP_MD_CTX_free(ctx);
    return ok == 1 ? 0 : -1;
}
