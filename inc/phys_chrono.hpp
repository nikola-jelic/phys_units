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
#include <chrono>

namespace std::chrono {

template <typename Rep, typename Period>
constexpr std::chrono::duration<Rep, Period>
duration_cast(const units::PhysicalUnit<Rep, Period, std::ratio<0>,
                                        std::ratio<0>, std::ratio<1>> &val) {
  return std::chrono::duration<Rep, Period>(val.value());
}
}; // namespace std::chrono

namespace units {

template <typename V, typename F>
PhysicalUnit<V, F, std::ratio<0>, std::ratio<0>, std::ratio<1>>
unit_cast(const std::chrono::duration<V, F> &val) {
  return PhysicalUnit<V, F, std::ratio<0>, std::ratio<0>, std::ratio<1>>(
      val.count());
}

}; // namespace units
