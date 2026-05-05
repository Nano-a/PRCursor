#!/usr/bin/env bash
# Scénario type regression_codereq : refus (0), acceptation (1), quitter (2)
# Usage : depuis la racine du dépôt avec paroles_client/serveur, serveur déjà lancé sur P
set -euo pipefail
H=::1
P="${PORT:-4245}"
./paroles_client "$H" "$P" reg Alice | tee /tmp/t31_u1.txt
./paroles_client "$H" "$P" reg Alice | tee /tmp/t31_u2.txt
./paroles_client "$H" "$P" reg Bob   | tee /tmp/t31_u3.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/t31_u1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/t31_u2.txt | head -1)
U3=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/t31_u3.txt | head -1)
./paroles_client "$H" "$P" newgroup "$U1" G31 | tee /tmp/t31_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/t31_g.txt | head -1)
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2" "$U3"
./paroles_client "$H" "$P" ans "$U2" "$IDG" 0
./paroles_client "$H" "$P" ans "$U3" "$IDG" 1
./paroles_client "$H" "$P" ans "$U3" "$IDG" 2
