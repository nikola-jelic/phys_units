/*
Copyright 2024 Nikola Jelic <nikola.jelic83@gmail.com>

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

template <typename ValType, typename Factor = std::ratio<1>,
          bool HalfInterval = false>
class AbsoluteAngle {
public:
  using DiffAngleUnit = PhysicalUnitAngle<ValType, Factor>;
  using ValueType = ValType;
  using ConvFactor = Factor;
  using IntervalAlign = std::integral_constant<bool, HalfInterval>;
  constexpr explicit AbsoluteAngle(ValType initialValue) {
    setValue(initialValue);
  }
  constexpr AbsoluteAngle() : m_value(0) {}
  constexpr AbsoluteAngle(const AbsoluteAngle &val) { setValue(val.value()); }
  template <typename V, typename F, bool H>
  explicit AbsoluteAngle(const AbsoluteAngle<V, F, H> &val) {
    using outRatio = std::ratio_divide<F, Factor>;
    setValue(decltype(val.value() * m_value)(val.value()) * outRatio::num /
             outRatio::den);
  }
  template <typename V, bool H>
  AbsoluteAngle const &operator=(const AbsoluteAngle<V, Factor, H> &val) {
    setValue(val.value());
    return *this;
  }

  AbsoluteAngle const &operator+=(const DiffAngleUnit &rhs) {
    setValue(m_value + rhs.value());
    return *this;
  }

  AbsoluteAngle const &operator-=(const DiffAngleUnit &rhs) {
    setValue(m_value - rhs.value());
    return *this;
  }

  constexpr DiffAngleUnit operator-(const AbsoluteAngle &rhs) const {
    return DiffAngleUnit(value - rhs.value());
  }

  constexpr AbsoluteAngle operator-(const DiffAngleUnit &rhs) const {
    return AbsoluteAngle(value - rhs.value());
  }

  constexpr AbsoluteAngle operator+(const DiffAngleUnit &rhs) const {
    return AbsoluteAngle(value + rhs.value());
  }

  constexpr ValType value() const { return m_value; }

protected:
  virtual void setValue(const ValType &value) {
    using fullCircle =
        std::ratio_multiply<std::ratio<360>,
                            std::ratio_divide<std::ratio<1>, Factor>>;
    ValType factor = ValType(fullCircle::num) / fullCircle::den;
    ValType result = std::fmod(value, factor);
    if (HalfInterval && (result >= (factor / 2))) {
      m_value = result - factor;
    } else if (!HalfInterval && (result < 0)) {
      m_value = result + factor;
    } else {
      m_value = result;
    }
  }

private:
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

using RadianRatio = std::ratio<57296, 1000>;

}; // namespace units

namespace std {
template <typename V, typename... Args>
units::AbsoluteAngle<V, Args...>
abs(const units::AbsoluteAngle<V, Args...> &val) {
  return abs(val.value());
}

}; // namespace std

#define ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(func)                               \
  template <typename Val, typename Factor, bool H>                             \
  typename units::ResultingType<Val>::type std::func(                          \
      const units::AbsoluteAngle<Val, Factor, H> &val) {                       \
    return std::func(                                                          \
        units::AbsoluteAngle<typename units::ResultingType<Val>::type,         \
                             units::RadianRatio, H>(val)                       \
            .value());                                                         \
  }

#define ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(func)                       \
  template <typename A, typename T>                                            \
  typename std::enable_if<                                                     \
      std::is_same<A, typename units::AbsoluteAngle<                           \
                          typename A::ValueType, typename A::ConvFactor,       \
                          A::IntevalAlign::value>>::value,                     \
      A>::type                                                                 \
  std::func(T val) {                                                           \
    return A(                                                                  \
        units::AbsoluteAngle<T, units::RadianRatio, A::IntervalAlign::value>(  \
            std::func(val)));                                                  \
  }

#define ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER2(func)                      \
  template <typename A, typename T>                                            \
  typename std::enable_if<                                                     \
      std::is_same<A, typename units::AbsoluteAngle<                           \
                          typename A::ValueType, typename A::ConvFactor,       \
                          A::IntevalAlign::value>>::value,                     \
      A>::type                                                                 \
  std::func(T val1, T val2) {                                                  \
    return A(                                                                  \
        units::AbsoluteAngle<T, units::RadianRatio, A::IntervalAlign::value>(  \
            std::func(val1, val2)));                                           \
  }

ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(sin)
ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(cos)
ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(tan)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(asin)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(acos)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(atan)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER2(atan2)

ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(sinh)
ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(cosh)
ABSOLUTE_UNIT_TRIGONOMETRY_WRAPPER(tanh)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(asinh)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(acosh)
ABSOLUTE_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(atanh)

#define PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(func)                               \
  template <typename Val, typename Factor>                                     \
  typename units::ResultingType<Val>::type std::func(                          \
      const units::PhysicalUnitAngle<Val, Factor> &val) {                      \
    return std::func(                                                          \
        units::PhysicalUnitAngle<typename units::ResultingType<Val>::type,     \
                                 units::RadianRatio>(val)                      \
            .value());                                                         \
  }

#define PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(func)                       \
  template <typename P, typename T>                                            \
  typename std::enable_if<std::is_same<P, typename units::PhysicalUnitAngle<   \
                                              typename P::ValueType,           \
                                              typename P::ConvFactor>>::value, \
                          P>::type                                             \
  std::func(T val) {                                                           \
    return P(units::PhysicalUnitAngle<T, units::RadianRatio>(std::func(val))); \
  }

#define PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER2(func)                      \
  template <typename P, typename T>                                            \
  typename std::enable_if<std::is_same<P, typename units::PhysicalUnitAngle<   \
                                              typename P::ValueType,           \
                                              typename P::ConvFactor>>::value, \
                          P>::type                                             \
  std::func(T val1, T val2) {                                                  \
    return P(units::PhysicalUnitAngle<T, units::RadianRatio>(                  \
        std::func(val1, val2)));                                               \
  }

PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(sin)
PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(cos)
PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(tan)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(asin)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(acos)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(atan)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER2(atan2)

PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(sinh)
PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(cosh)
PHYSICAL_UNIT_TRIGONOMETRY_WRAPPER(tanh)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(asinh)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(acosh)
PHYSICAL_UNIT_INVERSE_TRIGONOMETRY_WRAPPER(atanh)
