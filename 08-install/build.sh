#!/usr/bin/env bash
set -eo pipefail
SCRIPT_DIR="$(dirname -- "$(readlink -f -- "${BASH_SOURCE}")")"

help()
{
    cat <<"EOT"
Usage:

  build.sh [option]

Options:

  -h | --help
        :  Print version number and exit.
  <no option>
        : Make configuration, generation and build stage
  -c | --configure
  -g | --generate
        : Make configuration and generation stage from scratch
  -b | --build
        : Build project
  -i | --install
  -p | --pack
        : Pack build artifacts into binary package
  --clean
        : Clean build
  --rm
        : Remove build directory
EOT
}


case $1 in

  '')
    $0 --configure
    $0 --build
    $0 --install
    ;;

  -c|--configure|-g|--generate)
    set -x
    rm -rf  "${SCRIPT_DIR}/build"
    cmake  -S "${SCRIPT_DIR}"  \
           -B "${SCRIPT_DIR}/build"
    ;;

  -b|--build)
    set -x
    cmake  --build "${SCRIPT_DIR}/build"  \
           --config "Debug"
    ;;

  -i|--install)
    set -x
    cmake  --install "${SCRIPT_DIR}/build"  \
           --prefix "${SCRIPT_DIR}/build/install"  \
           --config Debug
    ;;

  -p|--pack)
    set -x
    cpack  --verbose  \
           --config "${SCRIPT_DIR}/build/CPackConfig.cmake"  \
           -G "TGZ;ZIP;DEB"  \
           -B "${SCRIPT_DIR}/build/packages"
    ;;

  --clean)
    set -x
    cmake  --build "${SCRIPT_DIR}/build"  \
           --target clean
    ;;

  --rm)
    set -x
    rm -rfv "${SCRIPT_DIR}/build"
    ;;

  *)
    help
    exit 1
    ;;
esac
