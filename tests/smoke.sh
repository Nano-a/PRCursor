#!/usr/bin/env bash
set -euo pipefail
PORT="${PORT:-4243}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
./paroles_server ::1 "$PORT" &
PID=$!
cleanup() { kill "$PID" 2>/dev/null || true; }
trap cleanup EXIT
sleep 0.15
./paroles_smoke ::1 "$PORT"
