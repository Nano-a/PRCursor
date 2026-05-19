#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
PORT="${PORT:-4242}"
./paroles_server ::1 "$PORT" &
PID=$!
sleep 0.3
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT

H=::1
P=$PORT
# CHRONO 27–30 : scénario reg → listinv retiré (extraits_scenario_reg/newgroup/invite/listinv.sh) — livrable 27 à 30
# La suite du smoke (ans, post, reply, feed) dépendait de U1/U2/IDG ci-dessus ; réintégrer depuis le livrable.
echo "smoke.sh: arrêt après retrait CHRONO 27–30 — réintégrer le bloc reg…listinv puis la suite." >&2
exit 1
