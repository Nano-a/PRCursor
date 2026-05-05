#!/usr/bin/env bash
# regression_codereq : newgroup avec UID inconnu → échec client
set -euo pipefail
H=::1
P="${PORT:-4245}"
if ./paroles_client "$H" "$P" newgroup 99999 Solo 2>/tmp/t34_err.txt; then
  echo "ECHEC: newgroup 99999 aurait dû échouer" >&2
  exit 1
fi
echo "OK rejet attendu"
