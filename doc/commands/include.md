# Including files

## 1. The `include()` command

```cmake
include(
    "<file|module>"
    [OPTIONAL]
    [RESULT_VARIABLE <var>]
)
```

Include file and **no separate scope** is created.

Error is raised if **a file doesn't exist** unless `OPTIONAL` keyword is specified.

Result may be stored in optional variable specified by a `RESULT_VARIABLE` keyword (path to included file or `NOTFOUND` on failure).


### `"<file>"`

If we provide a filename (a path with a `.cmake` extension), CMake opens and executes it.


### `"<module>"`

If the name of a module (without `.cmake`) is provided, without path, CMake try to find a module and include it.
A file with the name of `<module>.cmake` is searched for in `CMAKE_MODULE_PATH` and CMake module directory.


## 2. The `include_guard()` command

```cmake
include_guard([DIRECTORY | GLOBAL])
```

Make restriction to include file only once.

Restriction takes effect only in the current scope. \
`DIRECTORY` argument applies protection in within the current directory and below, \
`GLOBAL` keyword in the whole build.
 
 ## 3. Scoped subdirectories

```cmake
add_subdirectory(
    source_dir
    [binary_dir]
    [EXCLUDE_FROM_ALL]
)
```

* `source_dir`  - Path (relative to the current directory) where `CMakeLists.txt` file will be parsed in directory scope
* `binary_dir`  - Path in which built files will be written
* `EXCLUDE_FROM_ALL`  - Disables the default building of targets defined in the subdirectory
