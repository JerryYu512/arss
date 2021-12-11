/**
 * MIT License
 * 
 * Copyright © 2021 <wotsen>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file timeiso8601.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "time.hpp"
#include "timeiso8601.hpp"

namespace brsdk {

#define EPOCH INT64_C(62135683200)  /* 1970-01-01T00:00:00 */
#define RDN_OFFSET INT64_C(62135683200)  /* 1970-01-01T00:00:00 */
#define MIN_SEC INT64_C(-62135596800) /* 0001-01-01T00:00:00 */
#define MAX_SEC INT64_C(253402300799) /* 9999-12-31T23:59:59 */

static const uint32_t Pow10[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

static const uint16_t DayOffset[13] = {
    0, 306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275
};

static int parse_2d(const unsigned char* const p, size_t i, uint16_t* vp) {
    unsigned char d0, d1;
    if (((d0 = p[i + 0] - '0') > 9) ||
        ((d1 = p[i + 1] - '0') > 9))
        return 1;
    *vp = d0 * 10 + d1;
    return 0;
}

static int parse_4d(const unsigned char* const p, size_t i, uint16_t* vp) {
    unsigned char d0, d1, d2, d3;
    if (((d0 = p[i + 0] - '0') > 9) ||
        ((d1 = p[i + 1] - '0') > 9) ||
        ((d2 = p[i + 2] - '0') > 9) ||
        ((d3 = p[i + 3] - '0') > 9))
        return 1;
    *vp = d0 * 1000 + d1 * 100 + d2 * 10 + d3;
    return 0;
}

static void rdn_to_ymd(uint32_t rdn, uint16_t* yp, uint16_t* mp, uint16_t* dp) {
    uint32_t Z, H, A, B;
    uint16_t y, m, d;

    Z = rdn + 306;
    H = 100 * Z - 25;
    A = H / 3652425;
    B = A - (A >> 2);
    y = (100 * B + H) / 36525;
    d = B + Z - (1461 * y >> 2);
    m = (535 * d + 48950) >> 14;
    if (m > 12)
        y++, m -= 12;

    *yp = y;
    *mp = m;
    *dp = d - DayOffset[m];
}

static void rdn_to_struct_tm(uint32_t rdn, struct tm* tmp) {
    uint32_t Z, H, A, B;
    uint16_t C, y, m, d;

    Z = rdn + 306;
    H = 100 * Z - 25;
    A = H / 3652425;
    B = A - (A >> 2);
    y = (100 * B + H) / 36525;
    C = B + Z - (1461 * y >> 2);
    m = (535 * C + 48950) >> 14;
    if (m > 12)
        d = C - 306, y++, m -= 12;
    else
        d = C + 59 + ((y & 3) == 0 && (y % 100 != 0 || y % 400 == 0));

    tmp->tm_mday = C - DayOffset[m];    /* Day of month [1,31]           */
    tmp->tm_mon  = m - 1;               /* Month of year [0,11]          */
    tmp->tm_year = y - 1900;            /* Years since 1900              */
    tmp->tm_wday = rdn % 7;             /* Day of week [0,6] (Sunday =0) */
    tmp->tm_yday = d - 1;               /* Day of year [0,365]           */
}

static size_t timestamp_format_internal(char* dst, size_t len, const timeiso8601_t* tsp,
                                        const int precision) {
    unsigned char *p;
    uint64_t sec;
    uint32_t rdn, v;
    uint16_t y, m, d;
    size_t dlen;

    dlen = sizeof("YYYY-MM-DDThh:mm:ssZ") - 1;
    if (tsp->offset)
        dlen += 5; /* hh:mm */

    if (precision)
        dlen += 1 + precision;

    if (dlen >= len)
        return 0;

    sec = tsp->sec + tsp->offset * 60 + EPOCH;
    rdn = sec / 86400;

    rdn_to_ymd(rdn, &y, &m, &d);

   /*
    *           1
    * 0123456789012345678
    * YYYY-MM-DDThh:mm:ss
    */
    p = (unsigned char *)dst;
    v = sec % 86400;
    p[18] = '0' + (v % 10); v /= 10;
    p[17] = '0' + (v %  6); v /=  6;
    p[16] = ':';
    p[15] = '0' + (v % 10); v /= 10;
    p[14] = '0' + (v %  6); v /=  6;
    p[13] = ':';
    p[12] = '0' + (v % 10); v /= 10;
    p[11] = '0' + (v % 10);
    p[10] = 'T';
    p[ 9] = '0' + (d % 10); d /= 10;
    p[ 8] = '0' + (d % 10);
    p[ 7] = '-';
    p[ 6] = '0' + (m % 10); m /= 10;
    p[ 5] = '0' + (m % 10);
    p[ 4] = '-';
    p[ 3] = '0' + (y % 10); y /= 10;
    p[ 2] = '0' + (y % 10); y /= 10;
    p[ 1] = '0' + (y % 10); y /= 10;
    p[ 0] = '0' + (y % 10);
    p += 19;

    if (precision) {
        v = tsp->nsec / Pow10[9 - precision];
        switch (precision) {
            case 9: p[9] = '0' + (v % 10); v /= 10;
            case 8: p[8] = '0' + (v % 10); v /= 10;
            case 7: p[7] = '0' + (v % 10); v /= 10;
            case 6: p[6] = '0' + (v % 10); v /= 10;
            case 5: p[5] = '0' + (v % 10); v /= 10;
            case 4: p[4] = '0' + (v % 10); v /= 10;
            case 3: p[3] = '0' + (v % 10); v /= 10;
            case 2: p[2] = '0' + (v % 10); v /= 10;
            case 1: p[1] = '0' + (v % 10);
        }
        p[0] = '.';
        p += 1 + precision;
    }

    if (!tsp->offset)
        *p++ = 'Z';
    else {
        if (tsp->offset < 0)
            p[0] = '-', v = -tsp->offset;
        else
            p[0] = '+', v = tsp->offset;

        p[5] = '0' + (v % 10); v /= 10;
        p[4] = '0' + (v %  6); v /=  6;
        p[3] = ':';
        p[2] = '0' + (v % 10); v /= 10;
        p[1] = '0' + (v % 10);
        p += 6;
    }
    *p = 0;
    return dlen;
}

bool TimeISO8601::parse(const char* str, size_t len, timeiso8601_t* tsp) {
    const unsigned char *cur, *end;
    unsigned char ch;
    uint16_t year, month, day, hour, min, sec;
    uint32_t rdn, sod, nsec;
    int16_t offset;

    /*
     *           1
     * 01234567890123456789
     * 2013-12-31T23:59:59Z
     */
    cur = (const unsigned char*)str;
    if (len < 20 || cur[4] != '-' || cur[7] != '-' || cur[13] != ':' || cur[16] != ':') return 1;

    ch = cur[10];
    if (!(ch == 'T' || ch == ' ' || ch == 't')) return 1;

    if (parse_4d(cur, 0, &year) || year < 1 || parse_2d(cur, 5, &month) || month < 1 ||
        month > 12 || parse_2d(cur, 8, &day) || day < 1 || day > 31 || parse_2d(cur, 11, &hour) ||
        hour > 23 || parse_2d(cur, 14, &min) || min > 59 || parse_2d(cur, 17, &sec) || sec > 59)
        return false;

    if (day > 28 && day > days_of_month(year, month)) return false;

    if (month < 3) year--;

    rdn = (1461 * year) / 4 - year / 100 + year / 400 + DayOffset[month] + day - 306;
    sod = hour * 3600 + min * 60 + sec;
    end = cur + len;
    cur = cur + 19;
    offset = nsec = 0;

    ch = *cur++;
    if (ch == '.') {
        const unsigned char* start;
        size_t ndigits;

        start = cur;
        for (; cur < end; cur++) {
            const unsigned char digit = *cur - '0';
            if (digit > 9) break;
            nsec = nsec * 10 + digit;
        }

        ndigits = cur - start;
        if (ndigits < 1 || ndigits > 9) return false;

        nsec *= Pow10[9 - ndigits];

        if (cur == end) return false;

        ch = *cur++;
    }

    if (!(ch == 'Z' || ch == 'z')) {
        /*
         *  01234
         * ±00:00
         */
        if (cur + 5 < end || !(ch == '+' || ch == '-') || cur[2] != ':') return false;

        if (parse_2d(cur, 0, &hour) || hour > 23 || parse_2d(cur, 3, &min) || min > 59) return false;

        offset = hour * 60 + min;
        if (ch == '-') offset *= -1;

        cur += 5;
    }

    if (cur != end) return false;

    tsp->sec = ((int64_t)rdn - 719163) * 86400 + sod - offset * 60;
    tsp->nsec = nsec;
    tsp->offset = offset;

    return true;
}

int TimeISO8601::compare(const timeiso8601_t* t1, const timeiso8601_t* t2) {
    if (t1->sec < t2->sec)
        return -1;
    if (t1->sec > t2->sec)
        return 1;
    if (t1->nsec < t2->nsec)
        return -1;
    if (t1->nsec > t2->nsec)
        return 1;
    return 0;
}

/*
 *          1         2         3
 * 12345678901234567890123456789012345 (+ null-terminator)
 * YYYY-MM-DDThh:mm:ssZ
 * YYYY-MM-DDThh:mm:ss±hh:mm
 * YYYY-MM-DDThh:mm:ss.123Z
 * YYYY-MM-DDThh:mm:ss.123±hh:mm
 * YYYY-MM-DDThh:mm:ss.123456Z
 * YYYY-MM-DDThh:mm:ss.123456±hh:mm
 * YYYY-MM-DDThh:mm:ss.123456789Z
 * YYYY-MM-DDThh:mm:ss.123456789±hh:mm
 */

size_t TimeISO8601::format(char* dst, size_t len, const timeiso8601_t* tsp) {
    uint32_t f;
    int precision;

    if (!valid(tsp))
        return 0;

    f = tsp->nsec;
    if (!f)
        precision = 0;
    else {
        if      ((f % 1000000) == 0) precision = 3;
        else if ((f %    1000) == 0) precision = 6;
        else                         precision = 9;
    }
    return timestamp_format_internal(dst, len, tsp, precision);
}

size_t TimeISO8601::format_precision(char* dst, size_t len, const timeiso8601_t* tsp, int precision) {
    if (!valid(tsp) || precision < 0 || precision > 9)
        return 0;
    return timestamp_format_internal(dst, len, tsp, precision);
}

static struct tm* timeiso8601_to_tm(const timeiso8601_t* tsp, struct tm* tmp, const bool local) {
    uint64_t sec;
    uint32_t rdn, sod;

    if (!TimeISO8601::valid(tsp))
        return NULL;

    sec = tsp->sec + RDN_OFFSET;
    if (local)
        sec += tsp->offset * 60;
    rdn = sec / 86400;
    sod = sec % 86400;

    rdn_to_struct_tm(rdn, tmp);
    tmp->tm_sec  = sod % 60; sod /= 60;
    tmp->tm_min  = sod % 60; sod /= 60;
    tmp->tm_hour = sod;
    return tmp;
}

struct tm* TimeISO8601::to_tm_local(const timeiso8601_t* tsp, struct tm* tmp) {
    return timeiso8601_to_tm(tsp, tmp, true);
}

struct tm* TimeISO8601::to_tm_utc(const timeiso8601_t* tsp, struct tm* tmp) {
    return timeiso8601_to_tm(tsp, tmp, false);
}

bool TimeISO8601::valid(const timeiso8601_t* tsp) {
    const int64_t sec = tsp->sec + tsp->offset * 60;
    if (sec < MIN_SEC || sec > MAX_SEC ||
        tsp->nsec < 0 || tsp->nsec > 999999999 ||
        tsp->offset < -1439 || tsp->offset > 1439)
        return false;
    return true;
}

} // namespace brsdk
