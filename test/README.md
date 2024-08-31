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

Additionally, if the arithmetic operations yield a dimensionless physical unit (i.e. the one where all the dimension exponents are zero), a special `operator T()` allows us to simply extract the value, multiplied by its conversion factor.

The predictability of the arithmetic operations allows us to:
- create new types on the fly
- have a known resulting type in the compile time
- avoid common errors that can occur with complex calculations that involve values with different dimensions and/or conversion factors

Some common resulting types, as examples:
- if we divide meters by seconds, we will get $\frac{m}{s}$, or speed
- if we multiple speed by seconds (or some other time interval type), we will get the total distance as length
- if we divide a scalar by seconds we will get $\frac{1}{s}$ or simply _Hz_
