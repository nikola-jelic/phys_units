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
  units::AbsoluteAngle<int, std::ratio<1>, true> shared(45);
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
  units::AbsoluteAngle<int, std::ratio<1>, true> shared(-30);
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
  units::AbsoluteAngle<double, std::ratio<1>, true> shared(45);
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
  units::AbsoluteAngle<double, std::ratio<1>, true> shared(-30);
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

