#!/usr/bin/env bash
# feed depuis (0,0) — tout le fil
set -euo pipefail
H=::1
P="${PORT:-4245}"
U3="${U3:?export U3}"
IDG="${IDG:?export IDG}"
OUT=$(./paroles_client "$H" "$P" feed "$U3" "$IDG" 0 0)
echo "$OUT" | grep -q "billet=1"
echo "$OUT" | grep -q "r-b2"
