#!/usr/bin/env bash
set -x
SCRIPT_DIR="$(dirname -- "$(readlink -f -- "${BASH_SOURCE}")")"

## Configuration and generation stage
cmake  -S "${SCRIPT_DIR}"  \
       -B "${SCRIPT_DIR}/build"

cmake  --build "${SCRIPT_DIR}/build"  --target run  # --clean-first
# cmake  --build "${SCRIPT_DIR}/build"  --target clean
