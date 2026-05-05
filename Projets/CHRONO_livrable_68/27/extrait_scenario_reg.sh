#!/usr/bin/env bash
# Extrait — PRCursor/tests/smoke.sh — inscription (adapter H= et P=)
set -euo pipefail
H=::1
P="${PORT:-4242}"
./paroles_client "$H" "$P" reg Alice | tee /tmp/pc_u1.txt
./paroles_client "$H" "$P" reg Bob | tee /tmp/pc_u2.txt
# Vérifier : grep OK id= dans les sorties
