#!/usr/bin/env bash
set -x
SCRIPT_DIR="$(dirname -- "$(readlink -f -- "${BASH_SOURCE}")")"

## Configuration and generation stage

# generate project and show logs on TRACE level and above, default STATUS
# -S : source tree directory
# -B : build tree directory
# log levels: ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE
# cache variable: CMAKE_MESSAGE_LOG_LEVEL
cmake  -S "${SCRIPT_DIR}"  \
       -B "${SCRIPT_DIR}/build"  \
       --log-level=TRACE

# Generate Debug/Release/MinSizeRel/RelWithDebInfo build type in configuration phase
# --trace : in trace mode print every command with the filename and line number and arguments
cmake  -S "${SCRIPT_DIR}"  \
       -B "${SCRIPT_DIR}/build_debug"  \
       -D CMAKE_BUILD_TYPE=Debug  # --trace


## Build stage

# build project but clean before it
# -j | --parallel - number of parallel jobs for build
cmake  --build "${SCRIPT_DIR}/build"  \
       --clean-first  \
       --parallel 4

# clean target
# cmake --build "${SCRIPT_DIR}/build" --target 'clean'


## Installation mode

cmake --install "${SCRIPT_DIR}/build"  \
      --prefix "${SCRIPT_DIR}/install"
