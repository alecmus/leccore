//
// download.cpp - file download implementation
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

#include "download.h"
#include "../error/win_error.h"
#include "../versioninfo.h"

#include <Windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

class inet_handle {
	HINTERNET _handle;
public:
	inet_handle(HINTERNET handle) :
		_handle(handle) {}

	~inet_handle() {
		if (_handle)
			InternetCloseHandle(_handle);
	}

	operator HINTERNET() const { return _handle; }
};

std::string make_user_agent() {
	std::string user_agent = std::string(leccorename) + " " + std::string(leccoreversion);

#ifdef _WIN64
	user_agent += " (Win64)";
#else
	// Check if the 32bit process is running on a 64bit OS
	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS is_wow64_process =
		(LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleA("kernel32"), "IsWow64Process");

	if (is_wow64_process) {
		BOOL wow64 = FALSE;
		is_wow64_process(GetCurrentProcess(), &wow64);
		if (wow64)
			user_agent += " (WOW64)";
	}

#endif
	return user_agent;
}

bool get_http_header(HINTERNET handle, DWORD what_to_get, DWORD& output) {
	DWORD output_size = sizeof(output);
	DWORD header_index = 0;
	return HttpQueryInfoA(
		handle,
		what_to_get | HTTP_QUERY_FLAG_NUMBER,
		&output,
		&output_size,
		&header_index
	) == TRUE;
}

std::string get_url_filename(const URL_COMPONENTSA& urlc) {
	const char* last_slash = strrchr(urlc.lpszUrlPath, '/');
	return std::string(last_slash ? last_slash + 1 : urlc.lpszUrlPath);
}

bool liblec::leccore::download(const std::string& url,
	download_sink& sink, bool cache_data, std::string& error) {
	char url_path[512];
	URL_COMPONENTSA urlc;
	memset(&urlc, 0, sizeof(urlc));
	urlc.dwStructSize = sizeof(urlc);
	urlc.lpszUrlPath = url_path;
	urlc.dwUrlPathLength = sizeof(url_path);

	if (!InternetCrackUrlA(url.c_str(), 0, ICU_DECODE, &urlc)) {
		error = get_last_error();
		return false;
	}

	inet_handle inet = InternetOpenA(
		make_user_agent().c_str(),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, // lpszProxyName
		NULL, // lpszProxyBypass
		0     // dwFlags
	);

	if (!inet) {
		error = get_last_error();
		return false;
	}

	DWORD dwFlags = 0;
	if (!cache_data)
		dwFlags |= INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD;
	if (urlc.nScheme == INTERNET_SCHEME_HTTPS)
		dwFlags |= INTERNET_FLAG_SECURE;

	inet_handle conn = InternetOpenUrlA(
		inet,
		url.c_str(),
		NULL, // lpszHeaders
		-1,   // dwHeadersLength
		dwFlags,
		NULL  // dwContext
	);

	if (!conn) {
		error = get_last_error();
		return false;
	}

	char buffer[10240];

	// check returned status code - we need to detect 404 instead of
	// downloading the human-readable 404 page
	DWORD status_code;
	if (get_http_header(conn, HTTP_QUERY_STATUS_CODE, status_code) && status_code >= 400) {
		error = "Update file not found on the server";
		return false;
	}

	// get content length, if possible
	DWORD content_length;
	if (get_http_header(conn, HTTP_QUERY_CONTENT_LENGTH, content_length))
		sink.set_length(content_length);

	// get filename, if available
	char content_disposition[512];
	DWORD cdSize = 512;
	bool filename_set = false;
	if (HttpQueryInfoA(conn, HTTP_QUERY_CONTENT_DISPOSITION,
		content_disposition, &cdSize, NULL)) {
		char* ptr = strstr(content_disposition, "filename=");
		if (ptr) {
			char c_filename[512];
			ptr += 9;
			while (*ptr == ' ')
				ptr++;

			bool quoted = false;
			if (*ptr == '"' || *ptr == '\'') {
				quoted = true;
				ptr++;
			}

			char* ptr2 = c_filename;
			while (*ptr != ';' && *ptr != 0)
				*ptr2++ = *ptr++;

			if (quoted)
				*(ptr2 - 1) = 0;
			else
				*ptr2 = 0;

			if (!sink.set_filename(c_filename, error))
				return false;

			filename_set = true;
		}
	}

	if (!filename_set) {
		if (!sink.set_filename(get_url_filename(urlc), error))
			return false;
	}

	// download the data
	while (true) {
		DWORD read;
		if (!InternetReadFile(conn, buffer, sizeof(buffer), &read)) {
			error = get_last_error();
			return false;
		}

		if (read == 0)
			break;	// download complete

		if (!sink.add_chunk(buffer, read, error))
			return false;
	}

	return true;
}
