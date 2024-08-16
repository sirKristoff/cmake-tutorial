<!-- TOC -->

- [1. Target properties](#1-target-properties)
    - [1.1. Standard properties](#11-standard-properties)
    - [1.2. _Property_ commands:](#12-_property_-commands)
- [2. Imported targets](#2-imported-targets)
- [3. Interface libraries](#3-interface-libraries)
    - [3.1. Header-only libraries](#31-header-only-libraries)
    - [3.2. Properties handler libraries](#32-properties-handler-libraries)
- [4. Custom commands](#4-custom-commands)
    - [4.1. Custom command as a **generator**](#41-custom-command-as-a-generator)
    - [4.2. Custom command as a **target hook**](#42-custom-command-as-a-target-hook)

<!-- /TOC -->
# Concept of targets

Target is a high-level logical unit that forms a single objective for CMake

For creating target use commands:

 * `add_executable()`
 * `add_library()`
 * `add_custom_target()`

**Custom target** allows to specify own command line that will be executed _without checking whether the produced output is up to date_.
Custom target don't necessarily have to produce tangible artifacts in the form of file.

Unlike executable and library targets, custom targets **won't be build** until they are added to a dependency graph.

## 1. Target properties

```cmake
  get_target_property(
    <var>
    <target>
    <property-name>
  )
  set_target_properties(
    <target1>  <target2> ...
    PROPERTIES
      <prop1-name>  <value1>
      <prop2-name>  <value2> ...
  )
```

### 1.1. Standard properties

 Property name                | Property command                 | Argument example | Environment variable
------------------------------|----------------------------------|------------------|----
 `AUTOUIC_OPTIONS`            |
 `COMPILE_DEFINITIONS`        |  `target_compile_definitions()`  |  `"NDEBUG -D'X(x)=class x;'"`  |  `CXXFLAGS` `"-D"`
 `COMPILE_FEATURES`           |  `target_compile_features()`     |  `"cxx_std_17 cxx_decltype_auto"`
 `COMPILE_OPTIONS`            |  `target_compile_options()`      |  `"-Wall -Wpedantic"`  | `CXXFLAGS`
 `INCLUDE_DIRECTORIES`        |  `target_include_directories()`  |  `"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mylib>`<br>&NoBreak;` $<INSTALL_INTERFACE:include/mylib>"`  |  `CXXFLAGS` `"-I"`
 `LINK_DEPENDS`               |  `set_target_properties(`<br>&emsp;`${TARGET_NAME} PROPERTIES`<br>&emsp;`LINK_DEPENDS linker.ld)`  | `"linker.ld"`
 `LINK_DIRECTORIES`           |  `target_link_directories()`     |  | `LDFLAGS` `"-L"`
 `LINK_LIBRARIES`             |  `target_link_libraries()`       |  `"GTest::GTest debug zlibd optimized zlib"`  |  `LDFLAGS` `"-l"`
 `LINK_OPTIONS`               |  `target_link_options()`         |  `"-pthread LINKER:-rpath,."`  |  `LDFLAGS`
 `POSITION_INDEPENDENT_CODE`  |
 `PRECOMPILE_HEADERS`         |  `target_precompile_headers()`   |  `"<unordered_map>`<br>&NoBreak;` $<$<COMPILE_LANGUAGE:C>:<stddef.h$<ANGLE-R>>"`
 `SOURCES`                    |  `target_sources()`

### 1.2. _Property_ commands:

CMake supports setting properties of other scopes as well:

 * `GLOBAL`
 * `DIRECTORY`
 * `SOURCE`
 * `INSTALL`
 * `TEST`
 * `CACHE`

```cmake
  set_property(
    <
    GLOBAL                                       |
    DIRECTORY [<dir>]                            |
    TARGET    [<target1> ...]                    |
    SOURCE    [<src1> ...]
              [DIRECTORY  <dirs> ...]
              [TARGET_DIRECTORY  <targets> ...]  |
    INSTALL   [<file1> ...]                      |
    TEST      [<test1> ...]
              [DIRECTORY  <dir>]                 |
    CACHE     [<entry1> ...]
    >
    [APPEND] [APPEND_STRING]
    PROPERTY
      <name> [<value1> ...]
  )
```

## 2. Imported targets

Created by external dependencies (other projects, libraries...), CMake defines imported targets as result of the `find_package()` command.

Following target properties can be adjusted: _compile definitions_, _compile options_, _include directories_...
However targets should be treated as immutable.

## 3. Interface libraries

A library that **doesn't compile anything** but instead serves as a utility target. Its whole concept is built around propagated properties (_transitive usage requirements_).

### 3.1. Header-only libraries

Example:

```cmake
add_library(
  Eigen INTERFACE
  src/eigen.h
  src/vector.h
  src/matrix.h
)
target_include_directories(
  Eigen INTERFACE
  $<BUILD_INTERFACE:"${CMAKE_CURRENT_SOURCE_DIR}/src">
  $<INSTALL_INTERFACE:"include/Eigen">
)
```

 * `BUILD_INTERFACE` when target is **exported**
 * `INSTALL_INTERFACE` when target is **installed**

### 3.2. Properties handler libraries

Example:

```cmake
add_library(warning_props INTERFACE)
target_compile_options(
  warning_props INTERFACE
  -Wall -Wextra -Wpedantic
)
```

## 4. Custom commands

### 4.1. Custom command as a **generator**

_Custom command_ is used for **producing files without need of recreating them** every build. It doesn't create a logical target, but just like _custom target_, it has to be added to a dependency graph.

Example:

```cmake
add_custom_command(
  OUTPUT
    "person.pb.h"
    "person.pb.cc"
  COMMAND protoc
  ARGS "person.proto"
  DEPENDS "person.proto"
)
add_executable(
  serializer
  "serializer.cpp"
  "person.pb.cc"
)
```

### 4.2. Custom command as a **target hook**

Adds a _custom command_ to a target such as a library or executable.
This is useful for performing an operation before or after building the target.
The command becomes part of the target and will only execute when the target itself is built.
If the target is already built, the command will not execute.

Example:

```cmake
add_executable(main "main.cpp")
add_custom_command(
  TARGET main POST_BUILD
  COMMAND cksum
  ARGS "$<TARGET_FILE:main>" > "main.ck"
  VERBATIM
)
```
