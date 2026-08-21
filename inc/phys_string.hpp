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
#include <cstdint>
#include <iomanip>
#include <ratio>
#include <sstream>
#include <string>

/* Human readable rendering of units.

   This header is a hosted diagnostic aid, not part of the embedded core: it
   allocates std::string and pulls in <sstream>. The rest of the library is
   allocation free and resolves everything at compile time; nothing here does.
   Do not include it on a target where that matters.

   Extend it in place. The header is small and meant to be vendored, so adding
   a rendering style is an enum value plus a branch in exponent_string, and
   naming a derived unit is a DimSymbol or UnitSymbol specialisation. There is
   no plugin machinery to learn. */

namespace units {

enum class UnitStyle {
  Ascii,        /* m^2, m^1/2 */
  Utf8,         /* m², m^1/2  */
  Utf8Fractions /* m², m¹⁄²   */
};

/* Compile time default for every rendering call, so that a project which wants
   superscripts throughout does not have to name the style at each call site:

       -DUNITS_DEFAULT_STYLE=units::UnitStyle::Utf8Fractions

   Define it consistently for every translation unit that includes this header.
   Two translation units disagreeing about it is an ODR violation, in the same
   way as disagreeing about NDEBUG. */
#ifndef UNITS_DEFAULT_STYLE
#define UNITS_DEFAULT_STYLE UnitStyle::Ascii
#endif

inline UnitStyle default_style() { return UNITS_DEFAULT_STYLE; }

inline const char *si_unit_name(int index) {
  static const char *const names[] = {"m", "kg",  "s",  "A",
                                      "K", "mol", "cd", "deg"};
  return names[index];
}

/* Known SI prefixes. `known` is what tells a recognised prefix, which is glued
   straight onto the unit name, apart from an arbitrary conversion factor,
   which has to be rendered separately so that it cannot merge into the value
   in front of it. */
template <typename Factor> struct SiPrefix {
  static const bool known = false;
  static std::string symbol() { return ""; }
};

#define UNITS_DEFINE_SI_PREFIX(Ratio, Symbol)                                  \
  template <> struct SiPrefix<Ratio> {                                         \
    static const bool known = true;                                            \
    static std::string symbol() { return Symbol; }                             \
  };

UNITS_DEFINE_SI_PREFIX(std::ratio<1>, "")
UNITS_DEFINE_SI_PREFIX(std::atto, "a")
UNITS_DEFINE_SI_PREFIX(std::femto, "f")
UNITS_DEFINE_SI_PREFIX(std::pico, "p")
UNITS_DEFINE_SI_PREFIX(std::nano, "n")
UNITS_DEFINE_SI_PREFIX(std::micro, "u")
UNITS_DEFINE_SI_PREFIX(std::milli, "m")
UNITS_DEFINE_SI_PREFIX(std::centi, "c")
UNITS_DEFINE_SI_PREFIX(std::deci, "d")
UNITS_DEFINE_SI_PREFIX(std::deca, "da")
UNITS_DEFINE_SI_PREFIX(std::hecto, "h")
UNITS_DEFINE_SI_PREFIX(std::kilo, "k")
UNITS_DEFINE_SI_PREFIX(std::mega, "M")
UNITS_DEFINE_SI_PREFIX(std::giga, "G")
UNITS_DEFINE_SI_PREFIX(std::tera, "T")
UNITS_DEFINE_SI_PREFIX(std::peta, "P")
UNITS_DEFINE_SI_PREFIX(std::exa, "E")

/* Kept for callers that only want the prefix symbol. */
template <typename Factor> inline std::string unit_prefix() {
  if (SiPrefix<Factor>::known)
    return SiPrefix<Factor>::symbol();
  return std::to_string(Factor::num) + "/" + std::to_string(Factor::den);
}

namespace detail {

inline std::string superscript_digit(char c) {
  switch (c) {
  case '0':
    return "⁰";
  case '1':
    return "¹";
  case '2':
    return "²";
  case '3':
    return "³";
  case '4':
    return "⁴";
  case '5':
    return "⁵";
  case '6':
    return "⁶";
  case '7':
    return "⁷";
  case '8':
    return "⁸";
  case '9':
    return "⁹";
  default:
    return std::string(1, c);
  }
}

inline std::string superscript(const std::string &digits) {
  std::string result;
  for (std::string::size_type i = 0; i < digits.size(); ++i)
    result += superscript_digit(digits[i]);
  return result;
}

/* Exponents only ever reach this positive: a negative one has already been
   moved into the denominator.

   A fractional exponent is spelled out as ^n/d in the plain Utf8 style, since
   a raised fraction needs a font that positions U+2044 well and degrades badly
   where it does not. Utf8Fractions opts into it: m¹⁄². */
/* Extension point: a new UnitStyle gets its branch here. */
inline std::string exponent_string(intmax_t num, intmax_t den,
                                   UnitStyle style) {
  const std::string numerator = std::to_string(num);

  if (den == 1) {
    if (style == UnitStyle::Ascii)
      return "^" + numerator;
    return superscript(numerator);
  }

  const std::string denominator = std::to_string(den);
  if (style == UnitStyle::Utf8Fractions)
    return superscript(numerator) + "⁄" + superscript(denominator);
  return "^" + numerator + "/" + denominator;
}

struct DimEntry {
  intmax_t num;
  intmax_t den;
};

template <int Index> inline void collect_dims(DimEntry *) {}

template <int Index, typename Dim, typename... Dims>
inline void collect_dims(DimEntry *out) {
  out[Index].num = Dim::num;
  out[Index].den = Dim::den;
  collect_dims<Index + 1, Dims...>(out);
}

/* Positive exponents form the numerator and negative ones the denominator, so
   that a speed reads as m/s rather than as m s^-1. A denominator holding more
   than one unit is bracketed, because m/s A would otherwise be ambiguous. */
inline std::string compose_dims(const DimEntry *dims, int count,
                                UnitStyle style) {
  std::string numerator, denominator;
  int denominator_count = 0;

  for (int i = 0; i < count; ++i) {
    intmax_t num = dims[i].num;
    const intmax_t den = dims[i].den;
    if (num == 0)
      continue;

    const bool negative = num < 0;
    if (negative) {
      num = -num;
      ++denominator_count;
    }

    std::string &target = negative ? denominator : numerator;
    if (!target.empty())
      target += " ";
    target += si_unit_name(i);
    if (num != 1 || den != 1)
      target += exponent_string(num, den, style);
  }

  if (denominator.empty())
    return numerator;
  if (numerator.empty())
    numerator = "1";
  if (denominator_count > 1)
    return numerator + "/(" + denominator + ")";
  return numerator + "/" + denominator;
}

/* An unrecognised factor is bracketed and spaced off, so that a value of 2
   with a factor of 3/7 reads as "2 (3/7) m" instead of running together into
   the unreadable "23/7m". */
template <typename Factor> inline std::string factor_prefix() {
  if (SiPrefix<Factor>::known)
    return SiPrefix<Factor>::symbol();
  return "(" + std::to_string(Factor::num) + "/" + std::to_string(Factor::den) +
         ") ";
}

/* std::to_string is fixed at six decimals, which renders 2.0 as "2.000000".
   A default constructed stream instead gives the shortest useful form, and an
   explicit precision switches to fixed notation. */
template <typename V>
inline std::string value_string(const V &value, int precision) {
  std::ostringstream stream;
  if (precision >= 0)
    stream << std::fixed << std::setprecision(precision);
  stream << value;
  return stream.str();
}

inline void rstrip(std::string &text) {
  while (!text.empty() && text[text.size() - 1] == ' ')
    text.erase(text.size() - 1);
}

} // namespace detail

/* Symbol for a dimension list. Specialise this to give a derived unit its own
   name; it composes with SI prefixes, so a specialisation for volts also
   covers millivolts. */
template <typename... Dims> struct DimSymbol {
  static std::string get(UnitStyle style) {
    detail::DimEntry dims[sizeof...(Dims)];
    detail::collect_dims<0, Dims...>(dims);
    return detail::compose_dims(dims, sizeof...(Dims), style);
  }
};

// Volts
template <>
struct DimSymbol<std::ratio<2>, std::ratio<1>, std::ratio<-3>, std::ratio<-1>,
                 std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>> {
  static std::string get(UnitStyle) { return "V"; }
};

/* Symbol for a whole unit type, factor included. Specialise this for units
   whose conversion factor is part of their name, such as km/h, which no
   combination of an SI prefix and a dimension list can express. */
template <typename Unit> struct UnitSymbol;

template <typename V, typename Factor, typename... Dims>
struct UnitSymbol<PhysicalUnit<V, Factor, Dims...>> {
  static std::string get(UnitStyle style) {
    std::string prefix = detail::factor_prefix<Factor>();
    const std::string dims = DimSymbol<Dims...>::get(style);
    if (dims.empty()) {
      detail::rstrip(prefix);
      return prefix;
    }
    return prefix + dims;
  }
};

template <typename V, typename Factor, typename DiffType, typename Offset,
          typename... Dims>
struct UnitSymbol<AbsolutePhysicalUnit<V, Factor, DiffType, Offset, Dims...>> {
  static std::string get(UnitStyle style) {
    std::string prefix = detail::factor_prefix<Factor>();
    const std::string dims = DimSymbol<Dims...>::get(style);
    if (dims.empty()) {
      detail::rstrip(prefix);
      return prefix;
    }
    return prefix + dims;
  }
};

/* The symbol depends only on the type, so it can be asked for without a value
   in hand, for a column heading or an axis label. */
template <typename Unit>
inline std::string unit_symbol(UnitStyle style = default_style()) {
  return UnitSymbol<Unit>::get(style);
}

template <typename V, typename Factor, typename... Dims>
inline std::string to_string(const PhysicalUnit<V, Factor, Dims...> &val,
                             int precision = -1,
                             UnitStyle style = default_style()) {
  const std::string symbol =
      unit_symbol<PhysicalUnit<V, Factor, Dims...>>(style);
  const std::string value = detail::value_string(val.value(), precision);
  if (symbol.empty())
    return value;
  return value + " " + symbol;
}

template <typename V, typename Factor, typename DiffType, typename Offset,
          typename... Dims>
inline std::string
to_string(const AbsolutePhysicalUnit<V, Factor, DiffType, Offset, Dims...> &val,
          int precision = -1, UnitStyle style = default_style()) {
  const std::string symbol =
      unit_symbol<AbsolutePhysicalUnit<V, Factor, DiffType, Offset, Dims...>>(
          style);
  const std::string value = detail::value_string(val.value(), precision);
  if (symbol.empty())
    return value;
  return value + " " + symbol;
}

} // namespace units
