#!/usr/bin/env bash
# CHRONO N°53–54 — smoke TLS (OpenSSL) — adapté de PRCursor/tests/tls_smoke.sh
set -euo pipefail
LIV="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$LIV/../../../PRCursor" && pwd)"
cd "$ROOT"
PORT="${PORT:-4248}"
make -s
chmod +x scripts/gencerts.sh
scripts/gencerts.sh "$ROOT/tests/fixtures/certs"
CERTDIR="$ROOT/tests/fixtures/certs"
./paroles_server --tls "$CERTDIR/server.pem" "$CERTDIR/server.key" ::1 "$PORT" &
PID=$!
sleep 0.45
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT
./paroles_client --tls "$CERTDIR/ca.pem" ::1 "$PORT" reg TlsUser | tee /tmp/tls_reg.txt
grep -q "OK id=" /tmp/tls_reg.txt || { echo "ECHEC tls_smoke: inscription TLS" >&2; exit 1; }
echo "tls_smoke OK (port $PORT)"
