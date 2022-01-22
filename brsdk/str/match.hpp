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
 * @file match.hpp
 * @brief 常用的正则匹配
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <string>

namespace brsdk {

namespace str {

// 网址:[a-zA-z]+://[^\s]*
bool domain_match(const std::string& domain);
// qq:[1-9]\d{4,}
bool qq_match(const std::string& qq);
// html:<(.*)(.*)>.*<\/\1>|<(.*) \/>
bool element_match(const std::string& elm);
// 密码，(由数字/大写字母/小写字母/标点符号组成，四种都必有，8位以上):	(?=^.{8,}$)(?=.*\d)(?=.*\W+)(?=.*[A-Z])(?=.*[a-z])(?!.*\n).*$
bool passwd_match(const std::string& passwd);
// 简单密码:^[a-z0-9_-]{6,18}$
bool simple_passwd_match(const std::string& passwd);
// 用户名:^[a-z0-9_-]{3,16}$
bool username_match(const std::string& passwd);
// 日期（年-月-日）:(\d{4}|\d{2})-((1[0-2])|(0?[1-9]))-(([12][0-9])|(3[01])|(0?[1-9]))
bool date1_match(const std::string& date);
// 日期(月/日/年):((1[0-2])|(0?[1-9]))/(([12][0-9])|(3[01])|(0?[1-9]))/(\d{4}|\d{2})
bool date2_match(const std::string& date);
// 汉字:[\u4e00-\u9fa5]
bool hans_match(const std::string& word);
// 中文及全角标点符号(字符):[\u3000-\u301e\ufe10-\ufe19\ufe30-\ufe44\ufe50-\ufe6b\uff01-\uffee]
bool zh_match(const std::string& zh);
// 中国大陆固定号码:(\d{4}-|\d{3}-)?(\d{8}|\d{7})
bool zh_telephone_match(const std::string& telephone);
// 中国大陆邮编:[1-9]\d{5}
bool zh_postal_match(const std::string& postal);
// 仅字母:^[A-Za-z]+$
bool alphabet_match(const std::string& ch);
// 数字，字母、下划线:^\w+$
bool ch_match(const std::string& ch);

// 邮箱校验
bool email_match(const std::string &email);

// 手机号码校验
bool zh_cellphone_number_match(const std::string &number);

// 身份证校验
bool id_card_match(const std::string &number);

// ipv4 校验
bool ipv4_match(const std::string &ip);

// ipv6 校验
bool ipv6_match(const std::string &ip);

} // namespace str

} // namespace brsdk
