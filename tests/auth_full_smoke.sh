#!/usr/bin/env bash
# CHRONO N°59–63 : TLS + CODEREQ 0 (ED25519) + requête métier sur la même session TCP
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
PORT="${PORT:-4251}"
make -s
chmod +x scripts/gencerts.sh scripts/gen_ed25519.sh
scripts/gencerts.sh "$ROOT/tests/fixtures/certs"
scripts/gen_ed25519.sh "$ROOT/tests/fixtures/ed25519_u"
scripts/gen_ed25519.sh "$ROOT/tests/fixtures/ed25519_srv"
CERT="$ROOT/tests/fixtures/certs"
UU="$ROOT/tests/fixtures/ed25519_u"
SRV="$ROOT/tests/fixtures/ed25519_srv"
rm -f "/tmp/paroles_nonce_${PORT}_1"
./paroles_server --tls "$CERT/server.pem" "$CERT/server.key" --signing-key "$SRV/key.pem" ::1 "$PORT" &
PID=$!
sleep 0.45
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT
./paroles_client --tls "$CERT/ca.pem" ::1 "$PORT" reg AuthFull "$UU/pub.pem" | tee /tmp/af_reg.txt
grep -q "OK id=1" /tmp/af_reg.txt || { echo "ECHEC auth_full: reg" >&2; exit 1; }
./paroles_client --tls "$CERT/ca.pem" --key "$UU/key.pem" --server-pub "$SRV/pub.pem" ::1 "$PORT" newgroup 1 Gauth | tee /tmp/af_ng.txt
grep -q "OK idg=" /tmp/af_ng.txt || { echo "ECHEC auth_full: newgroup" >&2; exit 1; }
echo "auth_full_smoke OK (port $PORT)"
