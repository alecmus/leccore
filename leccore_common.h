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
#include <sstream>

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

		/// <summary>Rounding off class.</summary>
		class roundoff {
		public:
			/// <summary>Round-off a double to a string.</summary>
			/// <param name="d">The double to round-off.</param>
			/// <param name="precision">The number of decimal places to round it off to.</param>
			/// <returns>The rounded-off value, as a string.</returns>
			template <typename T>
			static std::basic_string<T> tostr(const double& d, int precision) {
				std::basic_stringstream<T> ss;
				ss << std::fixed;
				ss.precision(precision);
				ss << d;
				return ss.str();
			}

			/// <summary>Round-off a double to another double.</summary>
			/// <param name="d">The double to round-off.</param>
			/// <param name="precision">The number of decimal places to round it off to.</param>
			/// <returns>The rounded-off value.</returns>
			static double tod(const double& d, int precision) {
				int y = (int)d;
				double z = d - (double)y;
				double m = pow(10, precision);
				double q = z * m;
				double r = round(q);

				return static_cast<double>(y) + (1.0 / m) * r;
			}

			/// <summary>Round-off a float to another float.</summary>
			/// <param name="d">The float to round-off.</param>
			/// <param name="precision">The number of decimal places to round it off to.</param>
			/// <returns>The rounded-off value.</returns>
			static float tof(const float& f, int precision) {
				return static_cast<float>(tod(static_cast<double>(f), precision));
			}
		};
	}
}
