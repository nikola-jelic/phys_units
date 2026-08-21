#include "phys_string.hpp"
#include <gtest/gtest.h>

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using MeterKilogram =
    units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>, std::ratio<1>>;
using RootMeter = units::PhysicalUnit<double, std::ratio<1>, std::ratio<1, 2>>;
using Scalar = units::PhysicalUnit<int, std::ratio<1>>;
using CubeRootMeter =
    units::PhysicalUnit<double, std::ratio<1>, std::ratio<1, 3>>;
using OddExponent =
    units::PhysicalUnit<double, std::ratio<1>, std::ratio<23, 45>>;
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

/* Assertions about exact rendering pin the style explicitly. Relying on the
   default would make them assertions about the build configuration instead,
   and they would break under -DUNITS_DEFAULT_STYLE. */
template <typename Unit>
static std::string ascii(const Unit &value, int precision = -1) {
  return units::to_string(value, precision, units::UnitStyle::Ascii);
}

/* The numerical value is separated from the unit, as SI itself requires. */
TEST(StringTests, SimpleUnits) {
  EXPECT_EQ(ascii(Meter(3)), "3 m");
  EXPECT_EQ(ascii(MeterSquared(9)), "9 m^2");
}

TEST(StringTests, Prefixes) {
  EXPECT_EQ(ascii(CentiMeter(400)), "400 cm");
  EXPECT_EQ(units::unit_prefix<std::milli>(), "m");
  EXPECT_EQ(units::unit_prefix<std::kilo>(), "k");
  EXPECT_EQ(units::unit_prefix<std::ratio<1>>(), "");
}

/* Every emitted dimension needs a separator before the next one, not just the
   ones that carry an exponent. This used to render as "5mkg". */
TEST(StringTests, SeparatesAdjacentDimensions) {
  EXPECT_EQ(ascii(MeterKilogram(5)), "5 m kg");
}

/* A fractional exponent used to do "/" + Dim::den, which is pointer arithmetic
   on a string literal rather than string concatenation. */
TEST(StringTests, FractionalExponent) {
  EXPECT_EQ(ascii(RootMeter(2.)), "2 m^1/2");
}

/* Negative exponents become a denominator rather than trailing ^-n. */
TEST(StringTests, NegativeExponentsBecomeADenominator) {
  EXPECT_EQ(ascii(Speed(3.5)), "3.5 m/s");
  EXPECT_EQ(ascii(Acceleration(9.81)), "9.81 m/s^2");
  EXPECT_EQ(ascii(Newton(2.)), "2 m kg/s^2");
}

/* With nothing in the numerator the leading one has to be spelled out. */
TEST(StringTests, PureDenominator) { EXPECT_EQ(ascii(Hertz(50.)), "50 1/s"); }

/* "m/s A" would be ambiguous, so a compound denominator is bracketed. */
TEST(StringTests, CompoundDenominatorIsBracketed) {
  EXPECT_EQ(ascii(PerSecondAmpere(1.5)), "1.5 m/(s A)");
}

/* A factor that is not an SI prefix is bracketed and spaced off, so that it
   cannot merge into the value in front of it. This used to read "23/7m". */
TEST(StringTests, UnrecognisedFactorIsDisambiguated) {
  EXPECT_EQ(ascii(OddFactor(2)), "2 (3/7) m");
}

TEST(StringTests, SpecialisedDimensionName) {
  EXPECT_EQ(ascii(Volt(1.)), "1 V");
}

/* A dimension list symbol still composes with an SI prefix. */
TEST(StringTests, SpecialisedNameComposesWithPrefix) {
  EXPECT_EQ(ascii(MilliVolt(230.)), "230 mV");
}

/* std::to_string is fixed at six decimals, so 2.0 used to render as
   "2.000000". The default is now the shortest useful form. */
TEST(StringTests, Precision) {
  EXPECT_EQ(ascii(Acceleration(9.81)), "9.81 m/s^2");
  EXPECT_EQ(ascii(Acceleration(9.81), 1), "9.8 m/s^2");
  EXPECT_EQ(ascii(Acceleration(9.81), 4), "9.8100 m/s^2");
  EXPECT_EQ(ascii(Acceleration(2.), 0), "2 m/s^2");
}

TEST(StringTests, Utf8Superscripts) {
  EXPECT_EQ(units::to_string(MeterSquared(9), -1, units::UnitStyle::Utf8),
            "9 m²");
  EXPECT_EQ(units::to_string(Acceleration(9.81), -1, units::UnitStyle::Utf8),
            "9.81 m/s²");
  /* The plain Utf8 style leaves fractional exponents alone. */
  EXPECT_EQ(units::to_string(RootMeter(2.), -1, units::UnitStyle::Utf8),
            "2 m^1/2");
}

/* Utf8Fractions opts fractional exponents into superscripts as well. */
TEST(StringTests, Utf8Fractions) {
  EXPECT_EQ(
      units::to_string(RootMeter(2.), -1, units::UnitStyle::Utf8Fractions),
      "2 m¹⁄²");
  EXPECT_EQ(
      units::to_string(MeterSquared(9), -1, units::UnitStyle::Utf8Fractions),
      "9 m²");
  EXPECT_EQ(units::unit_symbol<CubeRootMeter>(units::UnitStyle::Utf8Fractions),
            "m¹⁄³");
  /* Multi digit numerators and denominators too. */
  EXPECT_EQ(units::unit_symbol<OddExponent>(units::UnitStyle::Utf8Fractions),
            "m²³⁄⁴⁵");
}

/* The default style is a compile time choice, settable for a whole project
   with -DUNITS_DEFAULT_STYLE. The contract is that omitting the style matches
   passing default_style() explicitly, which holds whatever it is set to; the
   suite therefore passes both with and without the flag. */
TEST(StringTests, DefaultStyleIsHonoured) {
  EXPECT_EQ(units::to_string(RootMeter(2.)),
            units::to_string(RootMeter(2.), -1, units::default_style()));
  EXPECT_EQ(units::unit_symbol<RootMeter>(),
            units::unit_symbol<RootMeter>(units::default_style()));
}

/* The symbol depends only on the type, so no value is needed to ask for it. */
TEST(StringTests, SymbolWithoutAValue) {
  EXPECT_EQ(units::unit_symbol<Speed>(units::UnitStyle::Ascii), "m/s");
  EXPECT_EQ(units::unit_symbol<Meter>(units::UnitStyle::Ascii), "m");
  EXPECT_EQ(units::unit_symbol<Volt>(units::UnitStyle::Ascii), "V");
  EXPECT_EQ(units::unit_symbol<Acceleration>(units::UnitStyle::Utf8), "m/s²");
}

/* A dimensionless value carries no symbol, and no trailing separator either. */
TEST(StringTests, Dimensionless) { EXPECT_EQ(ascii(Scalar(42)), "42"); }

/* The AbsolutePhysicalUnit overload had its template parameters in the wrong
   order and could never match the class. */
TEST(StringTests, AbsoluteUnit) {
  EXPECT_EQ(ascii(TimeStampSeconds(4856)), "4856 s");
}

using OddScalar = units::PhysicalUnit<int, std::ratio<3, 7>>;
using AbsoluteScalar =
    units::AbsolutePhysicalUnit<int, std::ratio<1>, int, std::ratio<0>>;
using BigExponent =
    units::PhysicalUnit<double, std::ratio<1>, std::ratio<106789>>;

/* A dimensionless value with a factor that is not an SI prefix: the factor is
   still bracketed, but with no unit to follow it the trailing separator has to
   be trimmed. */
TEST(StringTests, DimensionlessWithUnrecognisedFactor) {
  EXPECT_EQ(ascii(OddScalar(2)), "2 (3/7)");
}

/* A dimensionless absolute unit carries no symbol either. */
TEST(StringTests, DimensionlessAbsoluteUnit) {
  EXPECT_EQ(ascii(AbsoluteScalar(7)), "7");
}

/* Exercises every superscript digit rather than just the handful that show up
   in realistic exponents. */
TEST(StringTests, AllSuperscriptDigits) {
  EXPECT_EQ(units::unit_symbol<BigExponent>(units::UnitStyle::Utf8), "m¹⁰⁶⁷⁸⁹");
  EXPECT_EQ(units::unit_symbol<BigExponent>(units::UnitStyle::Ascii),
            "m^106789");
}
