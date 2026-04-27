#!/usr/bin/env bash
# CHRONO N°49 — notifs 18–23 (mcast/UDP) + réponse erreur 31 sur requête invalide
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
PORT="${PORT:-4247}"
fail() { echo "ECHEC N°49: $*" >&2; exit 1; }
command -v python3 >/dev/null || fail "python3 requis pour N°49"
make -s
./paroles_server ::1 "$PORT" &
PID=$!
sleep 0.35
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT

H=::1
P=$PORT

./paroles_client "$H" "$P" reg N1 | tee /tmp/n1.txt
./paroles_client "$H" "$P" reg N2 | tee /tmp/n2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/n1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/n2.txt | head -1)
UDP2=$(sed -n 's/.*udp=\([0-9]*\).*/\1/p' /tmp/n2.txt | head -1)
[[ -n "$U1" && -n "$U2" && -n "$UDP2" ]] || fail "reg / udp"

./paroles_client "$H" "$P" newgroup "$U1" Gn | tee /tmp/ng.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/ng.txt | head -1)
MCAST=$(sed -n 's/.*mcast=\([^ ]*\).*/\1/p' /tmp/ng.txt | head -1)
MPORT=$(sed -n 's/.*port=\([0-9]*\)$/\1/p' /tmp/ng.txt | head -1)
[[ -n "$IDG" && -n "$MCAST" && -n "$MPORT" ]] || fail "newgroup parse"

# 22 : invitation UDP vers N2 (en arrière-plan : écoute sur port UDP inscrit)
python3 "$ROOT/tests/notif_recv.py" udp "$UDP2" 2.5 > /tmp/n22.out &
R22=$!
sleep 0.2
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
wait $R22 || fail "recv 22"
grep -q '^22 ' /tmp/n22.out || fail "code 22 attendu"
grep -q "^22 $IDG\$" /tmp/n22.out || fail "idg dans notif 22"

# Jointure N2 + écoute multicast 18 (nouveau message)
./paroles_client "$H" "$P" ans "$U2" "$IDG" 1
python3 "$ROOT/tests/notif_recv.py" mcast "$MCAST" "$MPORT" 2.5 > /tmp/n18.out &
R18=$!
sleep 0.25
./paroles_client "$H" "$P" post "$U1" "$IDG" hello-mcast
wait $R18 || fail "recv 18"
grep -q '^18 ' /tmp/n18.out || fail "code 18 attendu"
grep -q "^18 $IDG\$" /tmp/n18.out || fail "idg dans notif 18"

# 31 : premier octet TCP = code inconnu → ERR
python3 "$ROOT/tests/notif_recv.py" tcp_bad "$H" "$P" > /tmp/n31.out
grep -q '^31$' /tmp/n31.out || fail "premier octet réponse = 31 (ERR)"

echo "notif_codereq OK (N°49 — échantillon 18, 22, 31)"
