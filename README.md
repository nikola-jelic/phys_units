# Physical units library

A simple, header-only template library for C++11 (and later) for physical units.

It provides:
- basic arithmetics with the physical units
- type deduction when multiplication and division are performed
- common helper mathematical functions
- helper converter functions

## Motivation

The development of the library came from a commercial project in the late 2018.
We were dealing with many different physical measurements and we couldn't use
any of the existing solutions due to the licensing policy on the project.

I started developing an embedded-focused C++ template library to solve our project
problems, and what began as an experiment ended up as a nice and useful exercise in C++.
I hope that it might come in handy to others as well.

Link to the [recording](https://www.youtube.com/watch?v=h7J8SMWG2Hw) where I
presented the original version of the solution.

## Basic design of the library

The main header of the library is `phys_units.hpp`, which defines two main template classes:
- PhysicalUnit, for all common use cases
- AbsolutePhysicalUnit, mostly for time and temperature

All physical unit values have:
- underlying type (e.g. int, double, etc.)
- conversion factor to the SI type in form of `std::ratio` template argument
- seven SI dimensions: length, mass, time, electrical current, temperature, ammount, luminosity
- there is also an *additional* eight dimension - angle, and it is discussed separately in the later chapter
Absolute physical units also have:
- differential type; the underlying type for the physical unit used as an interval difference
- offset; mostly used for temperature conversions between Kelvin, Celsius and Fahrenheit

Check the [examples](test/README.md) for more details on how to use the library in the code.

## Time

The `phys_chrono.hpp` header offers two useful converters between the `units::PhysicalUnit`
time interval and `std::chrono::duration`, which should suffice for all common use cases.

## Angles

While angles are not physical units per se, they are often used for various measurement representations
and calculations.
To facilitate easy integration, base PhysicalUnit class has an added eight dimension - angle, for which
the assumed SI unit is the degree. Radians and full circle measurements are also supported via the
conversion factor mechanism.

The library also provides the overrides for all basic trigonometry functions.

AbsoluteAngle class offers a functionality similar to the AbsolutePhysicalUnit, but specific to the angles:
- AbsoluteAngle can only have values within the given interval
- As with the AbsolutePhysicalUnit, AbsoluteAngle supports addition and subtraction, but not multiplication
and division
