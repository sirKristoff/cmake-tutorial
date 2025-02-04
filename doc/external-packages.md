# Managing dependencies to external packages

<!-- TOC -->

- [1. `find_package()`](#1-find_package)
    - [1.1. Procedure of finding package:](#11-procedure-of-finding-package)
    - [1.2. Variables set by `find_package()`](#12-variables-set-by-find_package)
- [2. `FindPkgConfig()`](#2-findpkgconfighttpscmakeorgcmakehelplatestmodulefindpkgconfightml)
    - [2.1. Command line tool](#21-command-line-tool)
    - [2.2. Usage of CMake commands](#22-usage-of-cmake-commands)
    - [2.3. Variables set by `pkg_check_modules()`](#23-variables-set-by-pkg_check_modules)
- [3. Own _find-module_](#3-own-_find-module_)

<!-- /TOC -->
-------------

## 1. `find_package()`

Command for discovering packages in a system.

```cmake
find_package(
    <Name>
    [<version>]  # request a specific version e.q 1.22...1.40.1
    [EXACT]      # exact version requested (a range is not supported)
    [QUIET]      # silences all messages about a found/not found package
    [REQUIRED]   # stop execution if a package is not found and print a diagnostic message
)
```

### 1.1. Procedure of finding package:

* Search for _**find-module**_ and _**config-file**_ in path stored in the **`CMAKE_MODULE_PATH`** variable (empty by default).
* Scan the list of _**build-in** find-modules_ in installed CMake (e.q. `FindProtobuf.cmake`).
* Search for corresponding package _**config-files**_ in host operating system paths. \
  Filenames must match the following patterns:
  * `<CamelCasePackageName>Config.cmake`
  * `kebab-case-package-name>-config.cmake`

### 1.2. Variables set by `find_package()`

In most cases, you can expect some variables to be set when you call `find_package()`:

* `<PKG_NAME>_FOUND`  e.q. `Protobuf__FOUND`
* `<PKG_NAME>_INCLUDE_DIRS` or `<PKG_NAME>_INCLUDES`
* `<PKG_NAME>_LIBRARIES` or `<PKG_NAME>_LIBS`
* `<PKG_NAME>_DEFINITIONS`
* `IMPORTED` targets specified by the _find-module`_ or _config-file_, usually:
  * `<PKG_NAME>::lib<PKG_NAME>` e.q. `protobuf::libprotobuf`
  * `<PKG_NAME>::lib<PKG_NAME>-lite`


## 2. [`FindPkgConfig()`](https://cmake.org/cmake/help/latest/module/FindPkgConfig.html)

One of tool for building packages [`PkgConfig`](http://freedektop.org/wiki/Software/pkg-config/).

Example `.pc` file containing details necessary for compilation and linking:

```
prefix=/usr/local
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib

Name: foobar
Description: A foobar library
Version: 1.0.0
Cflags: -I${includedir}/foobar
Libs: -L${libdir} -lfoobar
```

### 2.1. Command line tool

* Get linker flags for a library by running: \
  `pkg-config --libs libfoo`
* Get the _include directories_ for a library: \
  `pkg-config --cflags libfoo`

### 2.2. Usage of CMake commands

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(
  PQXX REQUIRED            # first arg specify new target name
  IMPORTED_TARGET libpqxx  # new target containing 'libpqxx' will be created with name 'PkgConfig::PQXX'
)

target_link_libraries(
  main
  PRIVATE PkgConfig::PQXX
)
```

### 2.3. Variables set by `pkg_check_modules()`

* `<PKG_NAME>_EXECUTABLE`
* `<PKG_NAME>_FOUND`


## 3. Custom _find-module_

---
