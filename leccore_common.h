//
// leccore_common.h - common leccore functions
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#include <string>
#include <Windows.h>

namespace liblec {
	namespace leccore {
		static inline std::wstring convert_string(const std::string& input) {
			int s_length = (int)input.length() + 1;
			int len = MultiByteToWideChar(CP_ACP, 0, input.c_str(), s_length, 0, 0);

			wchar_t* buffer = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, input.c_str(), s_length, buffer, len);

			std::wstring output(buffer);
			delete[] buffer;

			return output;
		}

		static inline std::string convert_string(const std::wstring& input) {
			int s_length = (int)input.length() + 1;
			int len = WideCharToMultiByte(CP_ACP, 0, input.c_str(), s_length, 0, 0, 0, 0);

			char* buffer = new char[len];
			WideCharToMultiByte(CP_ACP, 0, input.c_str(), s_length, buffer, len, 0, 0);

			std::string output(buffer);
			delete[] buffer;

			return output;
		}

		template <typename T>
		static inline T smallest(T a, T b) {
			return (((a) < (b)) ? (a) : (b));
		}

		template <typename T>
		static inline T largest(T a, T b) {
			return (((a) > (b)) ? (a) : (b));
		}

		static inline void trim(std::string& s) {
			auto trim_left = [](std::string& s) {
				s.erase(s.begin(), std::find_if(s.begin(), s.end(),
					[](unsigned char ch) { return !std::isspace(ch); }));
			};

			auto trim_right = [](std::string& s) {
				s.erase(std::find_if(s.rbegin(), s.rend(),
					[](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
			};

			trim_left(s);
			trim_right(s);
		}
	}
}
