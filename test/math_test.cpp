#include "phys_math.hpp"
#include <gtest/gtest.h>

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using MeterCubed = units::PhysicalUnit<int, std::ratio<1>, std::ratio<3>>;

TEST(CommonMathTests, SimpleRoots) {
  Meter m(7);
  MeterSquared ms = m * m;
  MeterCubed mc = ms * m;
  EXPECT_EQ(ms.value(), 49);
  EXPECT_EQ(mc.value(), 343);
  auto m_new = std::sqrt(ms);
  static_assert(
      std::is_same<decltype(m_new), units::PhysicalUnit<double, std::ratio<1>,
                                                        std::ratio<1>>>::value,
      "Type must be a meter");
  EXPECT_EQ(m_new.value(), 7);
  m_new = std::cbrt(mc);
  EXPECT_EQ(m_new.value(), 7);
}

using DeciMeter = units::PhysicalUnit<int, std::ratio<1, 10>, std::ratio<1>>;
using DeciMeterSquared =
    units::PhysicalUnit<int, std::ratio<1, 100>, std::ratio<2>>;
using DeciMeterCubed =
    units::PhysicalUnit<int, std::ratio<1, 1000>, std::ratio<3>>;

TEST(CommonMathTests, FactoredRoots) {
  DeciMeter m(3);
  DeciMeterSquared ms = m * m;
  DeciMeterCubed mc = ms * m;
  EXPECT_EQ(ms.value(), 9);
  EXPECT_EQ(mc.value(), 27);
  auto m_new = std::sqrt(ms);
  static_assert(
      std::is_same<
          decltype(m_new),
          units::PhysicalUnit<double, std::ratio<1, 10>, std::ratio<1>>>::value,
      "Type must be a meter");
  EXPECT_EQ(m_new.value(), 3);
  m_new = std::cbrt(mc);
  EXPECT_NEAR(m_new.value(), 3, 0.00001);
}
