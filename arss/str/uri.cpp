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
 * @file uri.cpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */

// RFC3986 Uniform Resource Identifier (URI): Generic Syntax => 2. Characters (p11)
// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
// reserved = gen-delims / sub-delims
// gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="

#include "uri.hpp"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

namespace arss {

namespace str {

#define N 64

/*
0000 0000 0000 0000 0000 0000 0000 0000

?>=< ;:98 7654 3210 /.-, +*)( '&%$ #"!
1010 1111 1111 1111 1111 1111 1111 1010

_^]\ [ZYX WVUT SRQP ONML KJIH GFED CBA@
1010 1111 1111 1111 1111 1111 1111 1111

 ~}| {zyx wvut srqp onml kjih gfed cba`
0100 0111 1111 1111 1111 1111 1111 1110

0000 0000 0000 0000 0000 0000 0000 0000
0000 0000 0000 0000 0000 0000 0000 0000
0000 0000 0000 0000 0000 0000 0000 0000
0000 0000 0000 0000 0000 0000 0000 0000
*/
static uint32_t s_characters[8] = {0x00, 0xAFFFFFFA, 0xAFFFFFFF, 0x47FFFFFE,
                                   0x00, 0x00,       0x00,       0x00};

static int uri_characters_check(uint8_t c) {
    int n = c / 32;
    int m = c % 32;
    return s_characters[n] & (1 << m);
}

static int uri_parse_complex(struct uri_t* uri, const char* str, int len);

struct uri_t* uri_parse(const char* uri, int len) {
    struct uri_t* u;

    if (NULL == uri || 0 == *uri || len < 1) return NULL;

    u = (struct uri_t*)malloc(sizeof(*u) + len + 5);
    if (NULL == u) return NULL;

    if (0 != uri_parse_complex(u, uri, len)) {
        free(u);
        return NULL;
    }

    return u;
}

void uri_free(struct uri_t* uri) {
    if (uri) free(uri);
}

struct uri_component_t {
    int has_scheme;
    int has_userinfo;
    int has_host;
    int has_port;
};

static int uri_check(const char* str, int len, struct uri_component_t* items) {
    char c;
    const char* pend;
    enum {
        URI_PARSE_START,
        URI_PARSE_SCHEME,
        URI_PARSE_AUTHORITY,
        URI_PARSE_USERINFO,
        URI_PARSE_HOST,
        URI_PARSE_PORT,

        URI_PARSE_HOST_IPV6,
    } state;

    state = URI_PARSE_START;
    items->has_scheme = 0;
    items->has_userinfo = 0;
    items->has_host = 0;
    items->has_port = 0;

    for (pend = str + len; str < pend; ++str) {
        c = *str;

        if (0 == uri_characters_check(c)) return -1;  // invalid character

        switch (state) {
            case URI_PARSE_START:
                switch (c) {
                    case '/':
                        // path only, all done
                        return 0;  // ok

                    case '[':
                        state = URI_PARSE_HOST_IPV6;
                        items->has_host = 1;
                        break;

                    default:
                        state = URI_PARSE_SCHEME;
                        items->has_host = 1;
                        --str;  // go back
                        break;
                }
                break;

            case URI_PARSE_SCHEME:
                switch (c) {
                    case ':':
                        state = URI_PARSE_AUTHORITY;
                        break;

                    case '@':
                        state = URI_PARSE_HOST;
                        items->has_userinfo = 1;
                        break;

                    case '/':
                    case '?':
                    case '#':
                        // all done, host only
                        return 0;

                    default:
                        break;
                }
                break;

            case URI_PARSE_AUTHORITY:
                if ('/' == c) {
                    if (str + 1 < pend && '/' == str[1]) {
                        state = URI_PARSE_HOST;
                        items->has_scheme = 1;
                        str += 1;  // skip "//"
                    } else {
                        items->has_port = 1;
                        return 0;
                    }
                } else {
                    items->has_port = 1;
                    state = URI_PARSE_PORT;
                }
                break;

            case URI_PARSE_HOST:
                assert(']' != c);
                switch (c) {
                    case '@':
                        items->has_userinfo = 1;
                        // state = URI_PARSE_HOST;
                        break;

                    case '[':
                        state = URI_PARSE_HOST_IPV6;
                        break;

                    case ':':
                        items->has_port = 1;
                        state = URI_PARSE_PORT;
                        break;

                    case '/':
                    case '?':
                    case '#':
                        return 0;

                    default:
                        break;
                }
                break;

            case URI_PARSE_PORT:
                switch (c) {
                    case '@':
                        items->has_port = 0;
                        items->has_userinfo = 1;
                        state = URI_PARSE_HOST;
                        break;

                    case '[':
                    case ']':
                    case ':':
                        return -1;

                    case '/':
                    case '?':
                    case '#':
                        items->has_port = 1;
                        return 0;

                    default:
                        break;
                }
                break;

            case URI_PARSE_HOST_IPV6:
                switch (c) {
                    case ']':
                        state = URI_PARSE_HOST;
                        break;

                    case '@':
                    case '[':
                    case '/':
                    case '?':
                    case '#':
                        return -1;

                    default:
                        break;
                }
                break;

            default:
                assert(0);
                return -1;
        }
    }

    return 0;
}

static int uri_parse_complex(struct uri_t* uri, const char* str, int len) {
    char* p;
    const char* pend;
    struct uri_component_t items;

    if (0 != uri_check(str, len, &items)) return -1;  // invalid uri

    pend = str + len;
    p = (char*)(uri + 1);

    // scheme
    if (items.has_scheme) {
        uri->scheme = p;
        while (str < pend && ':' != *str) *p++ = *str++;
        *p++ = 0;
        str += 3;  // skip "://"
    } else {
        uri->scheme = NULL;
    }

    // user info
    if (items.has_userinfo) {
        uri->userinfo = p;
        while (str < pend && '@' != *str) *p++ = *str++;
        *p++ = 0;
        str += 1;  // skip "@"
    } else {
        uri->userinfo = NULL;
    }

    // host
    if (items.has_host) {
        uri->host = p;
        assert(str < pend);
        if ('[' == *str) {
            // IPv6
            ++str;
            while (str < pend && ']' != *str) *p++ = *str++;
            *p++ = 0;
            str += 1;  // skip "]"

            if (str < pend && *str && NULL == strchr(":/?#", *str)) return -1;
        } else {
            while (str < pend && *str && NULL == strchr(":/?#", *str)) *p++ = *str++;
            *p++ = 0;
        }
    } else {
        uri->host = NULL;
    }

    // port
    if (items.has_port) {
        ++str;  // skip ':'
        for (uri->port = 0; str < pend && *str >= '0' && *str <= '9'; str++)
            uri->port = uri->port * 10 + (*str - '0');

        if (str < pend && *str && NULL == strchr(":/?#", *str)) return -1;
    } else {
        uri->port = 0;
    }

    // 3.3. Path (p22)
    // The path is terminated by the first question mark ("?")
    // or number sign ("#") character,
    // or by the end of the URI.
    uri->path = p;  // awayls have path(default '/')
    if (str < pend && '/' == *str) {
        while (str < pend && *str && '?' != *str && '#' != *str) *p++ = *str++;
        *p++ = 0;
    } else {
        // default path
        *p++ = '/';
        *p++ = 0;
    }

    // 3.4. Query (p23)
    // The query component is indicated by the first question mark ("?") character
    // and terminated by a number sign ("#") character
    // or by the end of the URI.
    if (str < pend && '?' == *str) {
        uri->query = p;
        for (++str; str < pend && *str && '#' != *str; ++str) *p++ = *str;
        *p++ = 0;
    } else {
        uri->query = NULL;
    }

    // 3.5. Fragment
    if (str < pend && '#' == *str) {
        uri->fragment = p;
        while (str < pend && *++str) *p++ = *str;
        *p++ = 0;
    } else {
        uri->fragment = NULL;
    }

    return 0;
}

int uri_path(const struct uri_t* uri, char* buf, int len) {
    int r, n;
    n = snprintf(buf, len, "%s", uri->path);
    if (n < 0 || n >= len) return -1;

    if (uri->query && *uri->query) {
        r = snprintf(buf + n, len - n, "?%s", uri->query);
        if (r < 0 || r + n >= len) return -1;
        n += r;
    }

    if (uri->fragment && *uri->fragment) {
        r = snprintf(buf + n, len - n, "#%s", uri->fragment);
        if (r < 0 || r + n >= len) return -1;
        n += r;
    }

    return n;
}

int uri_userinfo(const struct uri_t* uri, char* usr, int n1, char* pwd, int n2) {
    const char* sep;
    if (!uri->userinfo) {
        usr[0] = 0;
        pwd[0] = 0;
    } else {
        sep = strchr(uri->userinfo, ':');
        if (sep) {
            snprintf(usr, n1, "%.*s", (int)(sep - uri->userinfo), uri->userinfo);
            snprintf(pwd, n2, "%s", sep + 1);
        } else {
            snprintf(usr, n1, "%s", uri->userinfo);
            pwd[0] = 0;
        }
    }

    return 0;
}

int uri_query(const char* query, const char* end, struct uri_query_t** items) {
    int count;
    int capacity;
    const char* p;
    struct uri_query_t items0[N], *pp;

    assert(items);
    *items = NULL;
    capacity = count = 0;

    for (p = query; p && p < end; query = p + 1) {
        p = strpbrk(query, "&=");
        assert(!p || *p);
        if (!p || p > end) break;

        if (p == query) {
            if ('&' == *p) {
                continue;  // skip k1=v1&&k2=v2
            } else {
                uri_query_free(items);
                return -1;  // no-name k1=v1&=v2
            }
        }

        if (count < N) {
            pp = &items0[count++];
        } else {
            if (count >= capacity) {
                capacity = count + 64;
                pp = (struct uri_query_t*)realloc(*items, capacity * sizeof(struct uri_query_t));
                if (!pp) return -ENOMEM;
                *items = pp;
            }

            pp = &(*items)[count++];
        }

        pp->name = query;
        pp->n_name = (int)(p - query);

        if ('=' == *p) {
            pp->value = p + 1;
            p = strchr(pp->value, '&');
            if (NULL == p) p = end;
            pp->n_value = (int)(p - pp->value);  // empty-value
        } else {
            assert('&' == *p);
            pp->value = NULL;
            pp->n_value = 0;  // no-value
        }
    }

    if (count <= N && count > 0) {
        *items = (struct uri_query_t*)malloc(count * sizeof(struct uri_query_t));
        if (!*items) return -ENOMEM;
        memcpy(*items, items0, count * sizeof(struct uri_query_t));
    } else if (count > N) {
        memcpy(*items, items0, N * sizeof(struct uri_query_t));
    }

    return count;
}

void uri_query_free(struct uri_query_t** items) {
    if (items && *items) {
        free(*items);
        *items = NULL;
    }
}

#if defined(DEBUG) || defined(_DEBUG)
void uri_query_test(void) {
    const char* s1 = "";
    const char* s2 = "name=value&a=b&";
    const char* s3 = "name=value&&a=b&";
    const char* s4 = "name=value&&=b&";
    const char* s5 = "name=value&k1=v1&k2=v2&k3=v3&k4&k5&k6=v6";

    struct uri_query_t* items;
    assert(0 == uri_query(s1, s1 + strlen(s1), &items));
    uri_query_free(&items);

    assert(2 == uri_query(s2, s2 + strlen(s2), &items));
    assert(0 == strncmp("name", items[0].name, items[0].n_name) &&
           0 == strncmp("value", items[0].value, items[0].n_value));
    assert(0 == strncmp("a", items[1].name, items[1].n_name) &&
           0 == strncmp("b", items[1].value, items[1].n_value));
    uri_query_free(&items);

    assert(2 == uri_query(s3, s3 + strlen(s4), &items));
    assert(0 == strncmp("name", items[0].name, items[0].n_name) &&
           0 == strncmp("value", items[0].value, items[0].n_value));
    assert(0 == strncmp("a", items[1].name, items[1].n_name) &&
           0 == strncmp("b", items[1].value, items[1].n_value));
    uri_query_free(&items);

    assert(-1 == uri_query(s4, s4 + strlen(s4), &items));
    uri_query_free(&items);

    assert(7 == uri_query(s5, s5 + strlen(s5), &items));
    assert(0 == strncmp("name", items[0].name, items[0].n_name) &&
           0 == strncmp("value", items[0].value, items[0].n_value));
    assert(0 == strncmp("k1", items[1].name, items[1].n_name) &&
           0 == strncmp("v1", items[1].value, items[1].n_value));
    assert(0 == strncmp("k2", items[2].name, items[2].n_name) &&
           0 == strncmp("v2", items[2].value, items[2].n_value));
    assert(0 == strncmp("k3", items[3].name, items[3].n_name) &&
           0 == strncmp("v3", items[3].value, items[3].n_value));
    assert(0 == strncmp("k4", items[4].name, items[4].n_name) && 0 == items[4].n_value &&
           NULL == items[4].value);
    assert(0 == strncmp("k5", items[5].name, items[5].n_name) && 0 == items[5].n_value &&
           NULL == items[5].value);
    assert(0 == strncmp("k6", items[6].name, items[6].n_name) &&
           0 == strncmp("v6", items[6].value, items[6].n_value));
    uri_query_free(&items);
}
#endif

}  // namespace str

}  // namespace arss
