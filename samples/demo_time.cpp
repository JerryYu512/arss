#include "brsdk/log/logging.hpp"
#include "brsdk/time/date.hpp"
#include "brsdk/time/timeiso8601.hpp"
#include "brsdk/time/timestamp.hpp"
#include "brsdk/time/timezone.hpp"

using namespace brsdk;

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	{
		Date d1(2022, 1, 19);
		LOG_DEBUG << "d1 year[" << d1.year() << "], mon[" << d1.month() << "], day[" << d1.day() << "]";
		LOG_DEBUG << "d1 week: " << Date::weekday_itoa(d1.weekDay());
		LOG_DEBUG << "d1 julianday: " << d1.julianDayNumber();
		LOG_DEBUG << "d1 : " << d1.toIsoString();
	}

	{
		struct timeval tv;
		struct tm tm;

		gettimeofday(&tv, NULL);
		time_t tt = tv.tv_sec;
		localtime_r(&tt, &tm);

		Date d2(tm);
		LOG_DEBUG << "d2 year[" << d2.year() << "], mon[" << d2.month() << "], day[" << d2.day() << "]";
		LOG_DEBUG << "d2 week: " << Date::weekday_itoa(d2.weekDay());
		LOG_DEBUG << "d2 julianday: " << d2.julianDayNumber();
		LOG_DEBUG << "d2 : " << d2.toIsoString();
	}

	{
		auto tt = Timestamp::time();
		LOG_DEBUG << "tt : " << tt;
		LOG_DEBUG << "time offset : " << TimeZone::timezoneOffset();
		timeiso8601_t t = {
			tt,
			0,
			TimeZone::timezoneOffset() / 60,
		};
		TimeISO8601 itime(t);

		std::string f1 = itime.format();
		std::string f2 = itime.format(6);
		auto utct = itime.to_utc();
		auto localt = itime.to_local();

		LOG_DEBUG << "f1 : " << f1;
		LOG_DEBUG << "f2 : " << f2;
		LOG_DEBUG << "valid : " << (itime.valid() ? "true" : "false");

		char buf1[64] = "";
		char buf2[64] = "";

		LOG_DEBUG << "buf1 : " << Timestamp::gmtime_fmt(&utct, buf1, "GMT");
		LOG_DEBUG << "buf2 : " << Timestamp::gmtime_fmt(&localt, buf2);

		auto timei = itime.time();
		LOG_DEBUG << "sec[" << timei.sec << "], offset[" << timei.offset << "]";
	}

	return 0;
}