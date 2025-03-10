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
  -t | --test
        : Run tests
  --test-log
        : Print test log file
  --clean
        : Clean build
  --rm
        : Remove build directory
EOT
}


case $1 in

  ''|--)
    [ $# -ne 0 ] && shift
    # $0 --rm
    $0 --configure &&  \
    $0 --build &&  \
    $0 --test  $@
    ;;

  -c|--configure|-g|--generate)
    echo -e '\n====  --configure  ===='
    set -x
    cmake  -S "${SCRIPT_DIR}"  \
           -B "${SCRIPT_DIR}/build"
    ;;

  -b|--build)
    echo -e '\n===  --build  ===='
    set -x
    cmake  --build "${SCRIPT_DIR}/build"  \
           --config "Debug"
    ;;

  -t|--test)
    echo -e '\n===  --test  ===='
    shift
    set -x
    cd "${SCRIPT_DIR}/build/test"
        # --verbose
    ctest --timeout 10  \
          --build-config "Debug"  \
          --output-on-failure  \
          --output-log Testing/Temporary/ctest.log  \
          $@
    # "${SCRIPT_DIR}/build/test/prio_containers/prio_containers_test"
    ;;

  --test-log)
    echo -e '\n===  --test-log  ===='
    set -x
    less "${SCRIPT_DIR}/build/test/Testing/Temporary/LastTest.log"
    ;;

  --clean)
    echo -e '\n===  --clean  ===='
    set -x
    cmake  --build "${SCRIPT_DIR}/build"  \
           --target clean
    ;;

  --rm)
    echo -e '\n===  --rm  ===='
    set -x
    rm -rfv "${SCRIPT_DIR}/build"
    ;;

  *)
    help
    exit 1
    ;;
esac
