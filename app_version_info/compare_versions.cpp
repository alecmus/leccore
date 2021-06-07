//
// compare_versions.cpp - version number comparison implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

/*
*  Based on WinSparkle (http://winsparkle.org)
*
*  Copyright (C) 2009-2014 Vaclav Slavik
*
*  Permission is hereby granted, free of charge, to any person obtaining a
*  copy of this software and associated documentation files (the "Software"),
*  to deal in the Software without restriction, including without limitation
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,
*  and/or sell copies of the Software, and to permit persons to whom the
*  Software is furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*  DEALINGS IN THE SOFTWARE.
*
*/

#include "../app_version_info.h"
#include "../leccore_common.h"
#include <vector>

namespace liblec {
	namespace leccore {
		// String characters classification. Valid components of version numbers
		// are numbers, period or string fragments ("beta" etc.).
		enum class char_type {
			type_number,
			type_period,
			type_string
		};

		char_type classify_char(char c) {
			if (c == '.')
				return char_type::type_period;
			else if (c >= '0' && c <= '9')
				return char_type::type_number;
			else
				return char_type::type_string;
		}

		// Split version string into individual components. A component is continuous
		// run of characters with the same classification. For example, "1.20rc3" would
		// be split into ["1",".","20","rc","3"]
		std::vector<std::string> split_version_string(const std::string& version) {
			std::vector<std::string> list;

			if (version.empty())
				return list;

			std::string s;
			const size_t len = version.length();

			s = version[0];
			char_type previous_type = classify_char(version[0]);

			for (size_t i = 1; i < len; i++) {
				const char c = version[i];
				const char_type new_type = classify_char(c);

				if (previous_type != new_type ||
					previous_type == char_type::type_period) {
					// we have reached a new segment. Periods get a special treatment
					// as delimiters (and so ".." means there's empty component value)
					list.push_back(s);
					s = c;
				}
				else
					s += c;	// Add character to current segment and continue.

				previous_type = new_type;
			}

			// add the last part
			list.push_back(s);
			return list;
		}

		int compare_versions(const std::string& left,
			const std::string& right) {
			if (left == right)
				return 0;	// trivial case

			const std::vector<std::string> parts_left = split_version_string(left);
			const std::vector<std::string> parts_right = split_version_string(right);

			// compare common length of both version strings
			const size_t n = min(parts_left.size(), parts_right.size());

			for (size_t i = 0; i < n; i++) {
				const std::string& left_ = parts_left[i];
				const std::string& right_ = parts_right[i];

				const char_type type_left = classify_char(left_[0]);
				const char_type type_right = classify_char(right_[0]);

				if (type_left == type_right) {
					if (type_left == char_type::type_string) {
						int result = left_.compare(right_);
						if (result != 0)
							return result;
					}
					else if (type_left == char_type::type_number) {
						const int int_left = atoi(left_.c_str());
						const int int_right = atoi(right_.c_str());
						if (int_left > int_right)
							return 1;
						else if (int_left < int_right)
							return -1;
					}
				}
				else {
					// components of different types
					if (type_left != char_type::type_string && type_right == char_type::type_string)
						return 1;	// 1.2.0 > 1.2rc1
					else if (type_left == char_type::type_string && type_right != char_type::type_string)
						return -1;	// 1.2rc1 < 1.2.0
					else {
						// One is a number and the other is a period. The period is invalid.
						return (type_left == char_type::type_number) ? 1 : -1;
					}
				}
			}

			// The versions are equal up to the point where they both still have
			// parts. Lets check to see if one is larger than the other.
			if (parts_left.size() == parts_right.size())
				return 0; // the two strings are identical

			// Lets get the next part of the larger version string
			// Note that 'n' already holds the index of the part we want.
			int shorter_result, longer_result;
			char_type missing_part_type; // ('missing' as in "missing in shorter version")

			if (parts_left.size() > parts_right.size()) {
				missing_part_type = classify_char(parts_left[n][0]);
				shorter_result = -1;
				longer_result = 1;
			}
			else {
				missing_part_type = classify_char(parts_right[n][0]);
				shorter_result = 1;
				longer_result = -1;
			}

			if (missing_part_type == char_type::type_string)
				return shorter_result;	// 1.5 > 1.5b3
			else
				return longer_result;	// 1.5.1 > 1.5
		}
	}
}
