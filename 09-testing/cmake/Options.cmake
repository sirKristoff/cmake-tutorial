set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  ## Explicitly require supporting standard by compiler
set(CMAKE_CXX_EXTENSIONS OFF)  ## Disallows using extensions like gnu++14


# Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
# to the library and the library users.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/utf-8")
endif()

# # CMake does automatically add -fPIC when linking a shared library, but it
# # does not add it when linking a static library. This is a problem when the
# # static library is later linked into a shared library.
# # Doing it helps some users.
# set_property(TARGET ${library} PROPERTY POSITION_INDEPENDENT_CODE ON)

# Add as many warning as possible:
if (WIN32)
  if (MSVC)
    if(TUTORIAL_DEV_WARNINGS)
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
  if(TUTORIAL_DEV_WARNINGS)
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
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if(TUTORIAL_DEV_WARNINGS)
    add_compile_options("-Wdocumentation")
  endif()
endif()
