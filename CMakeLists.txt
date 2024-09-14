cmake_minimum_required(VERSION 3.15...3.30)

project(phys_units)

# Specify the include directory for the library
add_library(phys_units INTERFACE)

target_include_directories(phys_units INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

# C++ standard (optional)
target_compile_features(phys_units INTERFACE cxx_std_11)

# Use `GNUInstallDirs` to handle standard install directories
include(GNUInstallDirs)

# Install the library interface (mostly for CMake package)
install(TARGETS phys_units EXPORT phys_units)

# Install header files
install(DIRECTORY inc/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT phys_units
        FILE phys_units_config.cmake
        NAMESPACE phys_units::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/phys_units
)

# Optionally: Add tests coverage
