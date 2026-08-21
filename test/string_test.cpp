#include "phys_string.hpp"
#include <gtest/gtest.h>

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using MeterKilogram =
    units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>, std::ratio<1>>;
using RootMeter = units::PhysicalUnit<double, std::ratio<1>, std::ratio<1, 2>>;
using Scalar = units::PhysicalUnit<int, std::ratio<1>>;
using OddFactor = units::PhysicalUnit<int, std::ratio<3, 7>, std::ratio<1>>;

using Hertz = units::PhysicalUnit<double, std::ratio<1>, std::ratio<0>,
                                  std::ratio<0>, std::ratio<-1>>;
using Speed = units::PhysicalUnit<double, std::ratio<1>, std::ratio<1>,
                                  std::ratio<0>, std::ratio<-1>>;
using Acceleration = units::PhysicalUnit<double, std::ratio<1>, std::ratio<1>,
                                         std::ratio<0>, std::ratio<-2>>;
using Newton = units::PhysicalUnit<double, std::ratio<1>, std::ratio<1>,
                                   std::ratio<1>, std::ratio<-2>>;
using PerSecondAmpere =
    units::PhysicalUnit<double, std::ratio<1>, std::ratio<1>, std::ratio<0>,
                        std::ratio<-1>, std::ratio<-1>>;
using Volt = units::PhysicalUnit<double, std::ratio<1>, std::ratio<2>,
                                 std::ratio<1>, std::ratio<-3>, std::ratio<-1>>;
using MilliVolt =
    units::PhysicalUnit<double, std::milli, std::ratio<2>, std::ratio<1>,
                        std::ratio<-3>, std::ratio<-1>>;
using TimeStampSeconds =
    units::AbsolutePhysicalUnit<int, std::ratio<1>, int, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<1>>;

/* The numerical value is separated from the unit, as SI itself requires. */
TEST(StringTests, SimpleUnits) {
  EXPECT_EQ(units::to_string(Meter(3)), "3 m");
  EXPECT_EQ(units::to_string(MeterSquared(9)), "9 m^2");
}

TEST(StringTests, Prefixes) {
  EXPECT_EQ(units::to_string(CentiMeter(400)), "400 cm");
  EXPECT_EQ(units::unit_prefix<std::milli>(), "m");
  EXPECT_EQ(units::unit_prefix<std::kilo>(), "k");
  EXPECT_EQ(units::unit_prefix<std::ratio<1>>(), "");
}

/* Every emitted dimension needs a separator before the next one, not just the
   ones that carry an exponent. This used to render as "5mkg". */
TEST(StringTests, SeparatesAdjacentDimensions) {
  EXPECT_EQ(units::to_string(MeterKilogram(5)), "5 m kg");
}

/* A fractional exponent used to do "/" + Dim::den, which is pointer arithmetic
   on a string literal rather than string concatenation. */
TEST(StringTests, FractionalExponent) {
  EXPECT_EQ(units::to_string(RootMeter(2.)), "2 m^1/2");
}

/* Negative exponents become a denominator rather than trailing ^-n. */
TEST(StringTests, NegativeExponentsBecomeADenominator) {
  EXPECT_EQ(units::to_string(Speed(3.5)), "3.5 m/s");
  EXPECT_EQ(units::to_string(Acceleration(9.81)), "9.81 m/s^2");
  EXPECT_EQ(units::to_string(Newton(2.)), "2 m kg/s^2");
}

/* With nothing in the numerator the leading one has to be spelled out. */
TEST(StringTests, PureDenominator) {
  EXPECT_EQ(units::to_string(Hertz(50.)), "50 1/s");
}

/* "m/s A" would be ambiguous, so a compound denominator is bracketed. */
TEST(StringTests, CompoundDenominatorIsBracketed) {
  EXPECT_EQ(units::to_string(PerSecondAmpere(1.5)), "1.5 m/(s A)");
}

/* A factor that is not an SI prefix is bracketed and spaced off, so that it
   cannot merge into the value in front of it. This used to read "23/7m". */
TEST(StringTests, UnrecognisedFactorIsDisambiguated) {
  EXPECT_EQ(units::to_string(OddFactor(2)), "2 (3/7) m");
}

TEST(StringTests, SpecialisedDimensionName) {
  EXPECT_EQ(units::to_string(Volt(1.)), "1 V");
}

/* A dimension list symbol still composes with an SI prefix. */
TEST(StringTests, SpecialisedNameComposesWithPrefix) {
  EXPECT_EQ(units::to_string(MilliVolt(230.)), "230 mV");
}

/* std::to_string is fixed at six decimals, so 2.0 used to render as
   "2.000000". The default is now the shortest useful form. */
TEST(StringTests, Precision) {
  EXPECT_EQ(units::to_string(Acceleration(9.81)), "9.81 m/s^2");
  EXPECT_EQ(units::to_string(Acceleration(9.81), 1), "9.8 m/s^2");
  EXPECT_EQ(units::to_string(Acceleration(9.81), 4), "9.8100 m/s^2");
  EXPECT_EQ(units::to_string(Acceleration(2.), 0), "2 m/s^2");
}

TEST(StringTests, Utf8Superscripts) {
  EXPECT_EQ(units::to_string(MeterSquared(9), -1, units::UnitStyle::Utf8),
            "9 m²");
  EXPECT_EQ(units::to_string(Acceleration(9.81), -1, units::UnitStyle::Utf8),
            "9.81 m/s²");
  /* A fractional exponent stays in the plain form, being easier to read. */
  EXPECT_EQ(units::to_string(RootMeter(2.), -1, units::UnitStyle::Utf8),
            "2 m^1/2");
}

/* The symbol depends only on the type, so no value is needed to ask for it. */
TEST(StringTests, SymbolWithoutAValue) {
  EXPECT_EQ(units::unit_symbol<Speed>(), "m/s");
  EXPECT_EQ(units::unit_symbol<Meter>(), "m");
  EXPECT_EQ(units::unit_symbol<Volt>(), "V");
  EXPECT_EQ(units::unit_symbol<Acceleration>(units::UnitStyle::Utf8), "m/s²");
}

/* A dimensionless value carries no symbol, and no trailing separator either. */
TEST(StringTests, Dimensionless) {
  EXPECT_EQ(units::to_string(Scalar(42)), "42");
}

/* The AbsolutePhysicalUnit overload had its template parameters in the wrong
   order and could never match the class. */
TEST(StringTests, AbsoluteUnit) {
  EXPECT_EQ(units::to_string(TimeStampSeconds(4856)), "4856 s");
}
