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
 * @file flag.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-08-15
 * 
 * @copyright MIT License
 * 
 */
#include "flag.hpp"
#include "brsdk/str/fmt.hpp"
#include "brsdk/str/pystring.hpp"
#include "brsdk/fs/file.hpp"
#include "brsdk/fs/file_util.hpp"
#include "brsdk/os/os.hpp"
#include <stdlib.h>
#include <map>
#include <iostream>

DEF_string(help, "", ".help info");
DEF_string(config, "", ".path of config file");
DEF_bool(mkconf, false, ".generate config file");
DEF_bool(daemon, false, "#0 run program as a daemon");

namespace brsdk {

namespace flag {

namespace xx {

struct Flag {
    Flag(const char* type_str_, const char* name_, const char* value_,
         const char* help_, const char* file_, int line_, int type_, void* addr_)
        : type_str(type_str_), name(name_), value(value_), help(help_),
          file(file_), line(line_), type(type_), addr(addr_), lv(10) {
        const char* const h = help;
        if (h[0] == '#' && '0' <= h[1] && h[1] <= '9') {
            if (h[2] == ' ' || h[2] == '\0') {
                lv = h[1] - '0';
                help += 2 + !!h[2];
            } else if ('0' <= h[2] && h[2] <= '9' && (h[3] == ' ' || h[3] == '\0')) {
                lv = (h[1] - '0') * 10 + (h[2] - '0');
                help += 3 + !!h[3];
            }
        }
    }

    const char* type_str;
    const char* name;
    const char* value;  // default value
    const char* help;   // help info
    const char* file;   // file where the flag is defined
    int line;           // line of the file where the flag is defined
    int type;
    void* addr;         // point to the flag variable
    int lv;             // level: 0-9 for co, 10-99 for users
};

inline std::map<std::string, Flag>& gFlags() {
    static std::map<std::string, Flag> flags; // <name, flag>
    return flags;
}

std::string flag_set_value(Flag* flag, const std::string& v) {
    switch (flag->type) {
      case TYPE_string:
        *static_cast<std::string*>(flag->addr) = v;
        break;
      case TYPE_bool:
        *static_cast<bool*>(flag->addr) = brsdk::str::to_bool(v);
        break;
      case TYPE_int32:
        *static_cast<int32_t*>(flag->addr) = brsdk::str::to_int32(v);
        break;
      case TYPE_uint32:
        *static_cast<uint32_t*>(flag->addr) = brsdk::str::to_uint32(v);
        break;
      case TYPE_int64:
        *static_cast<int64_t*>(flag->addr) = brsdk::str::to_int64(v);
        break;
      case TYPE_uint64:
        *static_cast<uint64_t*>(flag->addr) = brsdk::str::to_uint64(v);
        break;
      case TYPE_double:
        *static_cast<double*>(flag->addr) = brsdk::str::to_double(v);
        break;
      default:
        return "unknown flag type";
    }

    if (errno == 0) return std::string();
    if (errno == ERANGE) return "out of range";
    return "invalid number";
}

template<typename T>
std::string int_to_string(T t) {
    if ((0 <= t && t <= 8192) || (t < 0 && static_cast<int>(t) >= -8192)) return std::to_string(t);

    const char* u = "kmgtp";
    int i = -1;
    while (t != 0 && (t & 1023) == 0) {
        t >>= 10;
        if (++i >= 4) break;
    }

    return i < 0 ? std::to_string(t) : std::to_string(t) + u[i];
}

std::string flag_get_value(const Flag* flag) {
    switch (flag->type) {
      case TYPE_string:
        return *static_cast<std::string*>(flag->addr);
      case TYPE_bool:
        return std::to_string(*static_cast<bool*>(flag->addr));
      case TYPE_int32:
        return int_to_string(*static_cast<int32_t*>(flag->addr));
      case TYPE_uint32:
        return int_to_string(*static_cast<uint32_t*>(flag->addr));
      case TYPE_int64:
        return int_to_string(*static_cast<int64_t*>(flag->addr));
      case TYPE_uint64:
        return int_to_string(*static_cast<uint64_t*>(flag->addr));
      case TYPE_double:
        return std::to_string(*static_cast<double*>(flag->addr));
      default:
        return "unknown flag type";
    }
}

std::string flag_to_str(const Flag* flag) {
	std::string ret = "--";
	ret = ret + flag->name + ": " + flag->help + "\n\t type: " + flag->type_str + "\t     default: " + flag->value + "\n\t from: " + flag->file;
	return ret;
}

void add_flag(const char* type_str, const char* name, const char* value,
              const char* help, const char* file, int line, int type, void* addr) {
    auto r = gFlags().insert(
        std::make_pair(std::string(name), Flag(type_str, name, value, help, file, line, type, addr))
    );

    if (!r.second) {
        std::cout << "multiple definitions of flag: " << name
             << ", from " << r.first->second.file << " and " << file << std::endl;
        exit(0);
    }
}

Flag* find_flag(const std::string& name) {
    auto it = gFlags().find(name);
    if (it != gFlags().end()) return &it->second;
    return NULL;
}

// Return error message on any error.
std::string set_flag_value(const std::string& name, const std::string& value) {
    Flag* flag = find_flag(name);
    if (!flag) return "flag not defined: " + name;

    std::string err = flag_set_value(flag, value);
    if (!err.empty()) err.append(": ").append(value);
    return err;
}

// set_bool_flags("abc"):  -abc -> true  or  -a, -b, -c -> true
std::string set_bool_flags(const std::string& name) {
    Flag* flag = find_flag(name);
    if (flag) {
        if (flag->type == TYPE_bool) {
            *static_cast<bool*>(flag->addr) = true;
            return std::string();
        } else {
            return std::string("value not set for non-bool flag: -").append(name);
        }
    }

    if (name.size() == 1) {
        return std::string("undefined bool flag -").append(name);
    }

    for (size_t i = 0; i < name.size(); ++i) {
        flag = find_flag(name.substr(i, 1));
        if (!flag) {
            return std::string("undefined bool flag -") + name[i] + " in -" + name;
        } else if (flag->type != TYPE_bool) {
            return std::string("-") + name[i] + " is not bool in -" + name;
        } else {
            *static_cast<bool*>(flag->addr) = true;
        }
    }

    return std::string();
}

void show_flags_info() {
    for (auto it = gFlags().begin(); it != gFlags().end(); ++it) {
        const auto& flag = it->second;
        if (*flag.help != '\0') std::cout << flag_to_str(&flag) << std::endl;
    }
}

void show_help_info(const std::string& exe) {
    if (!FLG_help.empty()) {
        std::cout << FLG_help << std::endl;
        return;
    }

    std::string s(exe);
  #ifdef _WIN32
    if (s.size() > 1 && s[1] == ':') {
        size_t p = s.rfind('/');
        if (p == s.npos) p = s.rfind('\\');
        if (p != s.npos) s = "./" + s.substr(p + 1);
    }
  #endif
    std::cout << "usage:\n"
         << "    " << s << " --                   print flags info\n"
         << "    " << s << " --help               print help info\n"
         << "    " << s << " --mkconf             generate config file\n"
         << "    " << s << " --daemon             run as a daemon (Linux)\n"
         << "    " << s << " xx.conf              run with config file\n"
         << "    " << s << " config=xx.conf       run with config file\n"
         << "    " << s << " -x -i=8k -s=ok       run with commandline flags\n"
         << "    " << s << " -x -i 8k -s ok       run with commandline flags\n"
         << "    " << s << " x=true i=8192 s=ok   run with commandline flags\n";
}

std::string format_str(const std::string& s) {
    size_t px = s.find('"');
    size_t py = s.find('\'');
    size_t pz = s.find('`');
    if (px == s.npos && py == s.npos && pz == s.npos) return s;
    if (px == s.npos) return std::string("\"").append(s).append("\"");
    if (py == s.npos) return std::string("'").append(s).append("'");
    if (pz == s.npos) return std::string("`").append(s).append("`");
    return std::string("```") + s + "```";
}

#define COMMENT_LINE_LEN 72

void mkconf(const std::string& exe) {
    // Order flags by lv, file, line.  <lv, <file, <line, flag>>>
    std::map<int, std::map<std::string, std::map<int, Flag*>>> flags;
    for (auto it = gFlags().begin(); it != gFlags().end(); ++it) {
        Flag* f = &it->second;
        if (f->help[0] == '.' || f->help[0] == '\0') continue; // ignore hidden flags.
        flags[f->lv][std::string(f->file)][f->line] = f;
    }

    std::string fname(exe);
	if (pystring::endswith(fname, ".exe")) fname.resize(fname.size() - 4);
    fname += ".conf";

    brsdk::fs::File f;
	if (f.open(fname.c_str(), "w") < 0) {
        std::cout << "can't open config file: " << fname << std::endl;
        return;
    }

    std::ostringstream ostr;
	ostr << std::string(COMMENT_LINE_LEN, '#') << '\n'
      << "###  > # or // for comments\n"
      << "###  > k,m,g,t,p (case insensitive, 1k for 1024, etc.)\n"
      << std::string(COMMENT_LINE_LEN, '#') << "\n\n\n";
	
    for (auto it = flags.begin(); it != flags.end(); ++it) {
        const auto& x = it->second;
        for (auto xit = x.begin(); xit != x.end(); ++xit) {
            const auto& y = xit->second;
            ostr << "# >> " << pystring::replace(xit->first, "\\", "/") << '\n';
            ostr << "#" << std::string(COMMENT_LINE_LEN - 1, '=') << '\n';
            for (auto yit = y.begin(); yit != y.end(); ++yit) {
                const Flag& flag = *yit->second;
                std::string v = flag_get_value(&flag);
                if (flag.type == TYPE_string) v = format_str(v);
                ostr << "# " << pystring::replace(flag.help, "\n", "\n# ") << '\n';
                ostr << flag.name << " = " << v << "\n\n";
            }
            ostr << "\n";
        }
    }

	f.write(ostr.str().c_str(), ostr.str().length());
}

#undef COMMENT_LINE_LEN


FlagSaver::FlagSaver(const char* type_str, const char* name, const char* value,
                     const char* help, const char* file, int line, int type, void* addr) {
    add_flag(type_str, name, value, help, file, line, type, addr);
}

// @kv:     for -a=b, or -a b, or a=b
// @bools:  for -a, -xyz
// return non-flag elements (etc. hello, -8, -8k, -, --, --- ...)
std::vector<std::string> analyze(
    const std::vector<std::string>& args, std::map<std::string, std::string>& kv, std::vector<std::string>& bools
) {
    std::vector<std::string> res;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        size_t bp = arg.find_first_not_of('-');
        size_t ep = arg.find('=');

        // @arg has only '-':  for -, --, --- ...
        if (bp == arg.npos) {
            res.push_back(arg);
            continue;
        }

        if (ep <= bp) {
            std::cout << "invalid parameter" << ": " << arg << std::endl;
            exit(0);
        }

        // @arg has '=', for -a=b or a=b
        if (ep != arg.npos) {
            kv[arg.substr(bp, ep - bp)] = arg.substr(ep + 1);
            continue;
        }

        // non-flag: etc. hello, -8, -8k ...
        if (bp == 0 || (bp == 1 && '0' <= arg[1] && arg[1] <= '9')) {
            res.push_back(arg);
            continue;
        }

        // flag: -a, -a b, or -j4
        {
            Flag* flag = 0;
            std::string next;
            std::string name = arg.substr(bp);

            // for -j4
            if (name.size() > 1 && (('0' <= name[1] && name[1] <= '9') || name[1] == '-')) {
                if (!find_flag(name) && find_flag(name.substr(0, 1))) {
                    kv[name.substr(0, 1)] = name.substr(1);
                    continue;
                }
            }

            if (i + 1 == args.size()) goto no_value;

            next = args[i + 1];
            if (next.find('=') != next.npos) goto no_value;
			if (pystring::startswith(next, "-") && next.find_first_not_of('-') != next.npos) {
                if (next[1] < '0' || next[1] > '9') goto no_value;
			}

            flag = find_flag(name);
            if (!flag) goto no_value;
            if (flag->type != TYPE_bool) goto has_value;
            if (next == "0" || next == "1" || next == "false" || next == "true") goto has_value;

          no_value:
            bools.push_back(name);
            continue;

          has_value:
            kv[name] = next;
            ++i;
            continue;
        };
    }

    return res;
}

void parse_config(const std::string& config);

std::vector<std::string> parse_command_line_flags(int argc, char** argv) {
    if (argc <= 1) return std::vector<std::string>();

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }

    std::string exe(argv[0]);

    if (args.size() == 1) {
        const std::string& arg = args[0];

        if (arg == "--help") {
            show_help_info(exe);
            exit(0);
        }

        if (arg.find_first_not_of('-') == arg.npos) {
            show_flags_info();
            exit(0);
        }
    }

    std::map<std::string, std::string> kv;
    std::vector<std::string> bools;
    std::vector<std::string> v = analyze(args, kv, bools);

    auto it = kv.find("config");
    if (it != kv.end()) {
        FLG_config = it->second;
    } else if (!v.empty()) {
		if (pystring::endswith(v[0], ".conf") || pystring::endswith(v[0], "config")) {
            if (brsdk::fs::exists(v[0].c_str())) FLG_config = v[0];
		}
    }

    if (!FLG_config.empty()) parse_config(FLG_config);

    for (it = kv.begin(); it != kv.end(); ++it) {
        std::string err = set_flag_value(it->first, it->second);
        if (!err.empty()) {
            std::cout << err << std::endl;
            exit(0);
        }
    }

    for (size_t i = 0; i < bools.size(); ++i) {
        std::string err = set_bool_flags(bools[i]);
        if (!err.empty()) {
            std::cout << err << std::endl;
            exit(0);
        }
    }

    return v;
}

std::string remove_quotes_and_comments(const std::string& s) {
    if (s.empty()) return s;

    std::string r;
    size_t p, q, l;
    char c = s[0];

    if (c == '"' || c == '\'' || c == '`') {
		if (pystring::startswith(s, "```")) {
            p = s.find("```", 3);
            l = 3;
        } else {
            p = s.find(c, 1);
            l = 1;
        }

        if (p == s.npos) goto no_quotes;

        p = s.find_first_not_of(" \t", p + l);
        if (p == s.npos) {
			r = pystring::rstrip(s, " \t");
        } else if (s[p] == '#' || s.substr(p, 2) == "//") {
            r = pystring::rstrip(s.substr(0, p), " \t");
        } else {
            goto no_quotes;
        }

        return r.substr(l, r.size() - l * 2);
    }

  no_quotes:
    p = s.find('#');
    q = s.find("//");
    if (p == s.npos && q == s.npos) return s;
    return pystring::rstrip(s.substr(0, p < q ? p : q), " \t");
}

std::string getline(std::vector<std::string>& lines, size_t& n) {
    std::string line;

    while (n < lines.size()) {
        std::string s(lines[n++]);
		pystring::replace(s, "　", " ");   // replace Chinese spaces
		pystring::strip(s);

        if (s.empty() || s.back() != '\\') {
            line += s;
            return line;
        }

		line += pystring::rstrip(s, " \t\r\n\\");
    }

    return line;
}

void parse_config(const std::string& config) {
	brsdk::fs::File f;

	if (f.open(config.c_str(), "r") < 0) {
        std::cout << "can't open config file: " << config << std::endl;
        exit(0);
    }

    std::string data;
	f.readall(data);

	std::string sep("\n");
    if (data.find('\n') == data.npos && data.find('\r') != data.npos) sep = "\r";

    std::vector<std::string> lines;
	pystring::split(data, lines, sep);
    size_t lineno = 0; // line number of config file.

    for (size_t i = 0; i < lines.size();) {
        lineno = i;
        std::string s = lines[i];
        if (s.empty() || s[0] == '#' || pystring::startswith(s, "//")) continue;

        size_t p = s.find('=');
        if (p == 0 || p == s.npos) {
            std::cout << "invalid config: " << s << ", at " << config << ':' << (lineno + 1) << std::endl;
            exit(0);
        }

        std::string flg = pystring::rstrip(s.substr(0, p), " \t");
        std::string val = pystring::lstrip(s.substr(p + 1), " \t");
        val = remove_quotes_and_comments(val);

        std::string err = set_flag_value(flg, val);
        if (!err.empty()) {
            std::cout << err << ", at " << config << ':' << (lineno + 1) << std::endl;
            exit(0);
        }
    }
}

} // namespace xx

std::vector<std::string> init(int argc, char** argv) {
    std::vector<std::string> v = xx::parse_command_line_flags(argc, argv);

    if (FLG_mkconf) {
        xx::mkconf(argv[0]);
        exit(0);
    }

    if (FLG_daemon) brsdk::os::daemon();
    return v;
}

void init(const std::string& path) {
    xx::parse_config(path);
}

} // namespace flag

} // namespace brsdk
