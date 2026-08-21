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
#include <ratio>
#include <string>

namespace units {

/* Wrapped in a function rather than left as a namespace scope array: a header
   scope object would give every translation unit its own mutable copy. */
inline const char *si_unit_name(int index) {
  static const char *const names[] = {"m", "kg",  "s",  "A",
                                      "K", "mol", "cd", "deg"};
  return names[index];
}

/* Explicit specialisations of a function template are not implicitly inline,
   so every one of these needs the keyword; without it, including this header
   in more than one translation unit fails to link. */
template <typename Factor> inline std::string unit_prefix() {
  if (Factor::num == 1 && Factor::den == 1)
    return "";
  return std::to_string(Factor::num) + "/" + std::to_string(Factor::den);
}

template <> inline std::string unit_prefix<std::atto>() { return "a"; }
template <> inline std::string unit_prefix<std::femto>() { return "f"; }
template <> inline std::string unit_prefix<std::pico>() { return "p"; }
template <> inline std::string unit_prefix<std::nano>() { return "n"; }
template <> inline std::string unit_prefix<std::micro>() { return "u"; }
template <> inline std::string unit_prefix<std::milli>() { return "m"; }
template <> inline std::string unit_prefix<std::centi>() { return "c"; }
template <> inline std::string unit_prefix<std::deci>() { return "d"; }
template <> inline std::string unit_prefix<std::deca>() { return "da"; }
template <> inline std::string unit_prefix<std::hecto>() { return "h"; }
template <> inline std::string unit_prefix<std::kilo>() { return "k"; }
template <> inline std::string unit_prefix<std::mega>() { return "M"; }
template <> inline std::string unit_prefix<std::giga>() { return "G"; }
template <> inline std::string unit_prefix<std::tera>() { return "T"; }
template <> inline std::string unit_prefix<std::peta>() { return "P"; }
template <> inline std::string unit_prefix<std::exa>() { return "E"; }

template <int Index> inline std::string dim_to_string(bool) { return ""; }

template <int Index, typename Dim, typename... Dims>
inline std::string dim_to_string(bool tail = false) {
  std::string result;
  if (Dim::num != 0) {
    result = std::string(tail ? " " : "") + si_unit_name(Index);
    if (Dim::num != 1 || Dim::den != 1) {
      result +=
          "^" + std::to_string(Dim::num) +
          (Dim::den == 1 ? std::string() : "/" + std::to_string(Dim::den));
    }
    /* Any emitted unit means the next one needs a separator, whether or not
       this one carried an exponent. */
    tail = true;
  }
  return result + dim_to_string<Index + 1, Dims...>(tail);
}

// Volts
template <>
inline std::string
dim_to_string<0, std::ratio<2>, std::ratio<1>, std::ratio<-3>, std::ratio<-1>,
              std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>>(
    bool) {
  return "V";
}

template <typename V, typename Factor, typename... Dims>
inline std::string to_string(const PhysicalUnit<V, Factor, Dims...> &val) {
  return std::to_string(val.value()) + unit_prefix<Factor>() +
         dim_to_string<0, Dims...>();
}

/* The parameter order here has to match AbsolutePhysicalUnit itself:
   value type, conversion factor, difference type, offset, then dimensions. */
template <typename V, typename Factor, typename DiffType, typename Offset,
          typename... Dims>
inline std::string to_string(
    const AbsolutePhysicalUnit<V, Factor, DiffType, Offset, Dims...> &val) {
  return std::to_string(val.value()) + unit_prefix<Factor>() +
         dim_to_string<0, Dims...>();
}

} // namespace units
