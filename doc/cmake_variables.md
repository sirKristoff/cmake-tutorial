# CMake predefined variables

## Cross-compilation 

 Name                           | Description                           | Values
--------------------------------|---------------------------------------|--------
 `CMAKE_SYSTEM_NAME`            |  Has to be set for cross-compilation  | `"Linux"` \| `"Darwin"` \| `"Windows"` \| `"AIX"`
 `CMAKE_SYSTEM_VERSION`         |  Has to be set for cross-compilation  |
 `CMAKE_HOST_SYSTEM`            |                                       |
 `CMAKE_HOST_SYSTEM_NAME`       |                                       |
 `CMAKE_HOST_SYSTEM_VERSION`    |                                       |
 `CMAKE_HOST_SYSTEM_PROCESSOR`  |                                       |

## Predefined host and target system variables

If a specific system is used, and appropriate variable will be set to a non-false value (`1` or `true`):

 * `ANDROID`
 * `APPLE`
 * `CYGWIN`
 * `UNIX`  -  for Linux, macOS and Cygwin
 * `IOS`
 * `WIN32`  -  for 32- and 64-bit versions of Windows and MSYS
 * `WINCE`
 * `WINDOWS_PHONE`
 * `CMAKE_HOST_APPLE`
 * `CMAKE_HOST_SOLARIS`
 * `CMAKE_HOST_UNIX`  -  for Linux, macOS and Cygwin
 * `CMAKE_HOST_WIN32`  -  for 32- and 64-bit versions of Windows and MSYS

## Host system information

For rarely needed information you have to ask explicitly with the following command:

```cmake
  cmake_host_system_information(
    RESULT <VARIABLE>
    QUERY <KEY>...
  )
```

### The environment and the OS details:

 Key                          | Returned value
------------------------------|----------------
 `HOSTNAME`                   |  The hostname
 `FQDN`                       |  The fully qualified domain name
 `TOTAL_VIRTUAL_MEMORY`       |  The total virtual memory in MiB
 `AVAILABLE_VIRTUAL_MEMORY`   |  The available virtual memory in MiB
 `TOTAL_PHYSICAL_MEMORY`      |  The total physical memory in MiB
 `AVAILABLE_PHYSICAL_MEMORY`  |  The available physical memory in MiB
 `OS_NAME`                    |  The output of `uname -s` if this command is present, or one of `Windows`, `Linux`, or `Darwin`
 `OS_RELEASE`                 |  The OS sub-type, e.q. Windows Professional
 `OS_VERSION`                 |  The OS build ID
 `OS_PLATFORM`                |  On Windows the `PROCESSOR_ARCHITECTURE` env variable value. On Unix and macOS output of `uname -m`
---

### Processor-specific information:

 Key                         | Returned value
-----------------------------|----------------
 `NUMBER_OF_LOGICAL_CORES`   |  The number of logical cores
 `NUMBER_OF_PHYSICAL_CORES`  |  The number of physical cores
 `HAS_SERIAL_NUMBER`         |  `1` if the processor has a serial number
 `PROCESSOR_SERIAL_NUMBER`   |  The processor serial number
 `PROCESSOR_NAME`            |  The human-readable processor name
 `PROCESSOR_DESCRIPTION`     |  The human-readable full processor description
 `IS_64BIT`                  |  `1` if the processor is 64 bit
 `HAS_FPU`                   |  `1` if the processor has floating-point unit
 `HAS_MMX`                   |  `1` if the processor supports MMX instructions

## Architecture specific information:

 Name                       | Description                                                 | Values
----------------------------|-------------------------------------------------------------|--------
 `CMAKE_SIZEOF_VOID_P`      |  The pointer size                                           |  `8` for 64-bit and `4` for 32-bit
 `CMAKE_<LANG>_BYTE_ORDER`  |  Endianness for `<LANG>` one of `C`, `CXX`, `OBJC`, `CUDA`  |  `BIG_ENDIAN` \| `LITTLE_ENDIAN`

## The toolchain configuration

### C++ standard

For project set **`CMAKE_CXX_STANDARD`** variable to one of values: `98`, `11`, `14`, `17`, `20` or `23` (since CMake 3.20).

Override on a **per-target** basis, like so:

```cmake
  set_property(
    TARGET <target>
    PROPERTY CXX_STANDARD <standard>
  )
```

Explicitly require the standard:

```cmake
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Disallows using extensions (like `-std=gnu++14`):

```cmake
  set(CMAKE_CXX_EXTENSIONS OFF)
```
