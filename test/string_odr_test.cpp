/* A second translation unit including phys_string.hpp.
   Its explicit specialisations are not implicitly inline, so without the
   keyword this file makes the test binary fail to link. The test below only
   has to exist for that guard to work. */

#include "phys_string.hpp"
#include <gtest/gtest.h>

using Kilometre = units::PhysicalUnit<int, std::kilo, std::ratio<1>>;

TEST(StringTests, LinksFromASecondTranslationUnit) {
  EXPECT_EQ(units::to_string(Kilometre(7)), "7km");
}
