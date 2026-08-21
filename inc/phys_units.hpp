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

#include <cmath>
#include <cstdlib>
#include <ratio>
#include <type_traits>
#include <utility>

namespace units {

namespace detail {
/* Result type of an arithmetic operation between A and B, following the usual
   promotion rules. Used to keep the converting constructors constexpr under
   C++11, where the constructor body must be empty. */
template <typename A, typename B>
using ArithResult = decltype(std::declval<A>() * std::declval<B>());
} // namespace detail

template <typename ValType, typename Factor = std::ratio<1>,
          typename LenDim = std::ratio<0>, typename MassDim = std::ratio<0>,
          typename TimeDim = std::ratio<0>, typename ElcurDim = std::ratio<0>,
          typename TempDim = std::ratio<0>, typename AmmDim = std::ratio<0>,
          typename LumDim = std::ratio<0>, typename AngleDim = std::ratio<0>>
class PhysicalUnit {
public:
  using ValueType = ValType;
  using ConvFactor = Factor;
  constexpr explicit PhysicalUnit(ValType initialValue)
      : m_value(initialValue) {}
  constexpr PhysicalUnit() : m_value(0) {}

  /* converting constructor */
  template <typename V, typename F>
  constexpr explicit PhysicalUnit(
      const PhysicalUnit<V, F, LenDim, MassDim, TimeDim, ElcurDim, TempDim,
                         AmmDim, LumDim, AngleDim> &val)
      : m_value(static_cast<ValType>(
            static_cast<detail::ArithResult<V, ValType>>(val.value()) *
            std::ratio_divide<F, Factor>::num /
            std::ratio_divide<F, Factor>::den)) {}

  template <typename V>
  PhysicalUnit const &
  operator=(const PhysicalUnit<V, Factor, LenDim, MassDim, TimeDim, ElcurDim,
                               TempDim, AmmDim, LumDim, AngleDim> &val) {
    m_value = val.value();
    return *this;
  }

  PhysicalUnit const &operator+=(const PhysicalUnit &rhs) {
    m_value += rhs.value();
    return *this;
  }

  PhysicalUnit const &operator-=(const PhysicalUnit &rhs) {
    m_value -= rhs.value();
    return *this;
  }

  PhysicalUnit const &operator*=(const ValType &rhs) {
    m_value *= rhs;
    return *this;
  }

  PhysicalUnit const &operator/=(const ValType &rhs) {
    m_value /= rhs;
    return *this;
  }

  PhysicalUnit const &operator++() {
    m_value++;
    return *this;
  }

  PhysicalUnit const &operator--() {
    m_value--;
    return *this;
  }

  constexpr PhysicalUnit operator-() const { return PhysicalUnit(-m_value); }
  constexpr ValType value() const { return m_value; }

private:
  ValType m_value;
};

/* Dimensionless specialisation: implicitly convertible to the underlying type,
   scaled by its conversion factor. It still exposes value()/ValueType/
   ConvFactor so that the generic comparison and arithmetic operators below
   work on dimensionless results just like on any other unit. */
template <typename ValType, typename Factor>
class PhysicalUnit<ValType, Factor, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                   std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                   std::ratio<0>> {
public:
  using ValueType = ValType;
  using ConvFactor = Factor;
  constexpr explicit PhysicalUnit(ValType initialValue)
      : m_value(initialValue) {}
  constexpr PhysicalUnit() : m_value(0) {}

  /* converting constructor */
  template <typename V, typename F>
  constexpr explicit PhysicalUnit(
      const PhysicalUnit<V, F, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                         std::ratio<0>, std::ratio<0>, std::ratio<0>,
                         std::ratio<0>, std::ratio<0>> &val)
      : m_value(static_cast<ValType>(
            static_cast<detail::ArithResult<V, ValType>>(val.value()) *
            std::ratio_divide<F, Factor>::num /
            std::ratio_divide<F, Factor>::den)) {}

  PhysicalUnit const &operator+=(const PhysicalUnit &rhs) {
    m_value += rhs.value();
    return *this;
  }

  PhysicalUnit const &operator-=(const PhysicalUnit &rhs) {
    m_value -= rhs.value();
    return *this;
  }

  PhysicalUnit const &operator*=(const ValType &rhs) {
    m_value *= rhs;
    return *this;
  }

  PhysicalUnit const &operator/=(const ValType &rhs) {
    m_value /= rhs;
    return *this;
  }

  constexpr PhysicalUnit operator-() const { return PhysicalUnit(-m_value); }

  /* Raw stored value, consistent with the primary template. */
  constexpr ValType value() const { return m_value; }

  /* Implicit decay to the underlying type, scaled by the conversion factor. */
  constexpr operator ValType() const {
    return m_value * Factor::num / Factor::den;
  }

private:
  ValType m_value;
};
template <typename V1, typename V2, typename... Ts>
inline constexpr bool operator==(const PhysicalUnit<V1, Ts...> &lhs,
                                 const PhysicalUnit<V2, Ts...> &rhs) {
  return lhs.value() == rhs.value();
}

template <typename V1, typename V2, typename... Ts>
inline constexpr bool operator!=(const PhysicalUnit<V1, Ts...> &lhs,
                                 const PhysicalUnit<V2, Ts...> &rhs) {
  return lhs.value() != rhs.value();
}

template <typename V1, typename V2, typename... Ts>
inline constexpr bool operator<=(const PhysicalUnit<V1, Ts...> &lhs,
                                 const PhysicalUnit<V2, Ts...> &rhs) {
  return lhs.value() <= rhs.value();
}

template <typename V1, typename V2, typename... Ts>
inline constexpr bool operator>=(const PhysicalUnit<V1, Ts...> &lhs,
                                 const PhysicalUnit<V2, Ts...> &rhs) {
  return lhs.value() >= rhs.value();
}

template <typename V1, typename V2, typename... Ts>
inline constexpr bool operator<(const PhysicalUnit<V1, Ts...> &lhs,
                                const PhysicalUnit<V2, Ts...> &rhs) {
  return lhs.value() < rhs.value();
}

template <typename V1, typename V2, typename... Ts>
inline constexpr bool operator>(const PhysicalUnit<V1, Ts...> &lhs,
                                const PhysicalUnit<V2, Ts...> &rhs) {
  return lhs.value() > rhs.value();
}

template <typename V1, typename V2, typename... Ts>
constexpr auto operator+(const PhysicalUnit<V1, Ts...> &lhs,
                         const PhysicalUnit<V2, Ts...> &rhs)
    -> PhysicalUnit<decltype(lhs.value() + rhs.value()), Ts...> {
  return PhysicalUnit<decltype(lhs.value() + rhs.value()), Ts...>(lhs.value() +
                                                                  rhs.value());
}

template <typename V1, typename V2, typename... Ts>
constexpr auto operator-(const PhysicalUnit<V1, Ts...> &lhs,
                         const PhysicalUnit<V2, Ts...> &rhs)
    -> PhysicalUnit<decltype(lhs.value() - rhs.value()), Ts...> {
  return PhysicalUnit<decltype(lhs.value() - rhs.value()), Ts...>(lhs.value() -
                                                                  rhs.value());
}

template <typename V1, typename F1, typename... Args1, typename V2, typename F2,
          typename... Args2>
constexpr auto operator*(const PhysicalUnit<V1, F1, Args1...> &lhs,
                         const PhysicalUnit<V2, F2, Args2...> &rhs)
    -> PhysicalUnit<decltype(lhs.value() * rhs.value()),
                    std::ratio_multiply<F1, F2>,
                    std::ratio_add<Args1, Args2>...> {
  return PhysicalUnit<decltype(lhs.value() * rhs.value()),
                      std::ratio_multiply<F1, F2>,
                      std::ratio_add<Args1, Args2>...>(lhs.value() *
                                                       rhs.value());
}

template <typename V1, typename V2, typename... Ts>
constexpr auto operator*(const V1 &lhs, const PhysicalUnit<V2, Ts...> &rhs)
    -> PhysicalUnit<decltype(lhs * rhs.value()), Ts...> {
  return PhysicalUnit<decltype(lhs * rhs.value()), Ts...>(lhs * rhs.value());
}

template <typename V1, typename V2, typename... Ts>
constexpr auto operator*(const PhysicalUnit<V1, Ts...> &lhs, const V2 &rhs)
    -> PhysicalUnit<decltype(lhs.value() * rhs), Ts...> {
  return PhysicalUnit<decltype(lhs.value() * rhs), Ts...>(lhs.value() * rhs);
}

template <typename V1, typename F1, typename... Args1, typename V2, typename F2,
          typename... Args2>
constexpr auto operator/(const PhysicalUnit<V1, F1, Args1...> &lhs,
                         const PhysicalUnit<V2, F2, Args2...> &rhs)
    -> PhysicalUnit<decltype(lhs.value() / rhs.value()),
                    std::ratio_divide<F1, F2>,
                    std::ratio_subtract<Args1, Args2>...> {
  return PhysicalUnit<decltype(lhs.value() / rhs.value()),
                      std::ratio_divide<F1, F2>,
                      std::ratio_subtract<Args1, Args2>...>(lhs.value() /
                                                            rhs.value());
}

template <typename V1, typename F, typename... Args, typename V2>
constexpr auto operator/(const V1 &lhs, const PhysicalUnit<V2, F, Args...> &rhs)
    -> PhysicalUnit<decltype(lhs / rhs.value()),
                    std::ratio_divide<std::ratio<1>, F>,
                    std::ratio_subtract<std::ratio<0>, Args>...> {
  return PhysicalUnit<decltype(lhs / rhs.value()),
                      std::ratio_divide<std::ratio<1>, F>,
                      std::ratio_subtract<std::ratio<0>, Args>...>(lhs /
                                                                   rhs.value());
}

template <typename V1, typename V2, typename... Ts>
constexpr auto operator/(const PhysicalUnit<V1, Ts...> &lhs, const V2 &rhs)
    -> PhysicalUnit<decltype(lhs.value() / rhs), Ts...> {
  return PhysicalUnit<decltype(lhs.value() / rhs), Ts...>(lhs.value() / rhs);
}

template <typename V, typename... Ts>
constexpr typename std::enable_if<std::is_integral<V>::value, V>::type
operator%(const PhysicalUnit<V, Ts...> &lhs,
          const PhysicalUnit<V, Ts...> &rhs) {
  return (lhs.value() % rhs.value());
}

template <typename ToType, typename... Ts>
constexpr ToType unit_cast(const PhysicalUnit<Ts...> &fromType) {
  return ToType(fromType);
}

template <typename ValType, typename Factor = std::ratio<1>,
          typename DiffType = ValType, typename Offset = std::ratio<0>,
          typename LenDim = std::ratio<0>, typename MassDim = std::ratio<0>,
          typename TimeDim = std::ratio<0>, typename ElcurDim = std::ratio<0>,
          typename TempDim = std::ratio<0>, typename AmmDim = std::ratio<0>,
          typename LumDim = std::ratio<0>>
class AbsolutePhysicalUnit {
public:
  using ValueType = ValType;
  using DiffPhysicalUnit =
      PhysicalUnit<DiffType, Factor, LenDim, MassDim, TimeDim, ElcurDim,
                   TempDim, AmmDim, LumDim>;
  constexpr explicit AbsolutePhysicalUnit(ValType initialValue)
      : m_value(initialValue) {}
  constexpr AbsolutePhysicalUnit() : m_value(0) {}

  constexpr AbsolutePhysicalUnit(const AbsolutePhysicalUnit &val)
      : m_value(val.value()) {}

  template <typename V, typename F, typename D, typename O>
  constexpr explicit AbsolutePhysicalUnit(
      const AbsolutePhysicalUnit<V, F, D, O, LenDim, MassDim, TimeDim, ElcurDim,
                                 TempDim, AmmDim, LumDim> &val)
      : m_value(static_cast<ValType>(
            static_cast<detail::ArithResult<V, ValType>>(val.value()) *
                std::ratio_divide<F, Factor>::num /
                std::ratio_divide<F, Factor>::den +
            static_cast<detail::ArithResult<V, ValType>>(
                std::ratio_divide<std::ratio_subtract<O, Offset>,
                                  Factor>::num) /
                std::ratio_divide<std::ratio_subtract<O, Offset>,
                                  Factor>::den)) {}

  template <typename V>
  AbsolutePhysicalUnit const &
  operator=(const AbsolutePhysicalUnit<V, Factor, DiffType, Offset, LenDim,
                                       MassDim, TimeDim, ElcurDim, TempDim,
                                       AmmDim, LumDim> &val) {
    m_value = val.value();
    return *this;
  }

  AbsolutePhysicalUnit const &operator+=(const DiffPhysicalUnit &rhs) {
    m_value += rhs.value();
    return *this;
  }

  AbsolutePhysicalUnit const &operator-=(const DiffPhysicalUnit &rhs) {
    m_value -= rhs.value();
    return *this;
  }

  AbsolutePhysicalUnit const &operator++() {
    m_value++;
    return *this;
  }

  AbsolutePhysicalUnit const &operator--() {
    m_value--;
    return *this;
  }

  constexpr DiffPhysicalUnit operator-(const AbsolutePhysicalUnit &rhs) const {
    return DiffPhysicalUnit(value() - rhs.value());
  }

  constexpr AbsolutePhysicalUnit operator-(const DiffPhysicalUnit &rhs) const {
    return AbsolutePhysicalUnit(value() - rhs.value());
  }

  constexpr AbsolutePhysicalUnit operator+(const DiffPhysicalUnit &rhs) const {
    return AbsolutePhysicalUnit(value() + rhs.value());
  }

  constexpr ValType value() const { return m_value; }

private:
  ValType m_value;
};

template <typename ToType, typename... Ts>
ToType unit_cast(const AbsolutePhysicalUnit<Ts...> &fromType) {
  return ToType(fromType);
}

template <typename... Ts>
inline constexpr bool operator==(const AbsolutePhysicalUnit<Ts...> &lhs,
                                 const AbsolutePhysicalUnit<Ts...> &rhs) {
  return lhs.value() == rhs.value();
}

template <typename... Ts>
inline constexpr bool operator!=(const AbsolutePhysicalUnit<Ts...> &lhs,
                                 const AbsolutePhysicalUnit<Ts...> &rhs) {
  return lhs.value() != rhs.value();
}

template <typename... Ts>
inline constexpr bool operator<=(const AbsolutePhysicalUnit<Ts...> &lhs,
                                 const AbsolutePhysicalUnit<Ts...> &rhs) {
  return lhs.value() <= rhs.value();
}

template <typename... Ts>
inline constexpr bool operator>=(const AbsolutePhysicalUnit<Ts...> &lhs,
                                 const AbsolutePhysicalUnit<Ts...> &rhs) {
  return lhs.value() >= rhs.value();
}

template <typename... Ts>
inline constexpr bool operator<(const AbsolutePhysicalUnit<Ts...> &lhs,
                                const AbsolutePhysicalUnit<Ts...> &rhs) {
  return lhs.value() < rhs.value();
}

template <typename... Ts>
inline constexpr bool operator>(const AbsolutePhysicalUnit<Ts...> &lhs,
                                const AbsolutePhysicalUnit<Ts...> &rhs) {
  return lhs.value() > rhs.value();
}

template <typename... Ts>
constexpr AbsolutePhysicalUnit<Ts...>
operator+(const typename AbsolutePhysicalUnit<Ts...>::DiffPhysicalUnit &lhs,
          const AbsolutePhysicalUnit<Ts...> &rhs) {
  return AbsolutePhysicalUnit<Ts...>(lhs.value() + rhs.value());
}

/* Mathematical helpers.
   These used to be injected into namespace std, which is undefined behaviour
   and is rejected outright by clang. They now live in namespace units and are
   found through argument-dependent lookup, so
       using std::abs; abs(length);
   resolves to units::abs for unit types and to std::abs for built-ins. */

/* fmod for integral types, so that the angle normalisation below can stay
   integral (and constexpr) rather than detouring through a floating point. */
template <typename A>
constexpr typename std::enable_if<std::is_integral<A>::value, A>::type
fmod(A a, A b) {
  return a % b;
}

template <typename A>
inline typename std::enable_if<std::is_floating_point<A>::value, A>::type
fmod(A a, A b) {
  return std::fmod(a, b);
}

template <typename V, typename... Args>
inline PhysicalUnit<V, Args...> abs(const PhysicalUnit<V, Args...> &val) {
  using std::abs;
  return PhysicalUnit<V, Args...>(abs(val.value()));
}

template <typename V, typename... Args>
inline AbsolutePhysicalUnit<V, Args...>
abs(const AbsolutePhysicalUnit<V, Args...> &val) {
  using std::abs;
  return AbsolutePhysicalUnit<V, Args...>(abs(val.value()));
}

} // namespace units
