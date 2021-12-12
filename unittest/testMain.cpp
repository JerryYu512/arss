#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "brsdk/doctest.h"
#include "brsdk/defs/defs.hpp"
#include "brsdk/co/co.hpp"
#include "brsdk/time/date.hpp"
#include "brsdk/time/timestamp.hpp"
#include "brsdk/time/timezone.hpp"
#include "brsdk/time/timeiso8601.hpp"

using namespace brsdk;

TEST_CASE("testing defines") {
	if (8 == sizeof(long)) {
		CHECK(BRSDK_64BIT == 1);
	} else {
		CHECK(BRSDK_64BIT == 0);
	}
#ifdef BRSDK_OS_LINUX
	CHECK(1 == 1);
#endif

#ifdef BRSDK_ARCH_X64
	MESSAGE("arch 64");
#elif BRSDK_ARCH_ARM
	MESSAGE("arch arm32");
#elif BRSDK_ARCH_ARM64
	MESSAGE("arch arm64");
#endif

#pragma BRSDK_TODO(.....)
	brsdk_printd("hello doctest");

	char dump_buf[] = "fwe0jvsjdkjti3425";
	BRSDK_DUMP_BUFFER(dump_buf, sizeof(dump_buf));

	MESSAGE("endian : ", BRSDK_BYTE_ORDER);

	char str[] = "dsfjowertwe9843245s/!@*$#'{,m";
	for (size_t i = 0; i < BRSDK_STRLEN(str); i++) {
		if (!BRSDK_IS_ALPHA(str[i])) {
			MESSAGE(str[i], " is not alpha");
		}
	}
}

static void co_1(int a) {
	for (int i = 0; i < a; i++) {
		printf("co 1 : %d - %d\n", a, i);
		yield();
	}
}

TEST_CASE("co schedule") {
	auto sch = brsdk::sch_ref();
	new_co_default(co_1, 1);
	new_co(1*1024*1024, co_1, 2);
	sch_run();
	sch_stop(sch);
}

TEST_CASE("time-date") {
	Date date(2021, 12, 12);
	CHECK_EQ("2021-12-12", date.toIsoString());
	Timestamp ts(Timestamp::now());

	MESSAGE(ts.toString());
	MESSAGE(ts.toFormattedFileString());
	MESSAGE(ts.toFormattedString());

	timeiso8601_t stdt = {
		Timestamp::time(),
		0,
		8 * 60
	};
	char stdbuf[64] = "";
	TimeISO8601::format(stdbuf, sizeof(stdbuf), &stdt);
	MESSAGE(stdbuf);
}