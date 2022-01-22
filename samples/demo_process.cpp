#include "brsdk/log/logging.hpp"
#include "brsdk/process/process_info.hpp"
#include "brsdk/os/os.hpp"
#include "brsdk/mem/slab.hpp"

using namespace brsdk;

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	LOG_DEBUG << "pid : " << ProcessInfo::pid();
	LOG_DEBUG << "pid : " << ProcessInfo::pidString();
	LOG_DEBUG << "uid : " << ProcessInfo::uid();
	LOG_DEBUG << "username : " << ProcessInfo::username();
	LOG_DEBUG << "starttime : " << ProcessInfo::startTime().toFormattedString();
	LOG_DEBUG << "clock : " << ProcessInfo::clockTicksPerSecond();
	LOG_DEBUG << "pagesize : " << ProcessInfo::pageSize();
	LOG_DEBUG << "hostname : " << ProcessInfo::hostname();
	LOG_DEBUG << "procname : " << ProcessInfo::procname();
	LOG_DEBUG << "procstatus :\n" << ProcessInfo::procStatus();
	LOG_DEBUG << "procstat : " << ProcessInfo::procStat();
	LOG_DEBUG << "threadstat : " << ProcessInfo::threadStat();
	LOG_DEBUG << "exepath : " << ProcessInfo::exePath();
	LOG_DEBUG << "openedFiles : " << ProcessInfo::openedFiles();
	LOG_DEBUG << "max open files : " << ProcessInfo::maxOpenFiles();
	LOG_DEBUG << "cputime : " << ProcessInfo::cpuTime().total();
	LOG_DEBUG << "threads : " << ProcessInfo::numThreads();
	auto ts = ProcessInfo::threads();

	for (auto &it : ts) {
		LOG_DEBUG << it;
	}

	LOG_DEBUG << "arch : " << os::get_arch();
	LOG_DEBUG << "platform : " << os::get_platform();
	LOG_DEBUG << "name : " << os::get_sysname();
	LOG_DEBUG << "processor : " << os::get_processor();
	LOG_DEBUG << "version : " << os::get_version();
	LOG_DEBUG << "cpu : " << os::get_ncpu();

	os::meminfo_t mem;

	os::get_meminfo(mem);

	LOG_DEBUG << "total : " << mem.total;
	LOG_DEBUG << "free : " << mem.free;

	LOG_DEBUG << "PATH : " << os::env("PATH");


	MemorySlab slab(malloc(16 * 1024 * 1024), 16 * 1024 * 1024);

	slab_stat_t st;

	slab.stat(st);

	LOG_DEBUG << "pool_size : " << st.pool_size << " bytes";
	LOG_DEBUG << "used_size : " << st.used_size << " bytes";
	LOG_DEBUG << "used_pct : " << st.used_pct << " bytes";

	LOG_DEBUG << "total page count : " << st.pages;
	LOG_DEBUG << "free page count : " << st.free_page;

	LOG_DEBUG << "small slab use page : " << st.p_small
	          << ", \tbytes : " << st.b_small;
	LOG_DEBUG << "exact slab use page : " << st.p_exact
	          << ", \tbytes : " << st.b_exact;
	LOG_DEBUG << "big slab use page : " << st.p_big
	          << ", \tbytes : " << st.b_big;
	LOG_DEBUG << "page slab use page : " << st.p_page
	          << ", \tbytes : " << st.b_page;

	LOG_DEBUG << "max free pages : " << st.max_free_pages;

	LOG_INFO << "start addr : " << slab.address();
	LOG_INFO << slab.alloc(1024);
	LOG_INFO << slab.alloc(2 * 1024);
	LOG_INFO << slab.alloc(8);

	slab.stat(st);

	LOG_DEBUG << "pool_size : " << st.pool_size << " bytes";
	LOG_DEBUG << "used_size : " << st.used_size << " bytes";
	LOG_DEBUG << "used_pct : " << st.used_pct << " bytes";

	LOG_DEBUG << "total page count : " << st.pages;
	LOG_DEBUG << "free page count : " << st.free_page;

	LOG_DEBUG << "small slab use page : " << st.p_small
	          << ", \tbytes : " << st.b_small;
	LOG_DEBUG << "exact slab use page : " << st.p_exact
	          << ", \tbytes : " << st.b_exact;
	LOG_DEBUG << "big slab use page : " << st.p_big
	          << ", \tbytes : " << st.b_big;
	LOG_DEBUG << "page slab use page : " << st.p_page
	          << ", \tbytes : " << st.b_page;

	LOG_DEBUG << "max free pages : " << st.max_free_pages;

	return 0;
}
