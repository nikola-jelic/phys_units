/*
Copyright 2024-2026 Nikola Jelić <nikola.jelic83@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "phys_units.hpp"
#include <cmath>
#include <ratio>
#include <type_traits>

namespace units {

template <typename T> struct ResultingType {
  using type = typename std::conditional<std::is_floating_point<T>::value, T,
                                         double>::type;
};

template <typename ValType, typename Factor>
using PhysicalUnitAngle =
    PhysicalUnit<ValType, Factor, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                 std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                 std::ratio<1>>;

/* Degrees per radian, 180/pi = 57.29577951308232...
   Kept at seven significant digits: enough to stay well inside double
   precision for a single conversion, while small enough that std::ratio
   arithmetic on it cannot overflow intmax_t. */
using RadianRatio = std::ratio<5729578, 100000>;

/* A full circle expressed in units of Factor. */
template <typename Factor>
using FullCircle = std::ratio_divide<std::ratio<360>, Factor>;

/* Which interval a direction is folded into. A bool could encode only two
   choices; an enum leaves room for the others that come up in practice.

   Extend it in place: a new range is a value here plus a branch in
   AbsoluteAngle::normalise. */
enum class AngleRange {
  Positive, /* [0, circle)           */
  Signed    /* [-circle/2, circle/2) */
};

template <typename ValType, typename Factor = std::ratio<1>,
          AngleRange Range = AngleRange::Positive>
class AbsoluteAngle {
public:
  using DiffAngleUnit = PhysicalUnitAngle<ValType, Factor>;
  using ValueType = ValType;
  using ConvFactor = Factor;
  using IntervalAlign = std::integral_constant<AngleRange, Range>;

  constexpr AbsoluteAngle() : m_value(0) {}
  explicit AbsoluteAngle(ValType initialValue)
      : m_value(normalise(initialValue)) {}

  /* The source is already normalised, so no renormalisation is needed and the
     copy stays trivial. */
  AbsoluteAngle(const AbsoluteAngle &) = default;
  AbsoluteAngle &operator=(const AbsoluteAngle &) = default;

  template <typename V, typename F, AngleRange R>
  explicit AbsoluteAngle(const AbsoluteAngle<V, F, R> &val)
      : m_value(normalise(static_cast<ValType>(
            static_cast<detail::ArithResult<V, ValType>>(val.value()) *
            std::ratio_divide<F, Factor>::num /
            std::ratio_divide<F, Factor>::den))) {}

  template <typename V, AngleRange R>
  AbsoluteAngle const &operator=(const AbsoluteAngle<V, Factor, R> &val) {
    m_value = normalise(static_cast<ValType>(val.value()));
    return *this;
  }

  AbsoluteAngle const &operator+=(const DiffAngleUnit &rhs) {
    m_value = normalise(m_value + rhs.value());
    return *this;
  }

  AbsoluteAngle const &operator-=(const DiffAngleUnit &rhs) {
    m_value = normalise(m_value - rhs.value());
    return *this;
  }

  /* The difference between two directions is an angle, not a direction, so it
     is returned unnormalised. */
  constexpr DiffAngleUnit operator-(const AbsoluteAngle &rhs) const {
    return DiffAngleUnit(value() - rhs.value());
  }

  AbsoluteAngle operator-(const DiffAngleUnit &rhs) const {
    return AbsoluteAngle(value() - rhs.value());
  }

  AbsoluteAngle operator+(const DiffAngleUnit &rhs) const {
    return AbsoluteAngle(value() + rhs.value());
  }

  constexpr ValType value() const { return m_value; }

private:
  /* Folds an arbitrary value into the interval named by Range. */
  static ValType normalise(const ValType &value) {
    const ValType circle =
        static_cast<ValType>(FullCircle<Factor>::num) / FullCircle<Factor>::den;
    const ValType result = units::fmod(value, circle);
    if (Range == AngleRange::Signed) {
      /* The upper bound is the complement of the lower one rather than
         circle / 2 again. With an odd circle, as an encoder with an odd count
         per revolution gives, integer division truncates and the two guards
         would overlap by one value, so normalising twice would not agree with
         normalising once: 2047 became -2048 became 2047. Both bounds are
         unchanged for an even circle and for a floating point one. */
      const ValType half = circle / 2;
      if (result >= circle - half)
        return static_cast<ValType>(result - circle);
      if (result < -half)
        return static_cast<ValType>(result + circle);
      return result;
    }
    if (result < 0)
      return static_cast<ValType>(result + circle);
    return result;
  }

  ValType m_value;
};

// Comparison operators for the AbsoluteAngle

template <typename... Ts>
inline constexpr bool operator==(const AbsoluteAngle<Ts...> &lhs,
                                 const AbsoluteAngle<Ts...> &rhs) {
  return lhs.value() == rhs.value();
}

template <typename... Ts>
inline constexpr bool operator!=(const AbsoluteAngle<Ts...> &lhs,
                                 const AbsoluteAngle<Ts...> &rhs) {
  return lhs.value() != rhs.value();
}

template <typename... Ts>
inline constexpr bool operator<=(const AbsoluteAngle<Ts...> &lhs,
                                 const AbsoluteAngle<Ts...> &rhs) {
  return lhs.value() <= rhs.value();
}

template <typename... Ts>
inline constexpr bool operator>=(const AbsoluteAngle<Ts...> &lhs,
                                 const AbsoluteAngle<Ts...> &rhs) {
  return lhs.value() >= rhs.value();
}

template <typename... Ts>
inline constexpr bool operator<(const AbsoluteAngle<Ts...> &lhs,
                                const AbsoluteAngle<Ts...> &rhs) {
  return lhs.value() < rhs.value();
}

template <typename... Ts>
inline constexpr bool operator>(const AbsoluteAngle<Ts...> &lhs,
                                const AbsoluteAngle<Ts...> &rhs) {
  return lhs.value() > rhs.value();
}

/* Range is a non-type parameter, so it has to be spelled out: a typename pack
   can never bind to one, and the earlier signature using one could not be
   deduced against any AbsoluteAngle at all.

   On a full interval this is a no-op, every direction there already being
   non-negative; on a half interval it mirrors the direction about zero. */
template <typename V, typename Factor, AngleRange Range>
inline AbsoluteAngle<V, Factor, Range>
abs(const AbsoluteAngle<V, Factor, Range> &val) {
  using std::abs;
  return AbsoluteAngle<V, Factor, Range>(abs(val.value()));
}

/* Trigonometry.
   These wrappers live in namespace units and are picked up by
   argument-dependent lookup, so `using std::sin; sin(direction);` works. The
   inverse functions take the target angle type as an explicit template
   argument and therefore have to be called qualified, e.g.
       auto d = units::asin<Direction>(0.5);
   Defining them inside namespace std, as earlier versions did, is undefined
   behaviour and is rejected by clang. */

#define UNITS_ABSOLUTE_ANGLE_TRIG(func)                                        \
  template <typename Val, typename Factor, AngleRange R>                       \
  inline typename ResultingType<Val>::type func(                               \
      const AbsoluteAngle<Val, Factor, R> &val) {                              \
    return std::func(                                                          \
        AbsoluteAngle<typename ResultingType<Val>::type, RadianRatio, R>(val)  \
            .value());                                                         \
  }

#define UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(func)                                \
  template <typename A, typename T>                                            \
  inline typename std::enable_if<                                              \
      std::is_same<                                                            \
          A, AbsoluteAngle<typename A::ValueType, typename A::ConvFactor,      \
                           A::IntervalAlign::value>>::value,                   \
      A>::type                                                                 \
  func(T val) {                                                                \
    return A(AbsoluteAngle<T, RadianRatio, A::IntervalAlign::value>(           \
        std::func(val)));                                                      \
  }

#define UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG2(func)                               \
  template <typename A, typename T>                                            \
  inline typename std::enable_if<                                              \
      std::is_same<                                                            \
          A, AbsoluteAngle<typename A::ValueType, typename A::ConvFactor,      \
                           A::IntervalAlign::value>>::value,                   \
      A>::type                                                                 \
  func(T val1, T val2) {                                                       \
    return A(AbsoluteAngle<T, RadianRatio, A::IntervalAlign::value>(           \
        std::func(val1, val2)));                                               \
  }

#define UNITS_PHYSICAL_ANGLE_TRIG(func)                                        \
  template <typename Val, typename Factor>                                     \
  inline typename ResultingType<Val>::type func(                               \
      const PhysicalUnitAngle<Val, Factor> &val) {                             \
    return std::func(                                                          \
        PhysicalUnitAngle<typename ResultingType<Val>::type, RadianRatio>(val) \
            .value());                                                         \
  }

#define UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(func)                                \
  template <typename P, typename T>                                            \
  inline typename std::enable_if<                                              \
      std::is_same<P, PhysicalUnitAngle<typename P::ValueType,                 \
                                        typename P::ConvFactor>>::value,       \
      P>::type                                                                 \
  func(T val) {                                                                \
    return P(PhysicalUnitAngle<T, RadianRatio>(std::func(val)));               \
  }

#define UNITS_PHYSICAL_ANGLE_INVERSE_TRIG2(func)                               \
  template <typename P, typename T>                                            \
  inline typename std::enable_if<                                              \
      std::is_same<P, PhysicalUnitAngle<typename P::ValueType,                 \
                                        typename P::ConvFactor>>::value,       \
      P>::type                                                                 \
  func(T val1, T val2) {                                                       \
    return P(PhysicalUnitAngle<T, RadianRatio>(std::func(val1, val2)));        \
  }

UNITS_ABSOLUTE_ANGLE_TRIG(sin)
UNITS_ABSOLUTE_ANGLE_TRIG(cos)
UNITS_ABSOLUTE_ANGLE_TRIG(tan)
UNITS_ABSOLUTE_ANGLE_TRIG(sinh)
UNITS_ABSOLUTE_ANGLE_TRIG(cosh)
UNITS_ABSOLUTE_ANGLE_TRIG(tanh)

UNITS_PHYSICAL_ANGLE_TRIG(sin)
UNITS_PHYSICAL_ANGLE_TRIG(cos)
UNITS_PHYSICAL_ANGLE_TRIG(tan)
UNITS_PHYSICAL_ANGLE_TRIG(sinh)
UNITS_PHYSICAL_ANGLE_TRIG(cosh)
UNITS_PHYSICAL_ANGLE_TRIG(tanh)

UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(asin)
UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(acos)
UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(atan)
UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(asinh)
UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(acosh)
UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG(atanh)
UNITS_ABSOLUTE_ANGLE_INVERSE_TRIG2(atan2)

UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(asin)
UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(acos)
UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(atan)
UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(asinh)
UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(acosh)
UNITS_PHYSICAL_ANGLE_INVERSE_TRIG(atanh)
UNITS_PHYSICAL_ANGLE_INVERSE_TRIG2(atan2)

} // namespace units
