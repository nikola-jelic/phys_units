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
  auto m_new = units::sqrt(ms);
  static_assert(
      std::is_same<decltype(m_new), units::PhysicalUnit<double, std::ratio<1>,
                                                        std::ratio<1>>>::value,
      "Type must be a meter");
  EXPECT_EQ(m_new.value(), 7);
  m_new = units::cbrt(mc);
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
  auto m_new = units::sqrt(ms);
  static_assert(
      std::is_same<
          decltype(m_new),
          units::PhysicalUnit<double, std::ratio<1, 10>, std::ratio<1>>>::value,
      "Type must be a meter");
  EXPECT_EQ(m_new.value(), 3);
  m_new = units::cbrt(mc);
  EXPECT_NEAR(m_new.value(), 3, 0.00001);
}

/* num_sqrt<360> used to claim 60, which is the square root of 3600, so any
   sqrt() over a factor of 360 came out 3.16x too large. 360 has no integer
   square root, so the table now carries 3600 instead. */
using SixtySecondUnit =
    units::PhysicalUnit<double, std::ratio<3600>, std::ratio<2>>;
using SixtySecondRoot =
    units::PhysicalUnit<double, std::ratio<60>, std::ratio<1>>;

TEST(CommonMathTests, TabulatedRootsAreCorrect) {
  static_assert(units::num_sqrt<3600>::value == 60, "sqrt(3600) is 60");
  static_assert(units::num_sqrt<100000000>::value == 10000, "sqrt(1e8) is 1e4");
  static_assert(units::num_cbrt<1000000000>::value == 1000, "cbrt(1e9) is 1e3");

  SixtySecondUnit area(4.);
  auto side = units::sqrt(area);
  static_assert(std::is_same<decltype(side), SixtySecondRoot>::value,
                "Factor 3600 must root to factor 60");
  EXPECT_NEAR(side.value(), 2., 0.000001);
}
