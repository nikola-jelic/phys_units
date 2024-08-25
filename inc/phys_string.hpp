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

#include "phys_units.hpp"
#include <array>
#include <string>

static std::array<std::string, 8> si_unit_names = {"m", "kg",  "s",  "A",
                                                   "K", "mol", "cd", "deg"};

template <typename Factor> std::string unit_prefix() {
  if (Factor::num == 1 && Factor::den == 1)
    return "";
  else
    return std::to_string(Factor::num) + "/" + std::to_string(Factor::den);
}

// helpers

template <> std::string unit_prefix<std::atto>() { return "a"; }

template <> std::string unit_prefix<std::femto>() { return "f"; }

template <> std::string unit_prefix<std::pico>() { return "p"; }

template <> std::string unit_prefix<std::nano>() { return "n"; }

template <> std::string unit_prefix<std::micro>() { return "u"; }

template <> std::string unit_prefix<std::milli>() { return "m"; }

template <> std::string unit_prefix<std::centi>() { return "c"; }

template <> std::string unit_prefix<std::deci>() { return "d"; }

template <> std::string unit_prefix<std::deca>() { return "da"; }

template <> std::string unit_prefix<std::hecto>() { return "h"; }

template <> std::string unit_prefix<std::kilo>() { return "k"; }

template <> std::string unit_prefix<std::mega>() { return "M"; }

template <> std::string unit_prefix<std::giga>() { return "G"; }

template <> std::string unit_prefix<std::tera>() { return "T"; }

template <> std::string unit_prefix<std::peta>() { return "P"; }

template <> std::string unit_prefix<std::exa>() { return "E"; }

template <int Index> std::string dim_to_string(bool) { return ""; }

template <int Index, typename Dim, typename... Dims>
std::string dim_to_string(bool tail = false) {
  std::string result;
  if (Dim::num == 0)
    result = "";
  else {
    result = (tail ? " " : "") + si_unit_names[Index];
    if (Dim::num != 1 || Dim::den != 1) {
      result += "^" + std::to_string(Dim::num) +
                (Dim::den == 1 ? "" : "/" + Dim::den);
      tail = true;
    }
  }
  return result + dim_to_string<Index + 1, Dims...>(tail);
}

// Volts
template <>
std::string
dim_to_string<0, std::ratio<2>, std::ratio<1>, std::ratio<-3>, std::ratio<-1>,
              std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>>(
    bool) {
  return "V";
}

namespace std {

template <typename V, typename Factor, typename... Dims>
std::string to_string(const units::PhysicalUnit<V, Factor, Dims...> &val) {
  return std::to_string(val.value()) + unit_prefix<Factor>() +
         dim_to_string<0, Dims...>();
}

template <typename V, typename DV, typename Offset, typename Factor,
          typename... Dims>
std::string to_string(
    const units::AbsolutePhysicalUnit<V, DV, Offset, Factor, Dims...> &val) {
  return std::to_string(val.value()) + unit_prefix<Factor>() +
         dim_to_string<0, Dims...>();
}
}; // namespace std
