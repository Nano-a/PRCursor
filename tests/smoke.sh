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
./paroles_client "$H" "$P" reg Alice | tee /tmp/pc_u1.txt
./paroles_client "$H" "$P" reg Bob | tee /tmp/pc_u2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/pc_u1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/pc_u2.txt | head -1)
./paroles_client "$H" "$P" newgroup "$U1" MonGroupe | tee /tmp/pc_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/pc_g.txt | head -1)
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
./paroles_client "$H" "$P" listinv "$U2" | grep -q idg=
./paroles_client "$H" "$P" ans "$U2" "$IDG" 1
./paroles_client "$H" "$P" post "$U1" "$IDG" "hello"
./paroles_client "$H" "$P" reply "$U2" "$IDG" 0 "reponse"
./paroles_client "$H" "$P" feed "$U2" "$IDG" 0 0 | grep -q billet=
echo "smoke OK (u1=$U1 u2=$U2 g=$IDG)"
