//
// app_version_info.cpp - application version information implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../app_version_info.h"
#include "../leccore_common.h"
#include "../error/win_error.h"

#include <Windows.h>
#include <comdef.h>	// for _com_error

#include <WinVer.h>
#pragma comment(lib, "version.lib")

using namespace liblec::leccore;

class app_version_info::impl {
public:
	HMODULE _h_module = nullptr;
	std::string _app_name;
	std::string _app_version;
	std::string _company_name;

	impl() = delete;
	impl(const std::string& app_name,
		const std::string& app_version,
		const std::string& company_name) :
		_app_name(app_name),
		_app_version(app_version),
		_company_name(company_name),
		_h_module(GetModuleHandle(nullptr)) {}
	~impl() {}

	// Helper class for RIIA handling of allocated buffers
	struct data_buffer {
		data_buffer(size_t size) {
			data = new unsigned char[size];
			memset(data, 0, size);
		}
		~data_buffer() { delete[] data; }
		unsigned char* data;
	};

	// Get language code for the resource to query
	bool get_version_info_language(void* fi,
		std::string& language,
		std::string& error) {
		struct translation_info {
			WORD language;
			WORD codepage;
		} *p_translations;

		unsigned translations_count;

		if (!VerQueryValueA(fi,
			"\\VarFileInfo\\Translation", (LPVOID*)&p_translations, &translations_count)) {
			error = "Executable doesn't have required VERSIONINFO\\VarFileInfo resource";
			return false;
		}

		translations_count /= sizeof(struct translation_info);

		if (translations_count == 0) {
			error = "No translations in VarFileInfo resource?";
			return false;
		}

		// TODO: be smarter about which language to use:
		//       1. use app main thread's locale
		//       2. failing that, try language-neutral or English
		//       3. use the first one as fallback
		const size_t idx = 0;

		char lang[9];
		HRESULT result = _snprintf_s(
			lang, 9, 8, "%04x%04x",
			p_translations[idx].language,
			p_translations[idx].codepage);

		if (FAILED(result)) {
			_com_error err(result);
			error = convert_string(err.ErrorMessage());
			return false;
		}

		language = lang;
		return true;
	}

	bool get_version_info_field(const std::string& field,
		std::string& value, std::string& error) {
		char exe_file_name[MAX_PATH + 1];

		if (!GetModuleFileNameA(_h_module, exe_file_name, MAX_PATH)) {
			error = get_last_error();
			return false;
		}

		DWORD unused_handle;
		DWORD version_info_size = GetFileVersionInfoSizeA(exe_file_name, &unused_handle);

		if (version_info_size == 0) {
			error = "Executable doesn't have the required VERSIONINFO resource";
			return false;
		}

		data_buffer fi(version_info_size);

		if (!GetFileVersionInfoA(exe_file_name, 0, version_info_size, fi.data)) {
			error = get_last_error();
			return false;
		}

		std::string language;
		if (!get_version_info_language(fi.data, language, error))
			return false;

		const std::string key = "\\StringFileInfo\\" + language + "\\" + field;
		LPSTR key_str = (LPSTR)key.c_str(); // explicit cast to work around VC2005 bug

		CHAR* _value;
		UINT len;

		if (!VerQueryValueA(fi.data, key_str, (LPVOID*)&_value, &len)) {
			error = "Executable doesn't have required key in StringFileInfo";
			return false;
		}

		value = _value;
		return true;
	}
};

app_version_info::app_version_info() :
	app_version_info("", "", "") {}

app_version_info::app_version_info(const std::string& app_name,
	const std::string& app_version,
	const std::string& company_name) :
	_d(*new impl(app_name, app_version, company_name)) {}

bool app_version_info::get_app_name(std::string& app_name, std::string& error) {
	if (_d._app_name.empty()) {
		if (!_d.get_version_info_field("ProductName", _d._app_name, error))
			return false;
	}
	app_name = _d._app_name;
	return true;
}

bool app_version_info::get_app_version(std::string& app_version, std::string& error) {
	if (_d._app_version.empty()) {
		if (!_d.get_version_info_field("FileVersion", _d._app_version, error))
			return false;
	}
	app_version = _d._app_version;
	return true;
}

bool app_version_info::get_company_name(std::string& company_name, std::string& error) {
	if (_d._company_name.empty()) {
		if (!_d.get_version_info_field("CompanyName", _d._company_name, error))
			return false;
	}
	company_name = _d._company_name;
	return true;
}
