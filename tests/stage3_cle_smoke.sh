#!/usr/bin/env bash
# CHRONO N°58 — inscription avec clé publique ED25519 PEM (32 octets + zéros → 113)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
PORT="${PORT:-4249}"
make -s
chmod +x scripts/gen_ed25519.sh
scripts/gen_ed25519.sh "$ROOT/tests/fixtures/ed25519"
PEM="$ROOT/tests/fixtures/ed25519/pub.pem"
./paroles_server ::1 "$PORT" &
PID=$!
sleep 0.35
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT
./paroles_client ::1 "$PORT" reg Stage3User "$PEM" | tee /tmp/s3cle.txt
grep -q "OK id=" /tmp/s3cle.txt || { echo "ECHEC stage3_cle_smoke" >&2; exit 1; }
echo "stage3_cle_smoke OK (port $PORT)"
