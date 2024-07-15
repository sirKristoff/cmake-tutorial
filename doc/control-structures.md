# Control structures

## 1. Conditional blocks

```cmake
if(<condition>)
    <commands>
elseif(<condition>)    # optional block, can be repeated
    <commands>
else()                 # optional block
    <commands>
endif()
```

## 2. Loops

```cmake
while(<condition>)
    <commands>
    break()    # optional, stop the loop execution
endwhile()
```

#### Iterate from `0` to `<max>` (inclusive):

```cmake
foreach(<loop_var> RANGE <max>)
    <commands>
    continue()    # optional, stop the execution of curent iteration and start the next one
endforeach()
```

#### Second `foreach` variant:

```cmake
foreach(<loop_var> RANGE <min> <max> [<step>])
# all values must be nonnegative integers
# <min> has to be smaller than <max>
```

#### List `foreach` variant:

```cmake
foreach(<loop_var>  IN  [LISTS <lists>]  [ITEMS <items>])
```

CMake will take elements from all of the provided `<lists>` list variables, followed by all of the explicitly stated `<items>` values, and store them in `<loop_var>`.

Example:

```cmake
set(myList "1;2;3")
foreach(var IN LISTS myList ITEMS "e" "f")
    message(${var})
endforeach()
## Printout:
# 1
# 2
# 3
# e
# f

## Equivalent:
# foreach(var 1 2 3 e f)
```

#### Zip `foreach` variant (since CMake `3.17`):


```cmake
foreach(<loop_var>... IN  ZIP_LISTS <lists>)
```

Example:

```cmake
set(listKeys "one;two;three;four")
set(listVals "1;2;3;4;5")

foreach(pair IN  ZIP_LISTS listKeys listVals)
    message("key=${pair_0}, val=${pair_1}")
endforeach()

foreach(key val IN  ZIP_LISTS listKeys listVals)
    message("key=${key}, val=${val}")
endforeach()
```

If the count of items differs between lists, CMake won't define variables for shorters ones.


## 3. Condition syntax

### 3.1. Logical operators

* `NOT <condition>`
* `<condition> AND <condition>`
* `<condition> OR  <condition>`

### 3.2. The evaluation of a string and a variable

For _unquoted arguments_ using plain variable name (e.q. `var`) inside condition is equal to writing `${var}`.

**Strings** are considered Boolean **`true`** if any of (case insensitive):

* `ON`
* `Y`
* `TRUE`
* A non-zero number

**Unquoted variable reference** is evaluated to **`false`** if any of (case insensitive):

* `OFF`
* `NO`
* `FALSE`
* `N`
* `IGNORE`
* `NOTFOUND`
* A string ending with `-NOTFOUND`
* An empty string (`""`)
* Zero (`0`)

#### Check if the variable **is defined**:

* `if (DEFINED  <var_name>)`
* `if (DEFINED  CACHE{<var_name>})`
* `if (DEFINED  ENV{<var_name>})`

### 3.3. Comparing values

#### Operators:

* `EQUAL`
* `LESS`  (`<`)
* `LESS_EQUAL`
* `GREATER`  (`>`)
* `GREATER_EQUAL`

If one of the operands is **not a number**, the value will be **`false`**.

#### Prefixed operators:

* `VERSION_` - software versions formated: `major[.minor[.patch[.tweak]]]`
* `STR` - lexicographic comparisons

#### **POSIX regex**

```cmake
<variable_name | "String"> MATCHES <regex>
```

### 3.4. Functional checks:

* `<variable_name | "String">  IN_LIST  <var>`  - Value in list
* `COMMAND  <command-name>`  - Command available for invocation
* `POLICY  <policy-id>`  - CMake policy exists
* `TEST  <test-name>`  - CTest was added with `add_test()`
* `TARGET  <target-name>`  - Build target defined

### 3.5. Examining the filesystem:

* `EXISTS  <path>`  - File or directory exists (`true` if sympolic link points to file/directory)
* `<file1>  IS_NEWER_THAN  <file2>`  - `file1` newer or equal to `file2` (`true` if any file **doesn't exist**)
* `IS_DIRECTORY  <file-name>`
* `IS_SYMLINK  <file-name>`
* `IS_ABSOLUTE  <path>`  - Path is absolute