#include "phys_units.hpp"
#include <complex>
#include <gtest/gtest.h>
#include <type_traits>

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;

TEST(PhysUnitsTests, BasicOperations) {
  Meter a(3), b(7);
  EXPECT_EQ((a + b).value(), 10);
  EXPECT_EQ((a - b).value(), -4);
  EXPECT_EQ((b - a).value(), 4);
  EXPECT_EQ((a * 4).value(), 12);
  EXPECT_EQ((b / 7).value(), 1);
  EXPECT_EQ((-a).value(), -3);
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(a > b);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a != a);
  EXPECT_TRUE(b >= a);
  EXPECT_TRUE(b > a);
}

TEST(PhysUnitsTests, SimpleConversions) {
  Meter meters(4);
  CentiMeter centimeters(meters);
  EXPECT_EQ(centimeters.value(), 400);
  centimeters = CentiMeter(1300);
  meters = units::unit_cast<Meter>(centimeters);
  EXPECT_EQ(meters.value(), 13);
}

using Seconds = units::PhysicalUnit<int, std::ratio<1>, std::ratio<0>,
                                    std::ratio<0>, std::ratio<1>>;
using Herz = units::PhysicalUnit<double, std::ratio<1>, std::ratio<0>,
                                 std::ratio<0>, std::ratio<-1>>;

TEST(PhysUnitsTests, ComplexConversions) {
  Meter a(3), b(6);
  MeterSquared c = a * b;
  EXPECT_EQ(c.value(), 18);
  EXPECT_EQ((c / a), b);
  EXPECT_EQ((b / a), 2);
  EXPECT_EQ((b / 2).value(), 3);
  Seconds s(20);
  Herz h = 1. / s;
  EXPECT_NEAR(h.value(), 0.05, 0.000001);
  EXPECT_NEAR(s * h, 1.0, 0.0000001);
}

TEST(PhysUnitsTests, SupportFunctions) {
  Meter meters(-14);
  EXPECT_EQ(units::abs(meters).value(), 14);
  EXPECT_EQ(units::fmod(14, 4), 2);
  static_assert(std::is_same<decltype(units::fmod(14, 4)), int>::value,
                "Type must be an integer");
}

using TimeStampSeconds =
    units::AbsolutePhysicalUnit<int, std::ratio<1>, int, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<1>>;
using TimeStampMilliSeconds =
    units::AbsolutePhysicalUnit<int64_t, std::milli, int, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<1>>;
TEST(AbsPhysUnitsTests, BasicOperations) {
  TimeStampSeconds ts1(5000), ts2(4856);
  Seconds interval = ts1 - ts2;
  EXPECT_EQ(interval.value(), 144);
  EXPECT_EQ(ts2 + interval, ts1);
  EXPECT_EQ(ts1 - interval, ts2);
  EXPECT_TRUE(ts1 == ts1);
  EXPECT_FALSE(ts1 == ts2);
  EXPECT_TRUE(ts1 != ts2);
  EXPECT_FALSE(ts1 < ts2);
  EXPECT_FALSE(ts1 <= ts2);
  EXPECT_TRUE(ts1 > ts2);
  EXPECT_TRUE(ts1 >= ts2);
  TimeStampMilliSeconds tsm = units::unit_cast<TimeStampMilliSeconds>(ts2);
  EXPECT_EQ(tsm.value(), 4856000);
}

using TempKelvin =
    units::AbsolutePhysicalUnit<double, std::ratio<1>, double, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                std::ratio<0>, std::ratio<1>>;
using TempCelsius = units::AbsolutePhysicalUnit<
    double, std::ratio<1>, double, std::ratio<27315, 100>, std::ratio<0>,
    std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
using TempFahrenheit = units::AbsolutePhysicalUnit<
    double, std::ratio<5, 9>, double, std::ratio<229835, 900>, std::ratio<0>,
    std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

TEST(TemperatureTests, ZeroKelvins) {
  TempKelvin k(0);
  TempCelsius c(k);
  TempFahrenheit f1(k), f2(c);
  EXPECT_NEAR(k.value(), 0., 0.000001);
  EXPECT_NEAR(c.value(), -273.15, 0.000001);
  EXPECT_NEAR(f1.value(), f2.value(), 0.000001);
  EXPECT_NEAR(f1.value(), -459.67, 0.000001);
}

TEST(TemperatureTests, ZeroCelsius) {
  TempCelsius c(0);
  TempFahrenheit f(c);
  TempKelvin k1(c), k2(f);
  EXPECT_NEAR(c.value(), 0., 0.000001);
  EXPECT_NEAR(f.value(), 32., 0.000001);
  EXPECT_NEAR(k1.value(), k2.value(), 0.000001);
  EXPECT_NEAR(k1.value(), 273.15, 0.000001);
}

TEST(TemperatureTests, ZeroFahrenheit) {
  TempFahrenheit f(0);
  TempKelvin k(f);
  TempCelsius c1(f), c2(k);
  EXPECT_NEAR(f.value(), 0., 0.000001);
  EXPECT_NEAR(k.value(), 255.37, 0.01);
  EXPECT_NEAR(c1.value(), c2.value(), 0.00001);
  EXPECT_NEAR(c1.value(), -17.78, 0.01);
}

TEST(ComplexTests, SimpleArithmetics) {
  units::PhysicalUnit<std::complex<double>, std::ratio<1>, std::ratio<0>,
                      std::ratio<0>, std::ratio<0>, std::ratio<1>>
      electricity(std::complex<double>(1., 0.5));
  EXPECT_TRUE(electricity == electricity);
  EXPECT_FALSE(electricity != electricity);
  electricity *= std::complex<double>(0.5, 1);
  EXPECT_NEAR(electricity.value().real(), 0, 0.00001);
  EXPECT_NEAR(electricity.value().imag(), 1.25, 0.00001);
  EXPECT_NEAR(std::abs(electricity.value()), 1.25, 0.00001);
  EXPECT_NEAR(std::arg(electricity.value()), M_PI / 2, 0.00001);
}

/* The dimensionless specialisation carried neither value() nor the ValueType
   and ConvFactor aliases, so the generic operators could not instantiate
   against it and two dimensionless results could not be compared. */
using Scalar = units::PhysicalUnit<int, std::ratio<1>>;

TEST(PhysUnitsTests, DimensionlessResults) {
  Meter a(6), b(3);
  auto ratio = a / b;
  static_assert(std::is_same<decltype(ratio)::ValueType, int>::value,
                "Dimensionless results must expose ValueType");

  EXPECT_EQ(ratio.value(), 2);
  EXPECT_TRUE(ratio == Scalar(2));
  EXPECT_TRUE(ratio != Scalar(3));
  EXPECT_TRUE(ratio > Scalar(1));
  EXPECT_TRUE(ratio <= Scalar(2));

  /* The documented implicit decay to the underlying type still works. */
  int plain = ratio;
  EXPECT_EQ(plain, 2);
}

/* Paths that no test instantiated. An uninstantiated template is unparsed
   rather than merely untested, and coverage tools cannot see it at all, which
   is how several broken operators survived here for years. */
TEST(PhysUnitsTests, IncrementAndCompoundAssignment) {
  Meter m(3);
  ++m;
  EXPECT_EQ(m.value(), 4);
  --m;
  EXPECT_EQ(m.value(), 3);
  m *= 4;
  EXPECT_EQ(m.value(), 12);
  m /= 3;
  EXPECT_EQ(m.value(), 4);
}

TEST(PhysUnitsTests, IntegralModulo) {
  Meter a(7), b(4);
  EXPECT_EQ(a % b, 3);
}

TEST(PhysUnitsTests, ScalarOnTheLeft) {
  Meter m(3);
  EXPECT_EQ((2 * m).value(), 6);
  EXPECT_EQ((m * 2).value(), 6);

  /* Dividing a scalar by a unit inverts its dimensions. */
  Seconds s(2);
  auto hz = 1 / s;
  static_assert(std::is_same<decltype(hz), Herz::ValueType>::value == false,
                "Dividing a scalar by a unit yields a unit, not a scalar");
  EXPECT_EQ(hz.value(), 0);
}

TEST(AbsPhysUnitsTests, IncrementAndReversedAddition) {
  TimeStampSeconds ts(5000);
  ++ts;
  EXPECT_EQ(ts.value(), 5001);
  --ts;
  EXPECT_EQ(ts.value(), 5000);

  /* The free operator+ with the interval on the left. Its first parameter is
     a non-deduced context, so the type comes from the absolute unit. */
  Seconds interval(144);
  TimeStampSeconds moved = interval + ts;
  EXPECT_EQ(moved.value(), 5144);
}

TEST(AbsPhysUnitsTests, AbsoluteValue) {
  TimeStampSeconds ts(-5);
  EXPECT_EQ(units::abs(ts).value(), 5);
  EXPECT_EQ(units::abs(Meter(-14)).value(), 14);
}
