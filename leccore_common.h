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
		static inline std::wstring convert_string(const std::string& string_) {
			int s_length = (int)string_.length() + 1;
			int len = MultiByteToWideChar(CP_ACP, 0, string_.c_str(), s_length, 0, 0);

			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, string_.c_str(), s_length, buf, len);

			std::wstring r(buf);
			delete[] buf;

			return r;
		}

		static inline std::string convert_string(const std::wstring& string_) {
			int s_length = (int)string_.length() + 1;
			int len = WideCharToMultiByte(CP_ACP, 0, string_.c_str(), s_length, 0, 0, 0, 0);

			char* buf = new char[len];
			WideCharToMultiByte(CP_ACP, 0, string_.c_str(), s_length, buf, len, 0, 0);

			std::string r(buf);
			delete[] buf;

			return r;
		}

		template <typename T>
		static inline T smallest(T a, T b) {
			return (((a) < (b)) ? (a) : (b));
		}

		template <typename T>
		static inline T largest(T a, T b) {
			return (((a) > (b)) ? (a) : (b));
		}
	}
}
