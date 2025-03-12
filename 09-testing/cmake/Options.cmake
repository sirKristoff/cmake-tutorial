set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  ## Explicitly require supporting standard by compiler
set(CMAKE_CXX_EXTENSIONS OFF)  ## Disallows using extensions like gnu++14


# Make build type 'Debug' if want to perform coverage tests.
if (${PROJECT_NAME}_ENABLE_COVERAGE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(NOTICE "-- Setting CMAKE_BUILD_TYPE=Debug to enable test coverage.")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ..."
      FORCE)
endif()
# Has to build tests to test coverage
if (${PROJECT_NAME}_ENABLE_COVERAGE AND NOT ${PROJECT_NAME}_BUILD_TESTS)
  message(NOTICE "-- Enabling ${PROJECT_NAME}_BUILD_TESTS to test coverage.")
  set(${PROJECT_NAME}_BUILD_TESTS ON CACHE BOOL "Set to ON to build tests" FORCE)
endif()


# Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
# to the library and the library users.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/utf-8")
endif()

# TODO: cleanup that
# # CMake does automatically add -fPIC when linking a shared library, but it
# # does not add it when linking a static library. This is a problem when the
# # static library is later linked into a shared library.
# # Doing it helps some users.
# set_property(TARGET ${library} PROPERTY POSITION_INDEPENDENT_CODE ON)

# Add as many warning as possible:
if (WIN32)
  if (MSVC)
    if (${PROJECT_NAME}_DEV_WARNINGS)
      add_compile_options("/W3")
      add_compile_options("/WX")
    endif()
    add_compile_options("/wd4244")
    add_compile_options("/wd4267")
    add_compile_options("/D_CRT_SECURE_NO_WARNINGS")
  endif()
  # Force Win32 to UNICODE
  add_compile_definitions(UNICODE _UNICODE)
else()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING
      "Flags used by the CXX compiler during DEBUG builds." FORCE)
  if (${PROJECT_NAME}_DEV_WARNINGS)
    add_compile_options("-Wall")
    add_compile_options("-Werror")
    add_compile_options("-Wextra")
    add_compile_options("-Wcast-align")
    add_compile_options("-Wcast-qual")
    add_compile_options("-Wdeprecated")
    add_compile_options("-Wmissing-declarations")
    add_compile_options("-Wnon-virtual-dtor")
    add_compile_options("-Wnull-dereference")
    add_compile_options("-Woverloaded-virtual")
    add_compile_options("-Wpedantic")
    add_compile_options("-Wshadow")
    add_compile_options("-Wunused")
  endif()
  if (${PROJECT_NAME}_VERBOSE_BUILD)
    # compiler stores the preprocessing stage in <source>.ii files
    # compiler stores the linguistic analysis stage in <source>.s files
    add_compile_options("-save-temps=obj")
    # # enable tracking inclusion of header files in compiled sources
    # add_compile_options("-H")
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if(${PROJECT_NAME}_DEV_WARNINGS)
    add_compile_options("-Wdocumentation")
  endif()
endif()

if (${PROJECT_NAME}_VERBOSE_BUILD)
  set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL
"If this value is on, makefiles will be generated without the \
.SILENT directive, and all commands will be echoed to the console \
during the make.  This is useful for debugging only. With Visual \
Studio IDE projects all commands are done without /nologo."
    FORCE)
endif()
