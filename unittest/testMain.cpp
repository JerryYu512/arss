#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "arss/doctest.h"
#include "arss/defs/defs.hpp"

TEST_CASE("testing defines") {
	CHECK(ARSS_64BIT == 1);
}