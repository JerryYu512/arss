#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "brsdk/doctest.h"
#include "brsdk/defs/defs.hpp"

TEST_CASE("testing defines") {
	CHECK(BRSDK_64BIT == 1);
}