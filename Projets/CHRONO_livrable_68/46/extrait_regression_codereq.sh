#!/usr/bin/env bash
# CHRONO N°46 — scénarios CODEREQ / non-régression (adapté de PRCursor/tests/regression_codereq.sh)
set -euo pipefail
LIV="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$LIV/../../../PRCursor" && pwd)"
cd "$ROOT"
PORT="${PORT:-4245}"
make -s
./paroles_server ::1 "$PORT" &
PID=$!
sleep 0.35
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT

H=::1
P=$PORT

fail() { echo "ECHEC: $*" >&2; exit 1; }

echo "== reg ×3 (même pseudo Alice deux fois → IDs distincts) =="
./paroles_client "$H" "$P" reg Alice | tee /tmp/rc_u1.txt
./paroles_client "$H" "$P" reg Alice | tee /tmp/rc_u2.txt
./paroles_client "$H" "$P" reg Bob   | tee /tmp/rc_u3.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/rc_u1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/rc_u2.txt | head -1)
U3=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/rc_u3.txt | head -1)
[[ -n "$U1" && -n "$U2" && -n "$U3" ]] || fail "ids inscription"
[[ "$U1" != "$U2" ]] || fail "deux Alice doivent avoir des IDs distincts"

echo "== newgroup + invite U2 U3 =="
./paroles_client "$H" "$P" newgroup "$U1" GReg | tee /tmp/rc_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/rc_g.txt | head -1)
[[ -n "$IDG" ]] || fail "idg groupe"
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2" "$U3"

echo "== listinv U2 / U3 =="
./paroles_client "$H" "$P" listinv "$U2" | grep -q "idg=$IDG" || fail "listinv U2"
./paroles_client "$H" "$P" listinv "$U3" | grep -q "idg=$IDG" || fail "listinv U3"

echo "== refus invitation U2 (ans 0) =="
./paroles_client "$H" "$P" ans "$U2" "$IDG" 0

echo "== acceptation U3 (ans 1) =="
./paroles_client "$H" "$P" ans "$U3" "$IDG" 1

echo "== listmem idg=0 (tous inscrits) =="
./paroles_client "$H" "$P" listmem "$U1" 0 | grep -q "membres" || fail "listmem 0"

echo "== listmem groupe (admin en tête) =="
./paroles_client "$H" "$P" listmem "$U1" "$IDG" | grep -q "$U1" || fail "listmem groupe"

echo "== post / reply / feed (ordre fil) =="
./paroles_client "$H" "$P" post "$U1" "$IDG" "b1"
./paroles_client "$H" "$P" post "$U1" "$IDG" "b2"
./paroles_client "$H" "$P" reply "$U3" "$IDG" 1 "r-b2"
OUT=$(./paroles_client "$H" "$P" feed "$U3" "$IDG" 0 0)
echo "$OUT" | grep -q "billet=1" || fail "feed attend 2e billet (numb=1)"
echo "$OUT" | grep -q "r-b2" || fail "feed attend la réponse r-b2"

echo "== erreur métier : newgroup avec uid inconnu =="
if ./paroles_client "$H" "$P" newgroup 99999 Solo 2>/tmp/rc_err.txt; then
  fail "newgroup 99999 devrait échouer"
fi

echo "== quitter groupe (U3, ans 2) =="
./paroles_client "$H" "$P" ans "$U3" "$IDG" 2

echo "regression_codereq OK (port $PORT)"
