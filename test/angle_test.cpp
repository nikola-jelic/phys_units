#include "phys_angle.hpp"
#include <gtest/gtest.h>

using AngleInDegrees = units::PhysicalUnitAngle<int, std::ratio<1>>;
using AngleInMinutes = units::PhysicalUnitAngle<int, std::ratio<1, 60>>;
using AngleInSeconds = units::PhysicalUnitAngle<int, std::ratio<1, 3600>>;
using AngleInRadians = units::PhysicalUnitAngle<double, units::RadianRatio>;
using AngleInUnits = units::PhysicalUnitAngle<double, std::ratio<360>>;

TEST(CommonAngleTests, BasicOperations) {
  AngleInDegrees a(60), b(30);
  EXPECT_EQ((a + b).value(), 90);
  EXPECT_EQ((a - b).value(), 30);
  EXPECT_EQ((b - a).value(), -30);
  EXPECT_EQ((a * 4).value(), 240);
  EXPECT_EQ((b / 6).value(), 5);
  EXPECT_EQ((-a).value(), -60);
  EXPECT_TRUE(a > b);
  EXPECT_FALSE(a < b);
  EXPECT_TRUE(a >= b);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a != a);
  EXPECT_TRUE(b <= a);
  EXPECT_TRUE(b < a);
}

TEST(CommonAngleTests, SimpleConversions) {
  AngleInDegrees a(90);
  AngleInMinutes am(a);
  AngleInSeconds as(am);
  EXPECT_EQ(am.value(), 5400);
  EXPECT_EQ(as.value(), 324000);
  AngleInUnits au(a);
  EXPECT_NEAR(au.value(), 0.25, 0.001);
}

TEST(CommonAngleTests, AngleNormalisationPositiveFullInterval) {
  units::AbsoluteAngle<int> shared(45);
  EXPECT_EQ(shared.value(), 45);
  shared = units::AbsoluteAngle<int>(180);
  EXPECT_EQ(shared.value(), 180);
  shared = units::AbsoluteAngle<int>(270);
  EXPECT_EQ(shared.value(), 270);
  shared = units::AbsoluteAngle<int>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<int>(390);
  EXPECT_EQ(shared.value(), 30);
}

TEST(CommonAngleTests, AngleNormalisationNegativeFullInterval) {
  units::AbsoluteAngle<int> shared(-30);
  EXPECT_EQ(shared.value(), 330);
  shared = units::AbsoluteAngle<int>(180);
  EXPECT_EQ(shared.value(), 180);
  shared = units::AbsoluteAngle<int>(-90);
  EXPECT_EQ(shared.value(), 270);
  shared = units::AbsoluteAngle<int>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<int>(-270);
  EXPECT_EQ(shared.value(), 90);
}

TEST(CommonAngleTests, AngleNormalisationPositiveHalfInterval) {
  units::AbsoluteAngle<int, std::ratio<1>, units::AngleRange::Signed> shared(
      45);
  EXPECT_EQ(shared.value(), 45);
  shared = units::AbsoluteAngle<int>(180);
  EXPECT_EQ(shared.value(), -180);
  shared = units::AbsoluteAngle<int>(270);
  EXPECT_EQ(shared.value(), -90);
  shared = units::AbsoluteAngle<int>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<int>(390);
  EXPECT_EQ(shared.value(), 30);
}

TEST(CommonAngleTests, AngleNormalisationNegativeHalfInterval) {
  units::AbsoluteAngle<int, std::ratio<1>, units::AngleRange::Signed> shared(
      -30);
  EXPECT_EQ(shared.value(), -30);
  shared = units::AbsoluteAngle<int>(180);
  EXPECT_EQ(shared.value(), -180);
  shared = units::AbsoluteAngle<int>(-90);
  EXPECT_EQ(shared.value(), -90);
  shared = units::AbsoluteAngle<int>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<int>(-270);
  EXPECT_EQ(shared.value(), 90);
}

TEST(CommonAngleTests, AngleNormalisationPositiveFullIntervalFloat) {
  units::AbsoluteAngle<double> shared(45.);
  EXPECT_EQ(shared.value(), 45.);
  shared = units::AbsoluteAngle<double>(180.);
  EXPECT_EQ(shared.value(), 180.);
  shared = units::AbsoluteAngle<double>(270.);
  EXPECT_EQ(shared.value(), 270.);
  shared = units::AbsoluteAngle<double>(360.);
  EXPECT_EQ(shared.value(), 0.);
  shared = units::AbsoluteAngle<double>(390.);
  EXPECT_EQ(shared.value(), 30.);
}

TEST(CommonAngleTests, AngleNormalisationNegativeFullIntervalFloat) {
  units::AbsoluteAngle<double> shared(-30);
  EXPECT_EQ(shared.value(), 330);
  shared = units::AbsoluteAngle<double>(180);
  EXPECT_EQ(shared.value(), 180);
  shared = units::AbsoluteAngle<double>(-90);
  EXPECT_EQ(shared.value(), 270);
  shared = units::AbsoluteAngle<double>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<double>(-270);
  EXPECT_EQ(shared.value(), 90);
}

TEST(CommonAngleTests, AngleNormalisationPositiveHalfIntervalFloat) {
  units::AbsoluteAngle<double, std::ratio<1>, units::AngleRange::Signed> shared(
      45);
  EXPECT_EQ(shared.value(), 45);
  shared = units::AbsoluteAngle<double>(180);
  EXPECT_EQ(shared.value(), -180);
  shared = units::AbsoluteAngle<double>(270);
  EXPECT_EQ(shared.value(), -90);
  shared = units::AbsoluteAngle<double>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<double>(390);
  EXPECT_EQ(shared.value(), 30);
}

TEST(CommonAngleTests, AngleNormalisationNegativeHalfIntervalFloat) {
  units::AbsoluteAngle<double, std::ratio<1>, units::AngleRange::Signed> shared(
      -30);
  EXPECT_EQ(shared.value(), -30);
  shared = units::AbsoluteAngle<double>(180);
  EXPECT_EQ(shared.value(), -180);
  shared = units::AbsoluteAngle<double>(-90);
  EXPECT_EQ(shared.value(), -90);
  shared = units::AbsoluteAngle<double>(360);
  EXPECT_EQ(shared.value(), 0);
  shared = units::AbsoluteAngle<double>(-270);
  EXPECT_EQ(shared.value(), 90);
}

using Direction = units::AbsoluteAngle<int, std::ratio<1>>;
using HalfDirection =
    units::AbsoluteAngle<int, std::ratio<1>, units::AngleRange::Signed>;
using HalfDirectionFloat =
    units::AbsoluteAngle<double, std::ratio<1>, units::AngleRange::Signed>;

/* AbsoluteAngle's own arithmetic operators referenced the member function
   `value` instead of calling it, so none of them had ever compiled. */
TEST(CommonAngleTests, AbsoluteAngleArithmetics) {
  Direction d(350);
  EXPECT_EQ((d + Direction::DiffAngleUnit(20)).value(), 10);
  EXPECT_EQ((d - Direction::DiffAngleUnit(20)).value(), 330);
  Direction low(10);
  EXPECT_EQ((low - Direction::DiffAngleUnit(20)).value(), 350);

  /* The difference between two directions is an angle, so it keeps its sign
     and is not folded back into the circle. */
  EXPECT_EQ((d - low).value(), 340);
  EXPECT_EQ((low - d).value(), -340);
}

TEST(CommonAngleTests, AbsoluteAngleCompoundAssignment) {
  Direction d(350);
  d += Direction::DiffAngleUnit(20);
  EXPECT_EQ(d.value(), 10);
  d -= Direction::DiffAngleUnit(20);
  EXPECT_EQ(d.value(), 350);
}

/* Half interval normalisation only folded values at or above +circle/2; those
   below -circle/2 were left outside the documented range. */
TEST(CommonAngleTests, HalfIntervalNormalisesBelowLowerBound) {
  EXPECT_EQ(HalfDirection(-200).value(), 160);
  EXPECT_EQ(HalfDirection(-190).value(), 170);
  EXPECT_EQ(HalfDirection(-180).value(), -180);
  EXPECT_EQ(HalfDirection(179).value(), 179);
  EXPECT_EQ(HalfDirection(180).value(), -180);
  EXPECT_EQ(HalfDirectionFloat(-200.).value(), 160.);
}

/* Trigonometry now resolves through ADL in namespace units rather than
   through overloads injected into namespace std. */
TEST(CommonAngleTests, Trigonometry) {
  using std::cos;
  using std::sin;
  EXPECT_NEAR(sin(AngleInDegrees(30)), 0.5, 0.000001);
  EXPECT_NEAR(cos(AngleInDegrees(60)), 0.5, 0.000001);
  EXPECT_NEAR(sin(units::AbsoluteAngle<double>(30.)), 0.5, 0.000001);
}

TEST(CommonAngleTests, InverseTrigonometry) {
  using Radians = units::AbsoluteAngle<double, std::ratio<1>>;
  EXPECT_NEAR(units::asin<Radians>(0.5).value(), 30., 0.000001);
  EXPECT_NEAR(units::acos<Radians>(0.5).value(), 60., 0.000001);
  EXPECT_NEAR(units::atan2<Radians>(1., 1.).value(), 45., 0.000001);
  EXPECT_NEAR(units::atan<AngleInRadians>(1.).value(), 0.785398, 0.000001);
}

/* abs() on an AbsoluteAngle could never be deduced: the range is a non-type
   parameter and the signature used a typename pack, which cannot bind to it. */
TEST(CommonAngleTests, AbsoluteValue) {
  EXPECT_NEAR(units::abs(units::AbsoluteAngle<double>(30.)).value(), 30.,
              0.000001);
  HalfDirectionFloat mirrored(-90.);
  EXPECT_NEAR(units::abs(mirrored).value(), 90., 0.000001);
}

TEST(CommonAngleTests, HyperbolicTrigonometry) {
  using std::sinh;
  using std::tanh;
  /* 0 degrees is 0 radians, so the hyperbolic functions are at their origin. */
  EXPECT_NEAR(sinh(AngleInDegrees(0)), 0., 0.000001);
  EXPECT_NEAR(tanh(units::AbsoluteAngle<double>(0.)), 0., 0.000001);
  EXPECT_NEAR(units::cosh(AngleInDegrees(0)), 1., 0.000001);
}

TEST(CommonAngleTests, InverseHyperbolicTrigonometry) {
  using Radians = units::AbsoluteAngle<double, std::ratio<1>>;
  EXPECT_NEAR(units::asinh<Radians>(0.).value(), 0., 0.000001);
  EXPECT_NEAR(units::atanh<Radians>(0.).value(), 0., 0.000001);
  EXPECT_NEAR(units::acosh<Radians>(1.).value(), 0., 0.000001);
}

/* Encoder counts. One count is 360/N degrees, which makes the full circle come
   out as exactly N, so a direction normalises straight onto the raw count range
   and wraparound is a single integer modulo, with no floating point involved.
 */
template <int N>
using EncoderCount = units::AbsoluteAngle<int, std::ratio<360, N>>;
template <int N>
using SignedEncoderCount =
    units::AbsoluteAngle<int, std::ratio<360, N>, units::AngleRange::Signed>;

TEST(EncoderAngleTests, FullCircleIsExactlyTheCountPerRevolution) {
  static_assert(units::FullCircle<std::ratio<360, 4096>>::num == 4096, "4096");
  static_assert(units::FullCircle<std::ratio<360, 4096>>::den == 1, "exact");
  static_assert(units::FullCircle<std::ratio<360, 999>>::num == 999, "999");
  static_assert(units::FullCircle<std::ratio<360, 999>>::den == 1, "exact");

  EXPECT_EQ(EncoderCount<4096>(4100).value(), 4);
  EXPECT_EQ(EncoderCount<4096>(-5).value(), 4091);
  EXPECT_EQ(EncoderCount<4096>(4096).value(), 0);
}

/* With an odd count per revolution, circle/2 truncates. The two guards used to
   overlap by one value, so normalising twice disagreed with normalising once
   and adding a zero interval moved the direction. */
TEST(EncoderAngleTests, OddCountNormalisationIsStable) {
  for (int v = -3 * 4095; v <= 3 * 4095; ++v) {
    const int once = SignedEncoderCount<4095>(v).value();
    EXPECT_EQ(SignedEncoderCount<4095>(once).value(), once)
        << "normalising twice disagreed for " << v;
    EXPECT_GE(once, -2047);
    EXPECT_LE(once, 2047);

    SignedEncoderCount<4095> moved(v);
    moved += SignedEncoderCount<4095>::DiffAngleUnit(0);
    EXPECT_EQ(moved.value(), once) << "adding zero moved " << v;
  }
}

/* The even case, which was already correct, keeps its half-open interval. */
TEST(EncoderAngleTests, EvenCountKeepsHalfOpenInterval) {
  EXPECT_EQ(SignedEncoderCount<4096>(2047).value(), 2047);
  EXPECT_EQ(SignedEncoderCount<4096>(2048).value(), -2048);
  EXPECT_EQ(SignedEncoderCount<4096>(-2048).value(), -2048);
  EXPECT_EQ(HalfDirection(180).value(), -180);
}
