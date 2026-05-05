#!/usr/bin/env bash
# CHRONO N°66 — démo soutenance : serveur + client en mode verbeux (étape 1 TCP clair)
# Adapter PORT ; nécessite PRCursor compilé.
set -euo pipefail
LIV="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$LIV/../../../PRCursor" && pwd)"
cd "$ROOT"
PORT="${PORT:-4260}"
make -s
echo "=== Terminal A : ./paroles_server -v ::1 $PORT"
echo "=== Terminal B : ./paroles_client -v ::1 $PORT reg Soutenance"
echo "(Pour TLS/auth, ajouter --tls / --key selon votre démo.)"
