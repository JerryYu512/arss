/**
 * MIT License
 * 
 * Copyright Â© 2021 <Jerry.Yu>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the â€œSoftwareâ€?), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED â€œAS ISâ€?, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file demo_crypto.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2021-12-26
 * 
 * @copyright MIT License
 * 
 */
#include "brsdk/log/logging.hpp"
#include "brsdk/crypto/crc16.hpp"
#include "brsdk/crypto/crc32.hpp"
#include "brsdk/crypto/jhash.hpp"
#include "brsdk/crypto/md5.hpp"
#include "brsdk/crypto/murmur_hash.hpp"
#include "brsdk/crypto/random.hpp"
#include "brsdk/crypto/sha1.hpp"
#include "brsdk/crypto/uuid.hpp"
#include "brsdk/defs/defs.hpp"
#include <string>

using namespace brsdk;

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	const char *str = "0123456789 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	LOG_DEBUG << "crc16 : " << crypto::crc16(str, strlen(str), 0) << "\n";
	LOG_DEBUG << "crc16 : " << crypto::crc16(str, strlen(str)) << "\n";
	LOG_DEBUG << "crc16 : " << crypto::crc16(str) << "\n";
	LOG_DEBUG << "crc16 : " << crypto::crc16(std::string(str)) << "\n";

	crypto::crc32_msb_init();
	crypto::crc32_lsb_init();

	LOG_DEBUG << "crc32 : " << crypto::crc32(0, (const unsigned char*)str, strlen(str)) << "\n";
	LOG_DEBUG << "crc32 : " << crypto::crc32_msb(0, (const unsigned char*)str, strlen(str)) << "\n";
	LOG_DEBUG << "crc32 : " << crypto::crc32_lsb(0, (const unsigned char*)str, strlen(str)) << "\n";

	LOG_DEBUG << "jhash : " << crypto::jhash(str, strlen(str), 53) << "\n";

	uint8_t md5[16] = {0};
	crypto::md5_gen(str, strlen(str), md5);
	BRSDK_DUMP_BUFFER(md5, sizeof(md5));

	char out[64] = "";
	crypto::sha1_hex((unsigned char*)str, strlen(str), out, 64);

	LOG_DEBUG << "sha1 : " << out << "\n";

	char uuid[38] = "";
	crypto::uuid_generate(uuid);
	LOG_DEBUG << "uuid : " << uuid << "\n";

	return 0;
}
