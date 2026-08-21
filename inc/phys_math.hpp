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
#include <cstdint>
#include <ratio>

namespace units {

namespace detail {
template <intmax_t N> struct DependentFalse : std::false_type {};
} // namespace detail

/* Compile time integer roots.
   std::ratio is a type, so its numerator and denominator cannot be run through
   a runtime root function; instead the roots of the values actually used as
   conversion factors are tabulated here. If your code needs a factor that is
   not listed, add a specialisation for it. */
template <intmax_t Number> struct num_sqrt {
  static_assert(detail::DependentFalse<Number>::value,
                "units::num_sqrt is not specialised for this value; add a "
                "specialisation for the conversion factor you need.");
};

/* Each entry asserts its own correctness, so a mistyped root cannot slip into
   the table again. Note that 360 is deliberately absent: it is not a perfect
   square, and the entry that used to claim num_sqrt<360> == 60 was really the
   root of 3600. */
#define UNITS_DEFINE_SQRT(Number, Root)                                        \
  template <> struct num_sqrt<Number> {                                        \
    static_assert(static_cast<intmax_t>(Root) * static_cast<intmax_t>(Root) == \
                      static_cast<intmax_t>(Number),                           \
                  "num_sqrt entry is not the square root of its key");         \
    static const intmax_t value = Root;                                        \
  };

UNITS_DEFINE_SQRT(0, 0)
UNITS_DEFINE_SQRT(1, 1)
UNITS_DEFINE_SQRT(4, 2)
UNITS_DEFINE_SQRT(9, 3)
UNITS_DEFINE_SQRT(100, 10)
UNITS_DEFINE_SQRT(3600, 60)
UNITS_DEFINE_SQRT(10000, 100)
UNITS_DEFINE_SQRT(1000000, 1000)
UNITS_DEFINE_SQRT(100000000, 10000)
UNITS_DEFINE_SQRT(10000000000, 100000)
UNITS_DEFINE_SQRT(1000000000000, 1000000)

template <typename R>
using ratio_sqrt = std::ratio<num_sqrt<R::num>::value, num_sqrt<R::den>::value>;

template <intmax_t Number> struct num_cbrt {
  static_assert(detail::DependentFalse<Number>::value,
                "units::num_cbrt is not specialised for this value; add a "
                "specialisation for the conversion factor you need.");
};

#define UNITS_DEFINE_CBRT(Number, Root)                                        \
  template <> struct num_cbrt<Number> {                                        \
    static_assert(static_cast<intmax_t>(Root) * static_cast<intmax_t>(Root) *  \
                          static_cast<intmax_t>(Root) ==                       \
                      static_cast<intmax_t>(Number),                           \
                  "num_cbrt entry is not the cube root of its key");           \
    static const intmax_t value = Root;                                        \
  };

UNITS_DEFINE_CBRT(0, 0)
UNITS_DEFINE_CBRT(1, 1)
UNITS_DEFINE_CBRT(8, 2)
UNITS_DEFINE_CBRT(27, 3)
UNITS_DEFINE_CBRT(1000, 10)
UNITS_DEFINE_CBRT(1000000, 100)
UNITS_DEFINE_CBRT(1000000000, 1000)

template <typename R>
using ratio_cbrt = std::ratio<num_cbrt<R::num>::value, num_cbrt<R::den>::value>;

/* Like abs(), these live in namespace units and are reached through ADL:
       using std::sqrt; sqrt(area);
   The dimensions are halved (or thirded) and the conversion factor is rooted
   at compile time via the tables above. */
template <typename V, typename F, typename... Dims>
inline auto sqrt(const PhysicalUnit<V, F, Dims...> &val)
    -> PhysicalUnit<decltype(std::sqrt(val.value())), ratio_sqrt<F>,
                    std::ratio_divide<Dims, std::ratio<2>>...> {
  return PhysicalUnit<decltype(std::sqrt(val.value())), ratio_sqrt<F>,
                      std::ratio_divide<Dims, std::ratio<2>>...>(
      std::sqrt(val.value()));
}

template <typename V, typename F, typename... Dims>
inline auto cbrt(const PhysicalUnit<V, F, Dims...> &val)
    -> PhysicalUnit<decltype(std::cbrt(val.value())), ratio_cbrt<F>,
                    std::ratio_divide<Dims, std::ratio<3>>...> {
  return PhysicalUnit<decltype(std::cbrt(val.value())), ratio_cbrt<F>,
                      std::ratio_divide<Dims, std::ratio<3>>...>(
      std::cbrt(val.value()));
}

} // namespace units
