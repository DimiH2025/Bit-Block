#!/bin/bash
set -euo pipefail

# Bit-block local run script
#
# Plain, portable entry point for building/running Bit-block locally.
# It runs two steps in sequence:
#   1. scripts/smoke_bit-block.sh  (download/verify + smoke test)
#   2. scripts/secure_startup.sh   (hardened config + start bitcoind)
#
# secure_startup.sh already runs the smoke test as its first step, so
# invoking it alone runs the full sequence end to end.
#
# Usage:
#   ./run.sh            Run the smoke test, then start bitcoind (regtest)
#   ./run.sh smoke      Run only the download/verification smoke test
#   ./run.sh daemon     Run the full secure startup + daemon (default)

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

MODE="${1:-daemon}"

case "$MODE" in
    smoke)
        exec "$ROOT_DIR/scripts/smoke_bit-block.sh"
        ;;
    daemon)
        exec "$ROOT_DIR/scripts/secure_startup.sh"
        ;;
    *)
        echo "Usage: $0 [smoke|daemon]" >&2
        exit 1
        ;;
esac
