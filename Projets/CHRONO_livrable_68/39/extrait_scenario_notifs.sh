#!/usr/bin/env bash
# Réception notif 22 (UDP) puis 18 (multicast) — adapté de PRCursor/tests/notif_codereq.sh
set -euo pipefail
LIV="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$LIV/../../../PRCursor" && pwd)"
PY="$LIV/extrait_notif_recv.py"
PORT="${PORT:-4247}"
command -v python3 >/dev/null || { echo "python3 requis" >&2; exit 1; }
cd "$ROOT"
make -s
./paroles_server ::1 "$PORT" &
PID=$!
sleep 0.35
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT
H=::1
P=$PORT
./paroles_client "$H" "$P" reg N1 | tee /tmp/n39_1.txt
./paroles_client "$H" "$P" reg N2 | tee /tmp/n39_2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/n39_1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/n39_2.txt | head -1)
UDP2=$(sed -n 's/.*udp=\([0-9]*\).*/\1/p' /tmp/n39_2.txt | head -1)
./paroles_client "$H" "$P" newgroup "$U1" Gn | tee /tmp/n39_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/n39_g.txt | head -1)
MCAST=$(sed -n 's/.*mcast=\([^ ]*\).*/\1/p' /tmp/n39_g.txt | head -1)
MPORT=$(sed -n 's/.*port=\([0-9]*\)$/\1/p' /tmp/n39_g.txt | head -1)
python3 "$PY" udp "$UDP2" 2.5 > /tmp/n39_22.out &
R22=$!
sleep 0.2
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
wait $R22
grep -q '^22 ' /tmp/n39_22.out && grep -q "^22 $IDG\$" /tmp/n39_22.out
./paroles_client "$H" "$P" ans "$U2" "$IDG" 1
python3 "$PY" mcast "$MCAST" "$MPORT" 2.5 > /tmp/n39_18.out &
R18=$!
sleep 0.25
./paroles_client "$H" "$P" post "$U1" "$IDG" hello-mcast
wait $R18
grep -q '^18 ' /tmp/n39_18.out && grep -q "^18 $IDG\$" /tmp/n39_18.out
echo "scenario notifs OK (22 udp, 18 mcast)"
