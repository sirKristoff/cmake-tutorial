# The `message()` command.

```cmake
message([<MODE>] "Text")
```

#### Recognized modes:

* `FATAL_ERROR`  - **Stops** processing and generation
* `SEND_ERROR`  - Continues processing, but **skips generation**
* `WARNING`
* `AUTHOR_WARNING`  - A CMake warning
* `DEPRECATION`  - Works accordingly if either of the `CMAKE_ERROR_DEPRECATION` or `CMAKE_WARN_DEPRECATED` variables are enabled.
* `NOTICE`  - Default mode, set if mode omitted. Prints to **`stderr`**.
* `STATUS`
* `VERBOSE`
* `DEBUG`
* `TRACE`

## 1. Log level

The `--log-level` command-line option can be used to control which messages will be shown (`STATUS` by default).

```bash
cmake --log-level=TRACE
```

##### _Since CMake `3.17`:_

To make a log level persist between CMake runs, the `CMAKE_MESSAGE_LOG_LEVEL` variable can be set instead.


## 2. Log context

##### _Since CMake `3.17`:_

For printing **messages with context** in generation stage use `cmake --log-context` command-line flag (or set the `CMAKE_MESSAGE_CONTEXT_SHOW` variable to `true`) and `CMAKE_MESSAGE_CONTEXT` variable (list).

```cmake
# extend context for messages in current scope
list(APPEND CMAKE_MESSAGE_CONTEXT "${CMAKE_CURRENT_FUNCTION}")
```

## 3. Indentation

##### _Since CMake `3.16`:_

Messages of log levels `NOTICE` and below will have each line preceded by the content of the `CMAKE_MESSAGE_INDENT` variable.

```cmake
# add indentation for messages
list(APPEND CMAKE_MESSAGE_INDENT "  ")
```

## 4. Reporting checks

##### _Since CMake `3.17`:_

A common pattern in CMake output is a message indicating the **start of** some sort of **check**, followed by another message reporting the **result of that check**.

```cmake
message(<checkState> "Text")
```

`<checkState>` must be one of the following:

* `CHECK_START`
* `CHECK_PASS`
* `CHECK_FAIL`

When recording a check result, the command repeats the message from the most recently **started check** for which no result has yet been reported, then some separator characters and then the message text provided after the `CHECK_PASS` or `CHECK_FAIL` keyword.
Check messages are always reported at `STATUS` log level.

##### _Example:_

```cmake
message(CHECK_START "Finding my things")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
unset(missingComponents)

# nested check
message(CHECK_START "Finding partA")
# ... do check, assume we find A
message(CHECK_PASS "found")

message(CHECK_START "Finding partB")
# ... do check, assume we don't find B
list(APPEND missingComponents B)
message(CHECK_FAIL "not found")

list(POP_BACK CMAKE_MESSAGE_INDENT)
if(missingComponents)
  message(CHECK_FAIL "missing components: ${missingComponents}")
else()
  message(CHECK_PASS "all components found")
endif()
```

Output from the above would appear something like the following:

```
-- Finding my things
--   Finding partA
--   Finding partA - found
--   Finding partB
--   Finding partB - not found
-- Finding my things - missing components: B
```
