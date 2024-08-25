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

namespace std {
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

template <> struct num_sqrt<10000> {
  static const intmax_t value = 100;
};

template <> struct num_sqrt<1000000> {
  static const intmax_t value = 1000;
};

template <> struct num_sqrt<100000000> {
  static const intmax_t value = 1000;
};

template <typename R>
using ratio_sqrt = ratio<num_sqrt<R::num>::value, num_sqrt<R::den>::value>;

template <typename V, typename F, typename... Dims>
auto sqrt(const units::PhysicalUnit<V, F, Dims...> &val)
    -> units::PhysicalUnit<decltype(sqrt(val.value())), ratio_sqrt<F>,
                           ratio_divide<Dims, ratio<2>>...> {
  return units::PhysicalUnit<decltype(sqrt(val.value())), ratio_sqrt<F>,
                             ratio_divide<Dims, ratio<2>>...>(
      sqrt(val.value()));
}

template <intmax_t Number> struct num_cbrt {};

template <> struct num_cbrt<0> {
  static const intmax_t value = 0;
};

template <> struct num_cbrt<1> {
  static const intmax_t value = 1;
};

template <> struct num_cbrt<8> {
  static const intmax_t value = 2;
};

template <> struct num_cbrt<27> {
  static const intmax_t value = 3;
};

template <> struct num_cbrt<1000> {
  static const intmax_t value = 10;
};

template <> struct num_cbrt<1000000> {
  static const intmax_t value = 100;
};

template <typename R>
using ratio_cbrt = ratio<num_cbrt<R::num>::value, num_cbrt<R::den>::value>;

template <typename V, typename F, typename... Dims>
auto cbrt(const units::PhysicalUnit<V, F, Dims...> &val)
    -> units::PhysicalUnit<decltype(cbrt(val.value())), ratio_cbrt<F>,
                           ratio_divide<Dims, ratio<3>>...> {
  return units::PhysicalUnit<decltype(cbrt(val.value())), ratio_cbrt<F>,
                             ratio_divide<Dims, ratio<3>>...>(
      cbrt(val.value()));
}

}; // namespace std
