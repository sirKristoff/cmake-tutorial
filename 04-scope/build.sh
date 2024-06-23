#!/usr/bin/env bash
set -x
SCRIPT_DIR="$(dirname -- "$(readlink -f -- "${BASH_SOURCE}")")"

## Configuration and generation stage

cmake  -S "${SCRIPT_DIR}"  \
       -B "${SCRIPT_DIR}/build"  \
       --log-level=TRACE  \
       --log-context

# rm -rf "${SCRIPT_DIR}/build"
