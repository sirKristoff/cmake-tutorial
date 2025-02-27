include(GNUInstallDirs)

#[==[
  Installing logical targets

  default CMAKE_INSTALL_PREFIX = /usr/local of c:/Program Files/${PROJECT_NAME}

  CMAKE_INSTALL_BINDIR = ${CMAKE_INSTALL_PREFIX}/bin
  CMAKE_INSTALL_INCLUDEDIR = ${CMAKE_INSTALL_PREFIX}/include
  CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_PREFIX}/lib

  <output-artifact-configuration>  configuration block syntax:
  <type>  [DESTINATION <dir>]
          [PERMISSION permissions...]
          [CONFIGURATIONS [Debug|Release|...]]
          [COMPONENT <component>]
          [NAMELINK_COMPONENT <component>]
          [OPTIONAL]  [EXCLUDE_FROM_ALL]  [NAMELINK_ONLY | NAMELINK_SKIP]

  <type> may be one of following:
  - ARCHIVE :  static libraries *.a *.lib
  - LIBRARY :  shared libraries *.so
  - RUNTIME :  executables and DLLs
  - OBJECTS :  object files from OBJECT libraries
  - PUBLIC_HEADER, PRIVATE_HEADER, RESOURCE
            :  files specified in the target properties with the same name

  A single install(TARGET) command can have multiple artifact configuration blocks.
  However, be aware that you may only specify one of each type per call.
  That is, if you'd like to configure different destinations of ARCHIVE artifacts for the Debug and
  Release configurations, then you must make two separate install() calls.

  You may also omit the <type> name and specify options for all the artifacts.
#]==]

install(
  # CrcLibrary exported library with crc_* targets assigned to
  TARGETS  crc_obj crc_shared crc_static
  # name of exported library (named export)
  EXPORT CrcLibrary
  # install archive output artifacts for development component-specific installation
  ARCHIVE  COMPONENT development
  # install library output artifacts for runtime component-specific installation
  LIBRARY  COMPONENT runtime
  # public headers will be in DESTINATION dir
  # (PUBLIC_HEADER property may be set separately for each target)
  PUBLIC_HEADER  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/crc
    COMPONENT runtime
)

# crc_console output artifacts will be installed in CMAKE_INSTALL_BINDIR aka bin/
install(
  TARGETS  crc_console
  RUNTIME COMPONENT runtime
)


if (UNIX)
  # run snippet during installation
  # ldconfig triggers dynamic linker to scan trusted directories for new shared libraries
  install(CODE "execute_process(COMMAND ldconfig)"
          COMPONENT runtime
  )
  # ldconfig -p | grep libcrc
endif()


# generate and install a target export file ('CrcLibrary.cmake')
# CrcLibrary must be defined with the install(TARGETS) command
install(EXPORT CrcLibrary
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/crc/cmake
  NAMESPACE Crc::
  COMPONENT runtime
)


#[==[
  install(<FILES|PROGRAMS> files...
          TYPE <type> | DESTINATION <dir>
          ...)

  File Type   | Build-In dir guess | Installation directory variable
  ------------|--------------------|---------------------------------
  BIN         | bin                | CMAKE_INSTALL_BINDIR
  SBIN        | sbin               | CMAKE_INSTALL_SBINDIR
  LIB         | lib                | CMAKE_INSTALL_LIBDIR
  INCLUDE     | include            | CMAKE_INSTALL_INCLUDEDIR
  SYSCONF     | etc                | CMAKE_INSTALL_SYSCONFDIR
  SHAREDSTATE | com                | CMAKE_INSTALL_SHAREDSTATEDIR
  LOCALSTATE  | var                | CMAKE_INSTALL_LOCALSTATEDIR
  RUNSTATE    | $LOCALSTATE/run    | CMAKE_INSTALL_RUNSTATEDIR
  DATA        | $DATAROOT          | CMAKE_INSTALL_DATADIR
  INFO        | $DATAROOT/info     | CMAKE_INSTALL_INFODIR
  LOCALE      | $DATAROOT/locale   | CMAKE_INSTALL_LOCALEDIR
  MAN         | $DATAROOT/man      | CMAKE_INSTALL_MANDIR
  DOC         | $DATAROOT/doc      | CMAKE_INSTALL_DOCDIR

  $LOCALSTATE is CMAKE_INSTALL_LOCALEDIR or defaults to 'var'
  $DATAROOT is CMAKE_INSTALL_DATAROOTDIR or defaults to 'share'
#]==]

# utility module for generating config-file from template
include(CMakePackageConfigHelpers)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/crc)
# generate config-file from template
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CrcConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/CrcConfig.cmake"
  INSTALL_DESTINATION  ${CMAKE_INSTALL_LIBDIR}/crc/cmake
  PATH_VARS
    LIB_INSTALL_DIR
)

# install advanced generated config-file
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/CrcConfig.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/crc/cmake
)
# # install basic config-file CrcConfig.cmake in destination
# install(FILES "cmake/CrcConfig.cmake"
#   DESTINATION ${CMAKE_INSTALL_LIBDIR}/crc/cmake
# )


# utility module for generating package's version files
# <config-file>-version.cmake  or  <config-file>Version.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  # file path to create
  "${CMAKE_CURRENT_BINARY_DIR}/CrcConfigVersion.cmake"

  # # optional explicit version number
  # # instead of provided in project() command
  # VERSION 1.4.2

  #[[
AnyNewerVersion  : will match any older version;
                   <package> on version 1.4.2 will be a good match for
                   find_package(<package> 1.2.8)
SameMajorVersion : matches if the first component is the same (ignores minor and patch)
SameMinorVersion : matches if the first two components are the same (ignores patch)
ExactVersion     : must match all three components of the version and won't
                   support ranged versions: find_package(<package> 1.2.8...1.3.4)
  #]]
  COMPATIBILITY AnyNewerVersion

  # # optionally for skipping same architecture check
  # # (header-only libraries or macro packages)
  # ARCH_INDEPENDENT
)

# install generated package's version file
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/CrcConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/crc/cmake
)


# CPack configuration
set(CPACK_PACKAGE_VENDOR "Krzysztof Lasota")
set(CPACK_PACKAGE_CONTACT "krzysztof.j.lasota@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple Crc")
set(CPACK_PACKAGE_DESCRIPTION "Library for making CRC sum.")
set(CPACK_PACKAGE_NAME "Crc")
# set(CPACK_SYSTEM_NAME "Linux")  # TODO: change name based on environment info
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/packages")
# set(CPACK_GENERATOR "TGZ;ZIP;DEB;MSI")  # list package generators for use

include(CPack)

# TODO: How to set CMAKE_INSTALL_DO_STRIP impacting stripping *.so library
