# The `execute_process()` command.

Run other process and collect its output.
It can be used during the **configuration stage**.
To run at **build time**, use the `add_custom_target()` or `add_custom_command()` command.

CMake will use the API of the operating system to create a child process (so, shell operators such as `&&`, `||`, and `>` **won't work**). \
If the `COMMAND` argument is **used more then once**, commands are **chained** (as a pipeline) and the output of one command is passed to another.

```cmake
execute_process(
    COMMAND <cmd1> [<arguments>]...
    [WORKING_DIRECTORY <directory>]
    [TIMEOUT <seconds>]
    [RESULT_VARIABLE <variable>]
    [RESULTS_VARIABLE <variable>]
    [OUTPUT_VARIABLE <variable>]
    [ERROR_VARIABLE <variable>]
)
```

### Options:

* `COMMAND`  - A child process command line
* `WORKING_DIRECTORY`  - Set as the current working directory of the child processes
* `TIMEOUT`  - Terminate all unfinished child processes after number of seconds (fractions allowed), and the `RESULT_VARIABLE` is set to a string mentioning the "timeout".
* `RESULT_VARIABLE`  - The result of the last executed command
* `RESULTS_VARIABLE`  - A list of the exit codes of all tasks
* `OUTPUT_VARIABLE`  - Collected output (`stdout`) of the **last** `COMMAND` process
* `ERROR_VARIABLE`  - Collected error outputs (`stderr`) of **all** `COMMAND` process
