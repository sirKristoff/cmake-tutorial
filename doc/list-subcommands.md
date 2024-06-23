# `list()` subcommands

CMake offers a `list()` command that provides a multitude of subcommands to read, search, modify, and order lists.
Here's short summary:

* `list(LENGTH  <list>  <out-var>)`
* `list(GET  <list>  <element-index> [<index> ...]  <out-var>)`
* `list(JOIN  <list>  <glue>  <out-var>)`
* `list(SUBLIST  <list>  <begin>  <length>  <out-var>)`
* `list(FIND  <list>  <value>  <out-var>)`
* `list(APPEND  <list>  [<element>...])`
* `list(FILTER  <list>  {INCLUDE | EXCLUDE}  REGEX <regex>)`
* `list(INSERT  <list>  <index>  [<element>...])`
* `list(POP_BACK  <list>  [<out-var>...])`
* `list(POP_FRONT  <list>  [<out-var>...])`
* `list(PREPEND  <list>  [<element>...])`
* `list(REMOVE_ITEM  <list>  <value>...)`
* `list(REMOVE_AT  <list>  <index>...)`
* `list(REMOVE_DUPLICATES  <list>)`
* `list(TRANSFORM  <list>  <ACTION> [...])`
* `list(REVERSE  <list>)`
* `list(SORT  <list>  [...])`
