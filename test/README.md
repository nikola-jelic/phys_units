# Example usage

## Single dimension quantity

If we define a type to represent a meter, we could do something like this:
```C++
using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
```
Let's understand the template arguments better:
- the holding type will be _int_
- meter is an SI unit, so the conversion factor is one
- the exponent for lenght is also one
- all other exponents for the remaining dimensions are implicitly set to zero

The following code would be legal:
```C++
Meter a(3), b(7), c;
c = a + b;
c = a - b;
c = a * 4;
c = b / 7;

if (a > b) {
	// do something
} else if (a <= b) {
	// do something else
}
```
Since the underlying types are the same and the basic arithmetic operations are simply forwarded, there are no surprises so far.

All of the following lines would be illegal:
```C++
c = a - 4;
c = 12 + a;
if (a < 17) {}
```
The compiler would produce a type error in all of these cases, because the physical unit cannot be added, subtracted nor compared with any other type except itself. It can, however, be multiplied and divided, as we will see in the future examples.

## Same dimension, different scales

Now let's imagine that we need to represent both the meters _and_ the centemeters in the code. In the embedded world, we tend to avoid the floating point types whenever we can, mostly due to the performance issues, so let's use the integer type for both units:
```C++
using Meter = units::PhysicalUnit<int, std::ratio<1>, std::ratio<1>>;
using CentiMeter = units::PhysicalUnit<int, std::centi, std::ratio<1>>;
```
We can see that the only difference between the two types is in the conversion factor:
- `std::centi` is just a nice naming for `std::ratio<1,100>`
- one centimeter is indeed one hundeth of a meter, so this makes sense

Let's see how these two types now interact:
```C++
Meter meters(4);
CentiMeter centimeters(meters); // centimeters are now set to 400
centimeters = CentiMeter(1300);
meters = units::unit_cast<Meter>(centimeters); // meters is now set to 13
```
Since these types have the same SI dimension (i.e. length), they are both constructor-convertible and `unit_cast` convertible.

The `units::unit_cast<T>(arg)` function is just a wrapper for the constructor invocation, which is allowed if and only if all unit dimensions of the source and target types are the same.

The copy constructor allows for a different holding type, but not for the different scaling factor.

The constructor is intentionally markes with the `explicit` keyword to avoid any accidental type casts in the code.

Additionally, except for the multiplication and division, _all_ other operations, assignments and comparisons between `Meter` and `CentiMeter` types will produce a compilation error.

## Higher dimensions

Let's say that our program needs to handle not only length in different scales, but also the surface.

We can introduce a new type:
```C++
using MeterSquared = units::PhysicalUnit<int, std::ratio<1>, std::ratio<2>>;
```
We see that the dimension parameter has changed from one to two, representing $m^2$.

We could now write a code like this:
```C++
Meter a(3), b(6);
MeterSquared c = a * b; // c has the value 18
int d = b / a; // d has the value 2
if ((c / a) == b) {} // the condition would be true
```
Notes on the multiplication and division operations:
- the resulting holding type is decided by C++ type deduction rules
- the conversion factors are multiplied or divided, depending on the operation
- all unit dimensions are added or subtracted, depending on the operation
- since `std::ratio` is being used for conversion factor and dimensions, **all** type calculations are done in the compile time, with no runtime overhead

Additionally, if the arithmetic operations yield a dimensionless physical unit (i.e. the one where all the dimension exponents are zero), a special `operator T()` allows us to simply extract the value, multiplied by its conversion factor.

The predictability of the arithmetic operations allows us to:
- create new types on the fly
- have a known resulting type in the compile time
- avoid common errors that can occur with complex calculations that involve values with different dimensions and/or conversion factors

Some common resulting types, as examples:
- if we divide meters by seconds, we will get $\frac{m}{s}$, or speed
- if we multiple speed by seconds (or some other time interval type), we will get the total distance as length
- if we divide a scalar by seconds we will get $\frac{1}{s}$ or simply _Hz_

## Absolute units

Absolute units have a similar template signature to the physical units, with some notable additions:
- diff type, to be used as a holding type for the resulting difference physical unit (more about this later)
- offset from the SI zero, mostly used for the tempareture scales (e.g. Celsius, Fahernheit, etc.)

Two common examples of the need for the absolute units are temperature and time. In case of time, we would draw two parallels:
- between the point in time and the absolute unit
- between the time interval and the ordinary physical unit.

In case of temperature, we would do similarly:
- between the measured temperature and the absolute unit
- between the temperature interval and the ordinary physical unit.

Let's also take a look at some operational differences between the absolute and common physical units:
- absolute units cannot be multiplied nor divided (e.g. it doesn't make much sense to divide _now_ as a timestamp)
- absolute units can be subtracted, thus yielding a differetial type: `now - yesteday == 24h`
- differential unit type can be added or subtracted from its absolute unit, thus yielding the same absolute unit type: `now + 7d == a_week_from_now`

As an example, let's convert between Kelvins and Fahrenheits:
```C++
using TempKelvin =
    units::AbsolutePhysicalUnit<double, std::ratio<1>, double, std::ratio<0>,
                                std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                std::ratio<0>, std::ratio<1>>;
using TempFahrenheit = units::AbsolutePhysicalUnit<
    double, std::ratio<5, 9>, double, std::ratio<229835, 900>, std::ratio<0>,
    std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
```
Some notes on the template signatures:
- both will use `double` for both the holding and the difference type
- as Kelvin is the SI unit, the conversion factor will be one, but it will be $\frac{5}{9}$ for Fahrenheit
- similarly, the offset for Kelvin is zero, but it is $\frac{229835}{900}$ for Fahrenheit

If we perform zero testing (i.e. checking the value of zero from one temperature unit to another), we will easily see that:
- 0K = -459.67F
- 0F = 255.37K

## Basic mathematical helper functions

It would be impossible to provide all the math functions from the standard library, but I've picked a few that were handy for me:
- abs; absolute value
- sqrt; square root
- cbrt; cubic root

Absolute value implementation is trivial and it won't be covered here.

The implementation of the root functions is more interesting and must be elaborated:
- for the holding value, we simply apply the root function
- for the dimensions, we divide them by the root exponent (i.e. divide by two for square root, etc.)
- for the conversion factor, we would like to apply the root function, but the ratio is a compile time type, so this is not an option

My solution is rather lazy, but effective - I have introduced a template structure that calculates the root (square or cubic) of a number in the compile time, but only for the commonly used values. **Note**: if your code needs some atypical value to be rooted, you can easily provide a template specialisation for that number.

Some example code for the template square structure:
```C++
template <intmax_t Number> struct num_sqrt {};

template <> struct num_sqrt<0> {
  static const intmax_t value = 0;
};

template <> struct num_sqrt<1> {
  static const intmax_t value = 1;
};

template <> struct num_sqrt<100> {
  static const intmax_t value = 10;
};

template <> struct num_sqrt<360> {
  static const intmax_t value = 60;
};
```

## Complex numbers as the holding types

The library will accept the complex types for holding values, but all other restrictions for the complex numbers apply. For example, complex numbers cannot be compared, aside from equality.

## Angles

Angles are not SI units, but they are very useful in the daily life of the embedded engineer, so I ended up introducing them as a special unit, both common and the absolute.

For the common unit, the eight dimension was added and that was it. As the base measurement unit for the angles, I chose the degree, as it offers excellent integral scaling and effectively better precision than the floating point types. The conversion factors for the radians and for the full circle units (i.e. effectively conversion factor of 360) are provided.

The absolute type for the angle is effectively a direction and it shares many of the constraints of other absolute units:
- it makes no sense to multiply or divide a direction, but it makes sense for the angle
- angle can be used as the interval value and the direction for the actual data point

There is one additional peculiarity for the absolute angles:
- the value can be normalised to either [0, N) or to [-N/2, N/2)
- the solution was to introduce a template argument which enforses the correct behaviour

### Trigonometry

All trigonometry functions provided by the standard library expect or provide values in radians, which posed a challenge to the library's design.

I ended up creating some helper macros for the two (or three) types of functions from the standard library:
- functions that expect radians (e.g. `sin`) are overloaded to convert any angle type to the radian variant and then call the library function
- functions that return radians (e.g. `asin`) are overloaded to accept a target angle type and convert the radians to the expected type
- the faux third type is only for the `atan2` function, as it takes two arguments instead of one, but it otherwise behaves like other inverse functions from the previous point

