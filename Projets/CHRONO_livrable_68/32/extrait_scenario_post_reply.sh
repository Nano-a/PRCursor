#!/usr/bin/env bash
# post + reply (membre doit être dans le groupe)
set -euo pipefail
H=::1
P="${PORT:-4245}"
# Prérequis : U1 admin, U3 membre, IDG (ex. scénario tâche 31 jusqu'à ans 1 seulement)
U1="${U1:?export U1}"
U3="${U3:?export U3}"
IDG="${IDG:?export IDG}"
./paroles_client "$H" "$P" post "$U1" "$IDG" "b1"
./paroles_client "$H" "$P" post "$U1" "$IDG" "b2"
./paroles_client "$H" "$P" reply "$U3" "$IDG" 1 "r-b2"
