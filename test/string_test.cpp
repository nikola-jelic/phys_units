#include "phys_string.hpp"
#include <gtest/gtest.h>

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using MeterKilogram =
    units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>, std::ratio<1>>;
using RootMeter = units::PhysicalUnit<double, std::ratio<1>, std::ratio<1, 2>>;
using Volt = units::PhysicalUnit<double, std::ratio<1>, std::ratio<2>,
                                 std::ratio<1>, std::ratio<-3>, std::ratio<-1>>;
using TimeStampSeconds =
    units::AbsolutePhysicalUnit<int, std::ratio<1>, int, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<1>>;

TEST(StringTests, SimpleUnits) {
  EXPECT_EQ(units::to_string(Meter(3)), "3m");
  EXPECT_EQ(units::to_string(MeterSquared(9)), "9m^2");
}

TEST(StringTests, Prefixes) {
  EXPECT_EQ(units::to_string(CentiMeter(400)), "400cm");
  EXPECT_EQ(units::unit_prefix<std::milli>(), "m");
  EXPECT_EQ(units::unit_prefix<std::kilo>(), "k");
  EXPECT_EQ(units::unit_prefix<std::ratio<1>>(), "");
}

/* Every emitted dimension needs a separator before the next one, not just the
   ones that carry an exponent. This used to render as "5mkg". */
TEST(StringTests, SeparatesAdjacentDimensions) {
  EXPECT_EQ(units::to_string(MeterKilogram(5)), "5m kg");
}

/* A fractional exponent used to do "/" + Dim::den, which is pointer arithmetic
   on a string literal rather than string concatenation. */
TEST(StringTests, FractionalExponent) {
  EXPECT_EQ(units::to_string(RootMeter(2.)), "2.000000m^1/2");
}

TEST(StringTests, SpecialisedDimensionName) {
  EXPECT_EQ(units::to_string(Volt(1.)), "1.000000V");
}

/* The AbsolutePhysicalUnit overload had its template parameters in the wrong
   order and could never match the class. */
TEST(StringTests, AbsoluteUnit) {
  EXPECT_EQ(units::to_string(TimeStampSeconds(4856)), "4856s");
}
