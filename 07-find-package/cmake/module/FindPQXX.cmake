#[[
Find module for PQXX library.

To let CMake `find_package()` discover this find-module,
"${PROJECT_ROOT}/cmake/module" path has to be appended to
`CMAKE_MODULE_PATH` variable in `CMakeLists.txt`.

```cmake
list(APPEND  CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/module/")
find_package(PQXX REQUIRED)
```

In scope of this file folowing variables will be set by CMake:

* `CMAKE_FIND_PACKAGE_NAME == "PQXX"`
* `PQXX_FIND_REQUIRED == 1`  \
    If `find_package(PQXX REQUIRED)` is called.  \
    `message(FATAL_ERROR)` should be called if a library is not found.
* `PQXX_FIND_QUIETLY == 1`  \
    If `find_package(PQXX QUIET)` is called.  \
    Printing diagnostic messages should be avoided.
* `PQXX_FIND_VERSION`  set to version required by calling the list file.  \
    A find-module should find the appropriate version or issue `FATAL_ERROR`.
#]]


macro(message_not_quietly text)
  if(NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    message(AUTHOR_WARNING ${text})
  endif()
endmacro()


if (NOT (PostgreSQL_LIBRARIES AND PostgreSQL_INCLUDE_DIR) )
  include(CMakeFindDependencyMacro)
  #[[
  Find the library and headers of the nested dependency - PostgreSQL.

  find_dependency() macro call return() if the dependency is not found!
  REQUIRED and QUIET flags are forwarded to nested search.
  POSTGRES_LIBRARIES and POSTGRES_INCLUDE_DIRECTORIES are macro output what is specific for PostgreSQL.
  #]]
  find_dependency(PostgreSQL)
endif()

message_not_quietly("add_library(PQXX::PostgreSQL)")
add_library(PQXX::PostgreSQL
  UNKNOWN  # allows the path to an imported library
          # to be used without having to know what type of library it is.
  IMPORTED   # References a library file located outside the project.
)
message_not_quietly("set_target_properties(PQXX::PostgreSQL IMPORTED_LOCATION ${PostgreSQL_LIBRARIES})")  # TODO: cleanup
set_target_properties(PQXX::PostgreSQL
  PROPERTIES
    IMPORTED_LOCATION  ${PostgreSQL_LIBRARIES}    # specifies the location of the main library file on disk
)
message_not_quietly("target_include_directories(PQXX::PostgreSQL ${PostgreSQL_INCLUDE_DIR})")  # TODO: cleanup
target_include_directories(PQXX::PostgreSQL
  INTERFACE ${PostgreSQL_INCLUDE_DIR}
)




function(add_imported_library  library  headers)
  message_not_quietly("add_library(PQXX::PQXX)")
  add_library(PQXX::PQXX
    UNKNOWN  # allows the path to an imported library
            # to be used without having to know what type of library it is.
    IMPORTED   # References a library file located outside the project.
  )
  message(AUTHOR_WARNING "set_target_properties(PQXX::PQXX IMPORTED_LOCATION ${library})")  # TODO: cleanup
  set_target_properties(PQXX::PQXX
    PROPERTIES
      IMPORTED_LOCATION  ${library}    # specifies the location of the main library file on disk
      # INTERFACE_INCLUDE_DIRECTORIES  ${headers}
  )
  target_link_libraries(PQXX::PQXX
    INTERFACE
      # ${library}
      PQXX::PostgreSQL
  )
  message(AUTHOR_WARNING "target_include_directories(PQXX::PQXX ${headers})")  # TODO: cleanup
  target_include_directories(PQXX::PQXX
    INTERFACE ${headers}
  )
  set(PQXX_FOUND  1  # it will indicate in global scope that PQXX was found
      CACHE INTERNAL "PQXX found" FORCE)
  set(PQXX_LIBRARIES  ${library}  # stores in cache to do not perform search again
      CACHE FILEPATH "Path to pqxx library" FORCE)
  set(PQXX_INCLUDES  ${headers}
      CACHE PATH "Path to pqxx headers" FORCE)
  # advanced variables are not visible in the CMake GUI unless the "advanced" option is enabled
  mark_as_advanced(FORCE  PQXX_LIBRARIES)
  mark_as_advanced(FORCE  PQXX_INCLUDES)
endfunction()


#[[ 
If paths to library and headers are known
(either provided by a user or comming from the cache of a previous run),
use these paths and create an `IMPORTED` target.
User may provide paths through the command line, with -D arguments.
#]]
if (PQXX_LIBRARIES AND PQXX_INCLUDES)
  add_imported_library("${PQXX_LIBRARIES}" "${PQXX_INCLUDES}")
  return()
endif()


#[[
Environment variable PQXX_DIR will be additional path to search for PQXX library.

Converts path(s) in PQXX_DIR env variable to CMake-style:
* path with forward-slashes (/)
* a search path will be converted to a cmake-style list separated by ; characters
The input can be a single path or a system search path like $ENV{PATH}.
#]]
file(TO_CMAKE_PATH "$ENV{PQXX_DIR}" _PQXX_DIR)

find_library(PQXX_LIBRARY_PATH  # if file is found, its path will be in PQXX_LIBRARY_PATH
  NAMES  # library binaries have to match provided names (extensions will be platform-specific)
    libpqxx  pqxx
  PATHS  # list of paths to search for library binaries
    ${_PQXX_DIR}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/local/pgsql/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    ${_PQXX_DIR}/lib
    ${_PQXX_DIR}
    ${CMAKE_INSTALL_PREFIX}/lib
    ${CMAKE_INSTALL_PREFIX}/bin
    /usr/local/pgsql/lib
    /usr/local/lib
    /usr/lib
  NO_DEFAULT_PATH  # disable scanning a long list of default paths provided by CMake for host environment
  DOC "Found path to pqxx library"
)

# gets path to directory containing header file otherwise PQXX_HEADER_PATH-NOTFOUND
find_path(PQXX_HEADER_PATH  # if header(s) is found, PQXX_HEADER_PATH contains directory path
  NAMES pqxx/pqxx  # exact name of header file we search for
  PATHS  # list of paths to search for library binaries
    ${_PQXX_DIR}/include
    ${_PQXX_DIR}
    ${CMAKE_INSTALL_PREFIX}/include
    /usr/local/pgsql/include
    /usr/local/include
    /usr/include
  NO_DEFAULT_PATH
  DOC "Found path to pqxx headers"
)

include(FindPackageHandleStandardArgs)
#[[
This command handles the `REQUIRED`, `QUIET` and version-related arguments of `find_package()`.
It also sets the `<PackageName>_FOUND`` variable.
The package is considered found if all variables listed contain valid results, e.g. valid filepaths.
#]]
find_package_handle_standard_args(
  PQXX  # package name
  REQUIRED_VARS  # variables which are required for this package
    PQXX_LIBRARY_PATH
    PQXX_HEADER_PATH
  # FAIL_MESSAGE <custom-failure-message>  # text for custom-failure-message - instead of using the default message
)
#[[
# optional function signatures
find_package_handle_standard_args(
  PQXX  # Package name
  DEFAULT_MSG  # default message should be used (text for custom-failure-message may be used instead)
  PQXX_LIBRARY_PATH
  PQXX_HEADER_PATH
)
#]]

# Check whether a PQXX library and its headers were found
if (PQXX_FOUND)
  # Created IMPORTED target with PQXX and PortgreSQL libraries and headers
  add_imported_library(
    "${PQXX_LIBRARY_PATH}"
    "${PQXX_HEADER_PATH}"
  )
endif()
