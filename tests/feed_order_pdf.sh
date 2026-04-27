#!/usr/bin/env bash
# CHRONO N°48 — ordre fil 16/17 = ordre de réception global (exemple sujet / PDF)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
PORT="${PORT:-4246}"
make -s
./paroles_server ::1 "$PORT" &
PID=$!
sleep 0.35
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT

H=::1
P=$PORT
fail() { echo "ECHEC N°48: $*" >&2; exit 1; }

./paroles_client "$H" "$P" reg A >/tmp/fo_u1.txt
./paroles_client "$H" "$P" reg B >/tmp/fo_u2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/fo_u1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/fo_u2.txt | head -1)
[[ -n "$U1" && -n "$U2" ]] || fail "reg"

./paroles_client "$H" "$P" newgroup "$U1" Gfo >/tmp/fo_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/fo_g.txt | head -1)
[[ -n "$IDG" ]] || fail "idg"

./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
./paroles_client "$H" "$P" ans "$U2" "$IDG" 1

# Séquence globale : billet 0, billet 1, réponse au 0, réponse au 1 (ordre PDF-type)
./paroles_client "$H" "$P" post "$U1" "$IDG" b0
./paroles_client "$H" "$P" post "$U1" "$IDG" b1
./paroles_client "$H" "$P" reply "$U2" "$IDG" 0 r-to-0
./paroles_client "$H" "$P" reply "$U1" "$IDG" 1 r-to-1

OUT=$(./paroles_client "$H" "$P" feed "$U2" "$IDG" 0 0)
# Après l’ancre (0,0) : 2e billet, puis réponses dans l’ordre d’arrivée dans le fil
mapfile -t L < <(echo "$OUT" | sed -n 's/.*billet=\([0-9]*\) rep=\([0-9]*\).*/\1 \2/p')
[[ ${#L[@]} -ge 3 ]] || fail "pas assez d’items feed"
[[ "${L[0]}" == "1 0" ]] || fail "1er après ancre : billet 1 rep 0 (2e billet), obtenu «${L[0]}»"
[[ "${L[1]}" == "0 1" ]] || fail "2e item : billet 0 rep 1, obtenu «${L[1]}»"
[[ "${L[2]}" == "1 1" ]] || fail "3e item : billet 1 rep 1, obtenu «${L[2]}»"

echo "feed_order_pdf OK (N°48)"
