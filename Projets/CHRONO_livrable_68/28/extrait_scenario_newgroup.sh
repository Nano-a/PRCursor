#!/usr/bin/env bash
# Extrait — logique smoke : newgroup après deux reg (U1 = id du premier)
set -euo pipefail
H=::1
P="${PORT:-4242}"
./paroles_client "$H" "$P" reg Alice | tee /tmp/pc_u1.txt
./paroles_client "$H" "$P" reg Bob | tee /tmp/pc_u2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/pc_u1.txt | head -1)
./paroles_client "$H" "$P" newgroup "$U1" MonGroupe | tee /tmp/pc_g.txt
# IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/pc_g.txt | head -1)
