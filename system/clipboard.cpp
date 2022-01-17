//
// clipboard.cpp - clipboard implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../system.h"
#include "../leccore_common.h"
#include "../error/win_error.h"
#include <Windows.h>

namespace liblec {
	namespace leccore {
		const bool clipboard::set_text(const std::string& text, std::string& error) {
			error.clear();

			std::wstring w_text = convert_string(text);
			LPCWSTR buffer = w_text.c_str();

			SIZE_T buffer_length = wcslen(buffer);
			HGLOBAL h_dst = nullptr;
			LPWSTR dst = nullptr;

			h_dst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (buffer_length + 1) * sizeof(WCHAR));

			if (!h_dst) {
				error = get_last_error();
				return false;
			}

			dst = (LPWSTR)GlobalLock(h_dst);

			if (dst) {
				memcpy(dst, buffer, buffer_length * sizeof(WCHAR));
				dst[buffer_length] = 0;
				GlobalUnlock(h_dst);

				if (!OpenClipboard(NULL)) {
					error = get_last_error();
					return false;
				}

				EmptyClipboard();

				if (!SetClipboardData(CF_UNICODETEXT, h_dst)) {
					error = get_last_error();
					CloseClipboard();
					return false;
				}

				CloseClipboard();
			}

			return true;
		}

		const bool clipboard::get_text(std::string& text, std::string& error) {
			text.clear();
			error.clear();

			HANDLE handle;
			if (!OpenClipboard(NULL)) {
				error = get_last_error();
				return false;
			}

			handle = GetClipboardData(CF_UNICODETEXT);
			if (handle == NULL) {
				error = get_last_error();
				CloseClipboard();
				return false;
			}

			LPCWSTR buffer = nullptr;
			DWORD length = 0;

			buffer = (LPCWSTR)GlobalLock(handle);
			length = (DWORD)GlobalSize(handle);

			if (buffer) {
				std::wstring w_text(buffer, length);
				text = convert_string(w_text);
			}

			return true;
		}
	}
}
