//
// registry.cpp - registry implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../registry.h"
#include "../leccore_common.h"
#include <Windows.h>
#include <comdef.h>
#include <vector>

using namespace liblec::leccore;

class registry::impl {
public:
	const HKEY root_;

	impl(scope registry_scope) :
		root_(registry_scope == scope::local_machine ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER) {}
	~impl() {}

	// adapted from https://msdn.microsoft.com/en-us/library/windows/desktop/ms724256(v=vs.85).aspx
	bool registry_enumerate(HKEY root, const std::string& path,
		std::vector<std::string>& sub_keys,
		std::vector<std::string>& values,
		std::string& error) {
		sub_keys.clear();
		values.clear();

		const int MAX_KEY_LENGTH = 255;
		const int MAX_VALUE_NAME = 16383;

		HKEY h_key = NULL;

		LONG result = RegOpenKeyExA(root, path.c_str(),
			0, KEY_READ, &h_key);

		if (result != ERROR_SUCCESS) {
			RegCloseKey(h_key);

			_com_error err(result);
			error = convert_string(err.ErrorMessage());
			return false;
		}

		CHAR     achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		CHAR     achClass[MAX_PATH] = "";  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys = 0;             // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;                  // number of values for key 
		DWORD    cchMaxValue;              // longest value name 
		DWORD    cbMaxValueData;           // longest value data 
		DWORD    cbSecurityDescriptor;     // size of security descriptor 
		FILETIME ftLastWriteTime;          // last write time 

		DWORD i;

		CHAR  achValue[MAX_VALUE_NAME];
		DWORD cchValue = MAX_VALUE_NAME;

		// Get the class name and the value count
		result = RegQueryInfoKeyA(
			h_key,                   // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

		// Enumerate the subkeys, until RegEnumKeyEx fails
		if (result != ERROR_SUCCESS) {
			RegCloseKey(h_key);

			_com_error err(result);
			error = convert_string(err.ErrorMessage());
			return false;
		}

		if (cSubKeys) {
			sub_keys.resize(cSubKeys);

			for (i = 0; i < cSubKeys; i++) {
				cbName = MAX_KEY_LENGTH;
				result = RegEnumKeyExA(h_key, i, achKey, &cbName,
					NULL, NULL, NULL, &ftLastWriteTime);

				if (result == ERROR_SUCCESS)
					sub_keys[i] = achKey;
			}
		}

		// Enumerate the key values
		if (cValues) {
			values.resize(cValues);

			for (i = 0, result = ERROR_SUCCESS; i < cValues; i++) {
				cchValue = MAX_VALUE_NAME;
				achValue[0] = '\0';
				result = RegEnumValueA(h_key, i, achValue, &cchValue,
					NULL, NULL, NULL, NULL);

				if (result == ERROR_SUCCESS)
					values[i] = achValue;
			}
		}

		RegCloseKey(h_key);
		return true;
	}
};

registry::registry(scope registry_scope) : d_(*new impl(registry_scope)) {}
registry::~registry() { delete& d_; }

bool registry::do_read(const std::string& path, const std::string& value_name,
	std::string& value, std::string& error) {
	HKEY h_key;
	LONG result = RegOpenKeyExA(d_.root_, path.c_str(),
		0, KEY_QUERY_VALUE, &h_key);

	if (result != ERROR_SUCCESS) {
		_com_error err(result);
		error = convert_string(err.ErrorMessage());
		return false;
	}

	DWORD type;

	/*
	** determine size of buffer
	** If lpData is NULL, and lpcbData is non-NULL, the function returns ERROR_SUCCESS
	** and stores the size of the data, in bytes, in the variable pointed to by lpcbData.
	** This enables an application to determine the best way to allocate a buffer for the value's data.
	** https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911(v=vs.85).aspx
	*/
	DWORD len = 1;

	result = RegQueryValueExA(h_key, value_name.c_str(),
		0, &type, NULL, &len);

	/*
	** Q: Why does the RegQueryValueEx() always return ERROR_MORE_DATA?
	** A: If the lpData buffer is too small to receive the data, the function returns ERROR_MORE_DATA.
	** Tips:
	** When you are using RegQueryValueExW(), the parameter of lpcbData
	** SHOULD be the array size in BYTES, not in characters.
	** from: http://nogeekhere.blogspot.com/2008/08/why-did-regqueryvalueex-return.html
	** MSDN Reference:
	** https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911(v=vs.85).aspx
	*/
	int string_length = len / sizeof(CHAR);
	CHAR* buf = new CHAR[string_length];

	result = RegQueryValueExA(h_key, value_name.c_str(),
		0, &type, (BYTE*)buf, &len);

	RegCloseKey(h_key);

	if (result != ERROR_SUCCESS) {
		if (buf) {
			delete[] buf;
			buf = nullptr;
		}

		_com_error err(result);
		error = convert_string(err.ErrorMessage());
		return false;
	}

	if (string_length > 0)
		value = std::string(buf, string_length - 1);	// -1 in order to exclude the NULL terminator (/0)

	if (buf) {
		delete[] buf;
		buf = nullptr;
	}

	return true;
}

bool registry::do_write(const std::string& path,
	const std::string& value_name, const std::string& value, std::string& error) {
	const std::string sub_key = path;

	HKEY h_key;
	LONG result = RegCreateKeyExA(d_.root_, sub_key.c_str(),
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE,
		NULL, &h_key, NULL);

	if (result != ERROR_SUCCESS) {
		_com_error err(result);
		error = convert_string(err.ErrorMessage());
		return false;
	}

	result = RegSetValueExA(h_key, value_name.c_str(),
		0, REG_SZ, (const BYTE*)value.c_str(),
		(strlen(value.c_str()) + 1) * sizeof(CHAR));

	RegCloseKey(h_key);

	if (result != ERROR_SUCCESS) {
		_com_error err(result);
		error = convert_string(err.ErrorMessage());
		return false;
	}

	return true;
}

bool registry::do_delete(const std::string& path,
	const std::string& value_name, std::string& error) {
	HKEY h_key;
	LONG result = RegOpenKeyExA(d_.root_, path.c_str(),
		0, KEY_ALL_ACCESS, &h_key);

	if (result == ERROR_SUCCESS) {
		if (value_name.empty())
			result = RegDeleteKeyA(h_key, "");
		else
			result = RegDeleteValueA(h_key, value_name.c_str());

		if (result != ERROR_SUCCESS) {
			_com_error err(result);
			error = convert_string(err.ErrorMessage());
			return false;
		}

		return true;
	}
	else
		if (result != ERROR_FILE_NOT_FOUND) {
			_com_error err(result);
			error = convert_string(err.ErrorMessage());
			return false;
		}

	return true;
}

void registry::do_recursive_delete(const std::string& sub_key) {
	std::vector<std::string> sub_keys, values;
	std::string error;
	d_.registry_enumerate(d_.root_, sub_key, sub_keys, values, error);

	for (auto& it : sub_keys)
		do_recursive_delete(sub_key + "\\" + it);

	for (auto& it : values)
		do_delete(sub_key, it, error);

	do_delete(sub_key, "", error);
}