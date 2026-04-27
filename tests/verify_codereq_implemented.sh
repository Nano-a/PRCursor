#!/usr/bin/env bash
# CHRONO N°47 — vérif statique : CODEREQ 0–17, 24–25, 31 (étape 3 inclut AUTH)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
HDR=include/paroles_proto.h
SRV=src/server.c

fail() { echo "ECHEC N°47: $*" >&2; exit 1; }

grep -qE '^#define PAROLES_CODEREQ_AUTH[[:space:]]+0$' "$HDR" || fail "AUTH 0"
grep -qE '^#define PAROLES_CODEREQ_AUTH_OK[[:space:]]+25$' "$HDR" || fail "AUTH_OK 25"
grep -qE '^#define PAROLES_CODEREQ_REG[[:space:]]+1$' "$HDR" || fail "REG 1"
grep -qE '^#define PAROLES_CODEREQ_REG_OK[[:space:]]+2$' "$HDR" || fail "REG_OK 2"
grep -qE '^#define PAROLES_CODEREQ_NEW_GROUP[[:space:]]+3$' "$HDR" || fail "NEW_GROUP 3"
grep -qE '^#define PAROLES_CODEREQ_NEW_GROUP_OK[[:space:]]+4$' "$HDR" || fail "NEW_GROUP_OK 4"
grep -qE '^#define PAROLES_CODEREQ_INVITE[[:space:]]+5$' "$HDR" || fail "INVITE 5"
grep -qE '^#define PAROLES_CODEREQ_LIST_INV[[:space:]]+6$' "$HDR" || fail "LIST_INV 6"
grep -qE '^#define PAROLES_CODEREQ_LIST_INV_OK[[:space:]]+7$' "$HDR" || fail "LIST_INV_OK 7"
grep -qE '^#define PAROLES_CODEREQ_INV_ANS[[:space:]]+8$' "$HDR" || fail "INV_ANS 8"
grep -qE '^#define PAROLES_CODEREQ_JOIN_OK[[:space:]]+9$' "$HDR" || fail "JOIN_OK 9"
grep -qE '^#define PAROLES_CODEREQ_LIST_MEM[[:space:]]+10$' "$HDR" || fail "LIST_MEM 10"
grep -qE '^#define PAROLES_CODEREQ_LIST_MEM_OK[[:space:]]+11$' "$HDR" || fail "LIST_MEM_OK 11"
grep -qE '^#define PAROLES_CODEREQ_POST[[:space:]]+12$' "$HDR" || fail "POST 12"
grep -qE '^#define PAROLES_CODEREQ_POST_OK[[:space:]]+13$' "$HDR" || fail "POST_OK 13"
grep -qE '^#define PAROLES_CODEREQ_REPLY[[:space:]]+14$' "$HDR" || fail "REPLY 14"
grep -qE '^#define PAROLES_CODEREQ_REPLY_OK[[:space:]]+15$' "$HDR" || fail "REPLY_OK 15"
grep -qE '^#define PAROLES_CODEREQ_FEED[[:space:]]+16$' "$HDR" || fail "FEED 16"
grep -qE '^#define PAROLES_CODEREQ_FEED_OK[[:space:]]+17$' "$HDR" || fail "FEED_OK 17"
grep -qE '^#define PAROLES_CODEREQ_ACK[[:space:]]+24$' "$HDR" || fail "ACK 24"
grep -qE '^#define PAROLES_CODEREQ_ERR[[:space:]]+31$' "$HDR" || fail "ERR 31"

while read -r a name val _; do
  [[ "$a" == "#define" && "$name" == PAROLES_CODEREQ_* ]] || continue
  [[ "$name" == "PAROLES_CODEREQ_AUTH" ]] && continue
  [[ "$val" == "0" ]] && fail "CODEREQ $name = 0 inattendu (seul AUTH peut être 0)"
done < "$HDR"

for sym in REG NEW_GROUP INVITE LIST_INV INV_ANS LIST_MEM POST REPLY FEED; do
  grep -q "case PAROLES_CODEREQ_${sym}:" "$SRV" || fail "serve_business_switch gère $sym"
done

grep -q "do_client_auth" "$SRV" || fail "serveur gère handshake AUTH (do_client_auth)"

echo "verify_codereq_implemented OK (N°47)"
