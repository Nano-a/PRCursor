#!/usr/bin/env bash
# Extrait — PRCursor/tests/smoke.sh — listinv après invite
set -euo pipefail
H=::1
P="${PORT:-4242}"
./paroles_client "$H" "$P" reg Alice | tee /tmp/pc_u1.txt
./paroles_client "$H" "$P" reg Bob | tee /tmp/pc_u2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/pc_u1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/pc_u2.txt | head -1)
./paroles_client "$H" "$P" newgroup "$U1" MonGroupe | tee /tmp/pc_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/pc_g.txt | head -1)
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
./paroles_client "$H" "$P" listinv "$U2" | grep -q idg=
