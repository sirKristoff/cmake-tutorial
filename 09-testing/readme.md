# Integration of testing frameworks

## `ctest` command-line tool

Discovers which file needs to be run, which framework is used, which arguments
should be passed to the runner, and how to collect results.

If all tests pass, `ctest` will return a `0` exit code.

### Modes of operations

- Test
- Build-and-test
- Dashboard client ([CDash](https://www.cdash.org))

### The *Build-and-test* mode

```bash
ctest --build-and-test  "source-path" "build-path"
      --build-generator  <generator> [<options>...]
      [--build-options  <options>...]
      [--test-command  <command>  [<args>...]]

$ ctest --build-and-test  "." "./build"  \
        --build-generator  "Unix Makefiles"  \
        --test-command  ctest
```

#### Additional arguments for controlling **configuration** stage

- `--build-options`  : Extra options for the `cmake` configuration
- `--build-two-config`  : Run the configuration stage for CMake twice
- `--build-nocmake`  : Skip the configuration stage
- `--build-generator-platform`  : Provide a generator-specific platform
- `--build-generator-toolset`  : Provide a generator-specific toolset
- `--build-makeprogram`  : Specify a `make` executable when using Make- or
      Ninja-based generators

#### Additional arguments for controlling **build** stage

- `--build-target`  : Build the specific target
- `--build-noclean`  : Build without the `clean` target first
- `--build-project`  : Provide the name of the build project

#### Additional arguments for controlling **test** stage

- `--test-timeout`  : Limit the execution of tests (in seconds)

### The *Test* mode

CTest should be executed in the build tree, after building the project
with `cmake`.

```bash
ctest [options]
```

### Querying tests

CTest offers an `-N` option, which disables execution and only prints a **list
of test cases**. Flag `--show-only=json-v1` sets **JSON** output format.

For printing available **labels** (groups of tests) use `--print-labels`. \
Label for test may be assigned through its property:

```cmake
add_test(test_name <test-command>)
set_tests_properties(test_name PROPERTIES LABELS "test_label")
```

### Filtering tests

Regular expression filtering:

- `-R <regex>`,  `--tests-regex <regex>`  : **Run** test **names** matching regex
- `-E <regex>`,  `--exclude-regex <regex>`  : **Skip** test **names** matching regex
- `-L <regex>`,  `--label-regex <regex>`  : **Run** test **labels** matching regex
- `-LE <regex>`, `--label-exclude <regex>`  : **Skip** test **labels** matching regex

Range in comma-separated format filtering:

```text
  --tests-information | -I  <start>, <end>, <step> [, <test_id>...]
```

examples:

- `-I 3,,`  : start from 3rd
- `-I ,2,`  : only 1st and 2nd
- `-I 2,,3`  : every third test, starting from 2nd
- `-I ,0,,3,9,7`  : only 3rd, 9th and 7th

By default, the `-I` option used with `-R` will narrow the execution (run tests
matching **both** requirements), unless `-U` (union) option is used.

### Shuffling tests

CTest executing every test case in a child CTest instance. By using
`--force-new-ctest-process` option creation of separate process is enforced.

`--schedule-random` oprion randomize the order of test execution.

### Handling failures

To enable printing messages to `stdout` when test fail use `--output-on-failure`
option or set environment variable `CTEST_OUTPUT_ON_FAILURE`. \
`--stop-on-failure` option stops execution after any of the tests fail. \
`--rerun-failed` option skips running the passing tests. \
`--no-tests=error` option causes that empty test list is an error (ctest returns
nonzero exit code) unless `--no-tests=ignore` is used.

### Repeating tests

Flaky tests may be run repeatedly with `--repeat <mode>:<n>` option. \
Available modes (`<mode>`):

- `until-fail`  : Require each test to run `<n>` times without failing in order
    to pass
- `until-pass`  : Allow each test to run up to `<n>` times in order to pass,
    with repetition on failure
- `after-timeout`  : Allow each test to run up to `<n>` times in order to pass,
    with repetition only if test timeout

### Controlling output

`-V` or `--verbose` option enables more **verbose** output. \
`-VV` or `--extra-verbose` option enables extremely in-depth debugging. \
`-Q` or `--quiet` option **suppress** any printed output (output will be stored
  in test files, by default in `./Testing/Temporary`). \
To store the logs in a specific path, use the `-O <file>` or
  `--output-log <file>` option. \
`--test-output-size-passed <size>` and `--test-output-size-failed <size>`
  options limit one **test log size** in bytes.

### Miscellaneous

`-C <cfg>` or `--build-config <cfg>` option specify which configuration should
  be tested (`Debug`, `Release` etc.). \
`-j <jobs>` or `--parallel <jobs>` sets the number of tests executed in parallel. \
`--test-load <level>` option allow parallel tests to **load CPU** at most `<level>`. \
`--timeout <seconds>` option sets **timeout** for **one** test.
