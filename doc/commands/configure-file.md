# Configuring the headers

Instead of `target_compile_definitions()` we may use `configure_file()` command for generating new files (headers) from templates consisting placeholders referencing various CMake variables.

```cmake
configure_file(
  <input>
  <output>
  [NO_SOURCE_PERMISSIONS           |
   USE_SOURCE_PERMISSIONS          |
   FILE_PERMISSIONS <permissions>... ]
  [COPYONLY]
  [ESCAPE_QUOTES]
  [@ONLY]
  [NEWLINE_STYLE [UNIX|DOS|WIN32|LF|CRLF] ]
)
```

* **`@ONLY`**  \
    &emsp;Restrict variable replacement to references of the form `@VAR@`. This is useful for configuring scripts that use `${VAR}` syntax.
* **`NEWLINE_STYLE <style>`**  \
    &emsp;Specify the newline style for the output file. Specify `UNIX` or `LF` for `\n` newlines, or specify `DOS`, `WIN32`, or `CRLF` for `\r\n` newlines.
