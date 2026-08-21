/* Compile probe for the compiler/standard matrix.
   It deliberately touches every header and every family of operations, so that
   a matrix cell only reports success if the whole public surface instantiates
   on that compiler and standard. */

#include <chrono>
#include <iostream>

#include "phys_angle.hpp"
#include "phys_chrono.hpp"
#include "phys_math.hpp"
#include "phys_string.hpp"
#include "phys_units.hpp"

using AngleInDegrees = units::PhysicalUnitAngle<int, std::ratio<1>>;
using AngleInMinutes = units::PhysicalUnitAngle<int, std::ratio<1, 60>>;
using Direction = units::AbsoluteAngle<double, std::ratio<1>>;

using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;

using TempKelvin =
    units::AbsolutePhysicalUnit<double, std::ratio<1>, double, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                std::ratio<0>, std::ratio<1>>;

template <typename V, typename F>
using PhysTime =
    units::PhysicalUnit<V, F, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

/* Explicit instantiation compiles every non-template member of these class
   templates, whether or not anything below calls them. Ordinary use only
   compiles the members it happens to touch, which is how AbsoluteAngle's
   operator+ and operator- stayed broken for years while the test suite passed:
   an uninstantiated member is unparsed, and no coverage tool can see it.

   It does not reach member templates such as the converting constructors, nor
   free function templates; main() below exercises those. */
template class units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
template class units::PhysicalUnit<double, std::centi, std::ratio<1>>;
template class units::PhysicalUnit<int, std::ratio<1>>;
/* PhysicalUnitAngle is an alias template, so its underlying class has to be
   named in full here. */
template class units::PhysicalUnit<
    int, std::ratio<1>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
    std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
template class units::AbsoluteAngle<double, std::ratio<1>>;
template class units::AbsoluteAngle<int, std::ratio<1>,
                                    units::AngleRange::Signed>;
template class units::AbsolutePhysicalUnit<int, std::ratio<1>, int,
                                           std::ratio<0>, std::ratio<0>,
                                           std::ratio<0>, std::ratio<1>>;
template class units::AbsolutePhysicalUnit<
    double, std::ratio<5, 9>, double, std::ratio<229835, 900>, std::ratio<0>,
    std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

int main(void) {
  AngleInDegrees angle(60);
  AngleInMinutes angle_minutes(angle);

  Meter meter(4);
  CentiMeter centimeter(meter);
  MeterSquared meter_squared = meter * meter;

  PhysTime<int, std::ratio<1>> seconds(1);
  std::chrono::duration<int> ch_seconds = units::duration_cast(seconds);
  PhysTime<int, std::ratio<1>> other_seconds = units::unit_cast(ch_seconds);

  /* Dimensionless results have to support both the implicit decay and the
     generic comparison operators. */
  int ratio = meter_squared / meter / meter;

  /* AbsoluteAngle arithmetic: these operators never compiled before. */
  Direction north(0.0), east(90.0);
  Direction turned = north + Direction::DiffAngleUnit(90.0);
  double delta = (east - north).value();

  /* Trigonometry through ADL, and the inverse form through an explicit type. */
  using std::sin;
  double s = sin(angle);
  Direction from_asin = units::asin<Direction>(0.5);

  /* Roots and absolute value, also through ADL. */
  using std::sqrt;
  auto side = sqrt(meter_squared);
  auto positive = units::abs(-meter);

  TempKelvin kelvin(273.15);

  std::cout << "Angle in minutes: " << angle_minutes.value() << std::endl;
  std::cout << "Length in centimeters: " << centimeter.value() << std::endl;
  std::cout << "Meter squared: " << units::to_string(meter_squared)
            << std::endl;
  std::cout << "Seconds: " << other_seconds.value() << std::endl;
  std::cout << "Dimensionless ratio: " << ratio << std::endl;
  std::cout << "Turned: " << turned.value() << ", delta: " << delta
            << std::endl;
  std::cout << "sin: " << s << ", asin: " << from_asin.value() << std::endl;
  std::cout << "Side: " << side.value() << ", abs: " << positive.value()
            << std::endl;
  std::cout << "Kelvin: " << kelvin.value() << std::endl;

  return 0;
}
