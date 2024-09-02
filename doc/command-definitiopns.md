# Command definitions

Command definitions are required before their calls.

Commonly used naming convention is `snake_case`.

## 1. Macros

```cmake
macro( <name>, [<argument>...] )
  <commands>
endmacro()
```

Works like a _find-and-replace_ instruction, it doesn't create a separate entry on **call stack**.
`return()` does not affect macro but calling statement last on call stack.

After macro declaration, it may be **executed** by calling its name (function calls are **case-insensitive**).

**Arguments** passed to macros aren't treated as real variables but rather as constant _find-and-replace_ instructions.


## 2. Functions

```cmake
function( <name>, [<argument-name>...] )
  <commands>
endfunction()
```

Creates a separate scope for local variables.
For changing variables in **parent scope** use `PARENT_SCOPE`.

```cmake
# change value only in parent scope!
set(var "value for parent" PARENT_SCOPE)
```

For printing **messages with context** in generation stage use `cmake --log-context` flag and `CMAKE_MESSAGE_CONTEXT` variable.

```cmake
# extend context for messages in current scope
list(APPEND CMAKE_MESSAGE_CONTEXT "${CMAKE_CURRENT_FUNCTION}")
```

#### **Arguments** references:

* `${ARGC}`  - Count of arguments
* `${ARGV}`  - A list of all arguments
* `${ARG<n>}`  - The value of n-th argument
* `${ARGN}`  - A list of anonymous arguments

If a function call **passes more arguments** than were declared,
the excess arguments will be interpreted as **anonymous arguments** and stored in the **`ARGN`** variable.

#### Automatic variables for functions:

* `CMAKE_CURRENT_FUNCTION`
* `CMAKE_CURRENT_FUNCTION_LIST_DIR`
* `CMAKE_CURRENT_FUNCTION_LIST_FILE`
* `CMAKE_CURRENT_FUNCTION_LIST_LINE`
