cmake_minimum_required(VERSION 3.15...3.30)

project(phys_units)

# Specify the include directory for the library
add_library(phys_units INTERFACE)

target_include_directories(phys_units INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<INSTALL_INTERFACE:inc>)

# C++ standard (optional)
target_compile_features(phys_units INTERFACE cxx_std_11)

# Use `GNUInstallDirs` to handle standard install directories
include(GNUInstallDirs)

# Configure installation
install(TARGETS phys_units EXPORT phys_units_config)

# Install only .hpp files
install(
  DIRECTORY inc/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp")

install(
  EXPORT phys_units_config
  FILE phys_units_config.cmake
  NAMESPACE phys_units::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/phys_units)

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

