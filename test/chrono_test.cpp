#include "phys_chrono.hpp"
#include <chrono>
#include <gtest/gtest.h>

template <typename V, typename F>
using PhysTime =
    units::PhysicalUnit<V, F, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

TEST(CommonChronoTests, BasicConversions) {
  PhysTime<int, std::ratio<1>> seconds(1);
  std::chrono::duration<int> ch_seconds = std::chrono::duration_cast(seconds);
  EXPECT_EQ(seconds.value(), ch_seconds.count());
  PhysTime<int, std::ratio<1>> other_seconds = units::unit_cast(ch_seconds);
  EXPECT_EQ(other_seconds.value(), ch_seconds.count());
}
