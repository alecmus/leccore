//
// win_error.cpp - windows error implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "win_error.h"
#include <Windows.h>

std::string get_last_error() {
	std::string last_error;
	const DWORD dw_last_error = GetLastError();

	if (dw_last_error != 0) {
		LPSTR buffer;
		if (FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM,	// retrieve message from system
			NULL,
			dw_last_error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&buffer, 0, NULL)) {
			last_error = buffer;
			LocalFree(buffer);
		}
	}

	if (last_error.empty()) {
		LPSTR buffer;
		if (FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_IGNORE_INSERTS
			| FORMAT_MESSAGE_FROM_HMODULE,	// retrieve message from specified DLL
			GetModuleHandleA("wininet.dll"),
			dw_last_error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&buffer, 0, NULL)) {
			last_error = buffer;
			LocalFree(buffer);
		}
	}

	return last_error;
}
