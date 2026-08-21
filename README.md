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

Given its embedded focus, the foundational design decisions for the library are:
- C++11 standard, given its wide spread; as a side note, I haven't seen that many embedded projects that use anything more modern than C++14
- Favour integral types over floating points whenever possible; speed and predictability are strong decision drivers in the embedded world
- Minimal runtime overhead; everything that can be done in the compile time should be done there

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
They live in `namespace units` and are reached through argument-dependent
lookup; see the section on namespaces below.

AbsoluteAngle class offers a functionality similar to the AbsolutePhysicalUnit, but specific to the angles:
- AbsoluteAngle can only have values within the given interval
- As with the AbsolutePhysicalUnit, AbsoluteAngle supports addition and subtraction, but not multiplication
and division

## Pretty printing

`phys_string.hpp` renders units for humans. It is a hosted diagnostic aid
rather than part of the embedded core: it allocates `std::string` and pulls in
`<sstream>`, which nothing else in the library does.

```C++
units::to_string(Acceleration(9.81));   // "9.81 m/s^2"
units::to_string(Speed(3.5));           // "3.5 m/s"
units::to_string(Hertz(50.));           // "50 1/s"
units::to_string(CentiMeter(400));      // "400 cm"
```

Negative exponents are rendered as a denominator instead of trailing `^-n`, and
a denominator holding more than one unit is bracketed, so that `m/(s A)` cannot
be misread as `(m/s) A`.

Precision and style are optional arguments:

```C++
units::to_string(Acceleration(9.81), 1);                      // "9.8 m/s^2"
units::to_string(Acceleration(9.81), -1, units::UnitStyle::Utf8);  // "9.81 m/s²"
```

A precision of -1, the default, gives the shortest useful form rather than the
six decimals `std::to_string` is fixed at.

The symbol depends only on the type, so it can be asked for without a value, for
a column heading or an axis label:

```C++
units::unit_symbol<Speed>();                        // "m/s"
units::unit_symbol<Acceleration>(units::UnitStyle::Utf8);  // "m/s²"
```

Two extension points name derived units. `DimSymbol` names a dimension list and
still composes with SI prefixes, so one specialisation covers both volts and
millivolts:

```C++
template <>
struct units::DimSymbol<std::ratio<2>, std::ratio<1>, std::ratio<-3>,
                        std::ratio<-1>, std::ratio<0>, std::ratio<0>,
                        std::ratio<0>, std::ratio<0>> {
  static std::string get(units::UnitStyle) { return "V"; }
};
```

`UnitSymbol` names a whole unit type, factor included, for units whose scale is
part of their name and which therefore cannot be spelled as a prefix plus a
dimension list, such as km/h. Without such a specialisation an unrecognised
factor is rendered explicitly, as `100 (5/18) m/s`.

## Namespaces and lookup

Every helper the library provides lives in `namespace units`. Earlier versions
defined some of them inside `namespace std` instead - the trigonometry
wrappers, `abs`, `fmod`, `sqrt`, `cbrt`, `to_string` and the `std::chrono`
converters. Adding overloads to `namespace std` for the library's own types is
undefined behaviour, and clang rejects it outright, which is why the library
used to fail to build with clang on every C++ standard.

Functions that take a unit as an argument are found through argument-dependent
lookup, so a `using` declaration is all that is needed:

```C++
using std::sin;
using std::sqrt;

double s = sin(AngleInDegrees(30));  // finds units::sin
auto side = sqrt(area);              // finds units::sqrt
auto positive = units::abs(-length);
```

Functions that only take the target type as an explicit template argument have
nothing to deduce a namespace from, so they must be qualified:

```C++
auto direction = units::asin<Direction>(0.5);
auto bearing = units::atan2<Direction>(1., 1.);
std::string text = units::to_string(length);
auto duration = units::duration_cast(interval);
auto interval = units::unit_cast(duration);
```

If you are upgrading from an earlier version, replacing `std::` with `units::`
at these call sites is the whole migration.

## Install

### Bash

Install library using `setup.sh` script.

#### Build and install with default path (/usr/include/) 
`./setup.sh install`

#### Uninstall with default path
`./setup.sh uninstall`

#### Build and install with custom path
`./setup.sh path=/custom_path install`

#### Uninstall with custom path
`./setup.sh path=/custom_path uninstall`


### CMake

#### Create build folder and navigate to it.
```
mkdir build
cd build
```

Command bellow is optional and used to specify custom install path.

`cmake .. -DCMAKE_INSTALL_PREFIX=/install_path`

#### Build and install project
```
cmake --build .
cmake --install .
```

#### Uninstall library
`cmake --build . --target uninstall` 

or

`xargs rm < install_manifest.txt`

*Note: These commands needs to be run from build folder.*

## Compilers and C++ standards matrix
<!-- compiler-matrix-start -->

| Compiler | C++11 | C++14 | C++17 | C++20 | C++23 |
|----------|-------|-------|-------|-------|-------|
| g++      | ✅    | ✅    | ✅    | ✅    | ✅    |
| clang++  | ✅    | ✅    | ✅    | ✅    | ✅    |

<!-- compiler-matrix-end -->
