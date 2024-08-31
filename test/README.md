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

