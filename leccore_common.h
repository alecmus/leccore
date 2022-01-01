//
// leccore_common.h - common leccore functions
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#include <Windows.h>
#include <GdiPlus.h>
#include <string>

namespace liblec {
	namespace leccore {
		template<class Interface>
		static inline void safe_release(Interface** pp_interface_to_release) {
			if (*pp_interface_to_release != nullptr) {
				(*pp_interface_to_release)->Release();
				(*pp_interface_to_release) = nullptr;
			}
		}

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

		class auto_mutex;

		/// <summary>
		/// A mutex object with no publicly accessible methods.
		/// Can only be used by the <see cref="auto_mutex"/> class.
		/// </summary>
		class mutex {
		public:
			mutex();
			~mutex();

		private:
			friend auto_mutex;
			class impl;
			impl& _d;
		};

		/// <summary>
		/// A mutex class that automatically unlocks the mutex when it's out of scope.
		/// </summary>
		/// 
		/// <remarks>
		/// Usage example to prevent multiple threads from accessing a function at the same moment:
		/// 
		/// liblec::mutex print_mutex;
		/// 
		/// void print() {
		///		liblec::auto_mutex lock(print_mutex);
		/// 
		///		// do printing
		/// 
		///		return;
		/// }
		/// 
		/// </remarks>
		class auto_mutex {
		public:
			auto_mutex(mutex& mtx);
			~auto_mutex();

		private:
			class impl;
			impl& _d;
		};

		// get detailed Gdiplus status information
		static inline std::string get_gdiplus_status_info(Gdiplus::Status* pstatus) {
			using namespace Gdiplus;

			std::string msg;

			switch (*pstatus) {
			case Ok:
				msg = "Ok: Indicates that the method call was successful.";
				break;
			case GenericError:
				msg = "GenericError";
				break;
			case InvalidParameter:
				msg = "InvalidParameter";
				break;
			case OutOfMemory:
				msg = "OutOfMemory";
				break;
			case ObjectBusy:
				msg = "ObjectBusy";
				break;
			case InsufficientBuffer:
				msg = "InsufficientBuffer";
				break;
			case NotImplemented:
				msg = "NotImplemented";
				break;
			case Win32Error:
				msg = "Win32Error";
				break;
			case WrongState:
				msg = "WrongState";
				break;
			case Aborted:
				msg = "Aborted";
				break;
			case FileNotFound:
				msg = "FileNotFound";
				break;
			case ValueOverflow:
				msg = "ValueOverflow";
				break;
			case AccessDenied:
				msg = "AccessDenied";
				break;
			case UnknownImageFormat:
				msg = "UnknownImageFormat";
				break;
			case FontFamilyNotFound:
				msg = "FontFamilyNotFound";
				break;
			case FontStyleNotFound:
				msg = "FontStyleNotFound";
				break;
			case NotTrueTypeFont:
				msg = "NotTrueTypeFont";
				break;
			case UnsupportedGdiplusVersion:
				msg = "UnsupportedGdiplusVersion";
				break;
			case GdiplusNotInitialized:
				msg = "GdiplusNotInitialized";
				break;
			case PropertyNotFound:
				msg = "PropertyNotFound";
				break;
			case PropertyNotSupported:
				msg = "PropertyNotSupported";
				break;
#if (GDIPVER >= 0x0110)
			case ProfileNotFound:
				msg = "ProfileNotFound";
				break;
#endif //(GDIPVER >= 0x0110)
			default:
				msg = "Invalid status";
				break;
			}

			return msg;
		}
	}
}
