cmake_minimum_required(VERSION 3.14)
project(physical_units_tests)

# googletest 1.14 requires C++14, so that is the floor rather than the library's
# own C++11 baseline; C++11 is covered by the compile probe instead. A plain
# set() here would shadow the cache entry, so the driver could not ask for a
# later standard.
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  phys_unit_test
  basic_test.cpp
  string_test.cpp
  string_odr_test.cpp
  angle_test.cpp
  chrono_test.cpp
  math_test.cpp
)
target_include_directories(
  phys_unit_test
  PRIVATE
  ../inc
)
target_link_libraries(
  phys_unit_test
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(phys_unit_test)
